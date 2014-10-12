require(['gameview', 'game'], function (GameView, game) {
    "use strict";
    var gameModel = new Backbone.Model(game);
    var gameview = new GameView({
        el: $("body"),
        model: gameModel});
    gameview.render();
});

