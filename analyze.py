import argparse
import subprocess
from pystockfish import Engine
import pgn
import json

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
    'positions': []
}

engine = Engine(stockfish_path)
moves = []
for i in range(len(game.moves)):
    move = game.moves[i]
    move_pgn = game_pgn.moves[i]
    moves.append(move)
    engine.setposition(moves)
    white_to_move = i % 2 == 0

    game_analysis['positions'].append({
        'move_number': (i+2) // 2,
        'side': 'white' if white_to_move else 'black',
        'move': move_pgn,
        'fen': engine.fen(),
        'details': engine.go_time(100),
    })

json_text = json.dumps(game_analysis)
f = open('output.json', 'w')
f.write(json_text)
f.close()

