import argparse
import subprocess
from pystockfish import Engine
import pgn
import json
from datetime import datetime

pgn_extract_path = "./external/pgn-extract/pgn-extract"
stockfish_path = "./external/stockfish/src/stockfish"

parser = argparse.ArgumentParser(description="Analyze a game")
parser.add_argument("gamefile", help="PGN file containing game to analyze")
args = parser.parse_args()

f = open(args.gamefile, 'r')
pgn_text = f.read()
f.close()

formatted_uci = subprocess.check_output([pgn_extract_path, "-Wuci", args.gamefile])

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
f = open('output.json', 'w')
f.write(json_text)
f.close()

