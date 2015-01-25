#!/bin/bash

cd /var/www/chessanalyze/
python analyze.py --email --thinktime=7000 --pgnextract=/var/www/chessanalyze/external/pgn-extract/pgn-extract --stockfish=/var/www/chessanalyze/external/stockfish/src/stockfish

