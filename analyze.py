import sys
import argparse
from pystockfish import Engine
import pgn
import json
from datetime import datetime
from subprocess import Popen, PIPE

pgn_extract_path = "./external/pgn-extract/pgn-extract"
stockfish_path = "./external/stockfish/src/stockfish"

parser = argparse.ArgumentParser(description="Analyze a game")
parser.add_argument('gamefile', help="PGN file containing game to analyze", nargs='?', type=argparse.FileType('r'), default=sys.stdin)
parser.add_argument("--outfile", help="JSON file to output analysis")

args = parser.parse_args()

pgn_text = args.gamefile.read()
args.gamefile.close()

p = Popen([pgn_extract_path, "-Wuci"], stdin=PIPE, stdout=PIPE, stderr=PIPE)
formatted_uci, stderr = p.communicate(pgn_text)

game_pgn = pgn.loads(pgn_text)[0]
game = pgn.loads(formatted_uci)[0]

game_analysis = {
    'white': game_pgn.white,
    'black': game_pgn.black,
    'event': game.event,
    'date': game.date,
    'analysis_date': str(datetime.now()),
    'positions': [],
}

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
        'details': engine.go_time(1),
    }

    move = game.moves[i]
    moves.append(move)
    engine.setposition(moves)

    position['fen'] = engine.fen()
    game_analysis['positions'].append(position)

json_text = json.dumps(game_analysis)

if args.outfile is None:
    outfile_name = "{} vs {} on {}.json".format(
            game_analysis['white'],
            game_analysis['black'],
            game_analysis['date'])
else:
    outfile_name = args.outfile

f = open(outfile_name, 'w')
f.write(json_text)
f.close()

