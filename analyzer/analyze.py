import sys
import argparse
from pystockfish import Engine
import pgn
import json
from datetime import datetime
from subprocess import Popen, PIPE
from emailparse import parse_email


# For external programs
def analyze(pgn, thinktime, outfile):
    # Assume pgn-extract and stockfish are on the path
    from distutils import spawn
    stockfish_path = spawn.find_executable("stockfish")
    pgnextract_path = spawn.find_executable("pgn-extract")

    main(pgn, thinktime, outfile, stockfish_path, pgnextract_path)


def main(pgn_text, thinktime, outfile, stockfish, pgnextract):
    game_pgn, game = parse_game(pgn_text, pgnextract)
    game_analysis = analyze_game(game, game_pgn, thinktime, stockfish)
    write_game(game_analysis, outfile)


def parse_args():
    parser = argparse.ArgumentParser(description="Analyze a game")
    parser.add_argument('gamefile', help="PGN file containing game to analyze", nargs='?', type=argparse.FileType('r'), default=sys.stdin)
    parser.add_argument("--outfile", help="JSON file to output analysis")
    parser.add_argument("--email", default=False, help="Parse email input", action="store_true")
    parser.add_argument("--thinktime", default=10000, help="Engine think time per move")
    parser.add_argument("--pgnextract", default="./external/pgn-extract/pgn-extract")
    parser.add_argument("--stockfish", default="./external/stockfish/src/stockfish")

    return parser.parse_args()


def parse_game(pgn_text, pgn_extract_path):
    p = Popen([pgn_extract_path, "-Wuci"], stdin=PIPE, stdout=PIPE, stderr=PIPE)
    formatted_uci, stderr = p.communicate(pgn_text)

    game_pgn = pgn.loads(pgn_text)[0]
    game = pgn.loads(formatted_uci)[0]

    return game_pgn, game


def analyze_game(game, game_pgn, thinktime, stockfish_path):
    game_analysis = {}
    game_analysis['white'] = game_pgn.white
    game_analysis['black'] = game_pgn.black
    game_analysis['event'] = game.event
    game_analysis['date'] = game.date
    game_analysis['analysis_date'] = str(datetime.now())
    game_analysis['positions'] = []

    engine = Engine(stockfish_path)
    moves = []
    for i in range(len(game.moves)):
        move_pgn = game_pgn.moves[i]
        white_to_move = i % 2 == 0

        move_str = str((i+2) // 2)
        if white_to_move:
            move_str = move_str + "."
        else:
            move_str = move_str + "..."

        position = {
            'move_number': move_str,
            'side': 'white' if white_to_move else 'black',
            'move': move_pgn,
            'details': engine.go_time(thinktime),
        }

        move = game.moves[i]
        moves.append(move)
        engine.setposition(moves)

        position['fen'] = engine.fen()
        game_analysis['positions'].append(position)

    return game_analysis


def write_game(game_analysis, outfile):
    json_text = json.dumps(game_analysis)

    if outfile is None:
        outfile_name = "games/{} vs {} on {}.{}.json".format(
                game_analysis['white'],
                game_analysis['black'],
                game_analysis['date'],
                game_analysis['analysis_date'].replace(":", "-").replace(" ", "-"))
    else:
        outfile_name = outfile

    f = open(outfile_name, 'w')
    f.write(json_text)
    f.close()


if __name__ == '__main__':
    args = parse_args()

    rawin = args.gamefile.read()
    args.gamefile.close()

    pgn_text = None
    if args.email:
        message = parse_email(rawin)
        pgn_text = message.body
    else:
        pgn_text = rawin
    args.pgnextract
    args.thinktime
    args.stockfish
    args.outfile

    main(pgn_text, args.thinktime, args.outfile, args.stockfish, args.pgnextract)

