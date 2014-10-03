pgn-extract c code that seems to do the job of converting PGN to uci

./pgn-extract  -Wuci nnnnnnnn-vs-Gogogorosh.2014.04.11.pgn 


TODO:
- Write script to go from pgn to uci and feed input to stockfish
- output move by move analysis 
- store analysis, figure out data structures for chess games.  Mabe store everything raw in json blob and figure out later. 
- Web UI
- Async game processing.  RabbitMQ, python
- Auto sync games from chesstime
- Stockfish for analysis

