python analyze.py games/nnnnnnnn-vs-Gogogorosh.2014.04.11.pgn; echo "define(function(){var game=" > web/js/game.js; cat output.json | python -mjson.tool >> web/js/game.js; echo "; return game;});" >> web/js/game.js
