define([
    "jquery",
    "underscore",
    "backbone",
    "chessboard",
    "moveview",
    "movedetailsview"],
function ($, _, backbone, chessboard, MoveView, MoveDetailsView) {
"use strict";

var GameView = Backbone.View.extend({
    initialize: function() {
        var board_cfg = {
            draggable: true,
            dropOffBoard: 'snapback',
            position: 'start'
        };

        this.board = new ChessBoard('board', board_cfg);

        this.moveView = new MoveView({
            el: $("#move-list"),
            model: this.model
        });

        this.movmeDetailsView = new MoveDetailsView({
            el: $("#analysis"),
            model: this.model
        });

        this.model.on("selected: move", this.move_selected, this);
    },

    render: function() {
        this.moveView.render();
        var title = this.model.get("white") + " VS. " + this.model.get("black");
        this.$("#matchup").text(title);
        this.$("#event").text(this.model.get("event"));
        this.$("#date").text(this.model.get("date"));

        return this;
    },

    move_selected: function(move) {
        this.board.position(move.fen);
    }
});

return GameView;
});

