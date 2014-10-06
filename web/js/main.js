require([
"jquery",
"underscore",
"backbone",
"chessboard-0.3.0.min",
"MoveView",
"game"
], function ($, _, backbone, chessboard, MoveView, game) {
    var board = new ChessBoard('board', 'start');
    var gameModel = new Backbone.Model(game);
    var moveView = new MoveView({
        el: $("#move-list"),
        model: gameModel
    });
    moveView.render();

    gameModel.on("move-selected", function(move) {
        board.position(move.fen);
    });
});
