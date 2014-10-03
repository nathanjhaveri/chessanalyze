import argparse
import subprocess
from pystockfish import Engine
import pgn

parser = argparse.ArgumentParser(description="Analyze a game")
parser.add_argument("pgn", help="PGN file containing game to analyze")

args = parser.parse_args()
args.pgn


pgn_extract_path = "./external/pgn-extract/pgn-extract"
stockfish_path = "./external/stockfish/src/stockfish"

uci = subprocess.check_output([pgn_extract_path, "-Wuci", args.pgn])
game = pgn.loads(uci)[0]

engine = Engine(stockfish_path, depth=20)
moves = []
for move in game.moves:
    moves.append(move)
    engine.setposition(moves)
    print engine.bestmove()

