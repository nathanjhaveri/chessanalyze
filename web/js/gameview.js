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

        this.$window.on('scroll', _.bind(this.onScroll, this));
        this.$window.on('resize', _.bind(this.resize, this));
        this.model.on("selected: move", this.move_selected, this);
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

    move_selected: function(move) {
        this.board.position(move.fen);
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

