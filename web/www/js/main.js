require([
    'jquery',
    'backbone',
    'gameview',
    'annotator'
], function (
    $,
    Backbone,
    GameView,
    Annotator) {
    "use strict";

    function getParameterByName(name) {
        name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
        var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
        results = regex.exec(location.search);
        return results === null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
    }

    function game_loaded(game) {
        game.positions.forEach(function(position, i, positions) {
            var prevFen = i > 0 ? positions[i - 1].fen : null;
            position.best_move_san = Annotator.squareToSan(prevFen, [position.details.best_move.move]);
        });

        var gameModel = new Backbone.Model(game);
        var gameview = new GameView({
            el: $("body"),
            model: gameModel});
        gameview.render();
     }

    var game_path = "games/" + getParameterByName("game");
    $.get(game_path, game_loaded)
        .fail(function () {
            // For local development
            require(["game"], game_loaded);
        });
});

