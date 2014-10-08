define(["jquery", "underscore", "backbone", "chessboard", "moveview", "game"],
function ($, _, backbone, chessboard, MoveView, game) {
"use strict";

var GameView = Backbone.View.extend({
    initialize: function() {
        this.board = new ChessBoard('board', 'start');

        var gameModel = new Backbone.Model(game);
        this.moveView = new MoveView({
            el: $("#move-list"),
            model: gameModel
        });

        gameModel.on("selected: move", function(move) {
            this.board.position(move.fen);
        }, this);


        // set original position on load
        //$board.data("top", $board.offset().top); 
        //$moves.data("left", $moves.offset().left);
        //$(window).scroll(function() { fixDiv($board, $moves); });
    },

    render: function() {
        this.moveView.render();
        return this;
    },

    fix_board: function() {
        var scrollTop = $(window).scrollTop();
        if (scrollTop > $board.data("top")) { 
            $board.css({'position': 'fixed', 'top': '0'});
            $moves.css({'margin-left': $moves.data('left') - 78});
        } else {
            $board.css({'position': 'static', 'top': $board.data("top") - scrollTop});
            $moves.css({'margin-left': 0});
        }
    }
});

return GameView;
});

