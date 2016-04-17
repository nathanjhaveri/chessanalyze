// Given a game file, this adds extra information
define(['chess-0.9.4.min'], function (chess) {
'use strict';

var Annotator = {
    // Given a starting FEN, and a list of moves in square notation,
    // returns the list of moves in SAN notation for easy output.
    // E.g. e2e4 gets converted to e4
    squareToSan: function (fen, moves) {
        var chess = fen ? new Chess(fen) : new Chess();
        var sanMoves = moves.map(function(move) {
            if (move === '(none)') {
                return null;
            }

            var moveObj = chess.move({
                from: move.substring(0, 2),
                to: move.substring(2,4)
            });

            if (move.length === 5) {
                moveObj.promotion = move[4];
            }

            return moveObj ? moveObj.san : null;
        });

        return sanMoves;
    }
}

return Annotator;
});

