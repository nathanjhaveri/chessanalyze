define([
    "jquery",
    "underscore",
    "backbone",
    "chessboard",
    "moveview"],
function ($, _, backbone, chessboard, MoveView) {
"use strict";

var GameView = Backbone.View.extend({
    $body: $("body"),
    $window: $(window),
    $board: $("#board"),
    dockclass: "has-docked-board",
    board_offset: undefined,

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

        $("#flip").click(_.bind(this.board.flip, this.board));
        this.$window.on('scroll', _.bind(this.onScroll, this));
        this.$window.on('resize', _.bind(this.resize, this));
        this.model.on("selected:move", this.move_selected, this);
        this.model.on("selected:sub-move", this.sub_move_selected, this);
    },

    render: function() {
        this.moveView.render();
        var title = this.model.get("white") + " VS. " + this.model.get("black");
        this.$("#matchup").text(title);
        this.$("#event").text(this.model.get("event"));
        this.$("#date").text(this.model.get("date"));
        this.$("#white-name").text("White: " + this.model.get("white"));
        this.$("#black-name").text("Black: " + this.model.get("black"));

        this.resize();
        return this;
    },

    move_selected: function(move, prevmove) {
        if (prevmove) {
            this.board.position(prevmove.fen, false);
        } else {
            this.board.start(false);
        }

        this.board.position(move.fen, true);
    },

    sub_move_selected: function(move, submove, prevmove) {
        if (prevmove) {
            this.board.position(prevmove.fen, false);
        } else {
            this.board.start(false);
        }

        var next_moves = move.details.analysis.next_moves;
        for (var i = 0; i <= submove; i++) {
            var next_move = next_moves[i];
            var fmt_move = next_move.substring(0, 2) + "-" + next_move.substring(2, 4);
            this.board.move(fmt_move, i == submove);
        }
    },

    resize: function() {
        this.$body.removeClass(this.dockclass);
        this.board_offset = this.$board.offset().top;
        $("#board-spacer").height(this.$board.height());
        this.board.resize();
        this.onScroll();
    },

    onScroll: function() {
        if (this.board_offset < this.$window.scrollTop() &&
                !this.$body.hasClass(this.dockclass)) {
            this.$body.addClass(this.dockclass);
        }

        if (this.board_offset > this.$window.scrollTop() &&
                this.$body.hasClass(this.dockclass)) {
            this.$body.removeClass(this.dockclass);
        }
    }
});

return GameView;
});

