require(['jquery', 'gameview'], function ($, GameView) {
    "use strict";

    function getParameterByName(name) {
        name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
        var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
        results = regex.exec(location.search);
        return results === null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
    }

    var game_path = "/chess/games/" + getParameterByName("game");
    $.get(game_path, function(game) {
        var gameModel = new Backbone.Model(game);
        var gameview = new GameView({
            el: $("body"),
            model: gameModel});
        gameview.render();
     });
});

