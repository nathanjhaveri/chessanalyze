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
            var animate = (i == submove);
            var next_move = next_moves[i];
            var sq_from = next_move.substring(0, 2);
            var sq_to = next_move.substring(2, 4);
            var fmt_move = sq_from + "-" + sq_to;


            // There are three move cases:  Regular move (including capture), 
            // casting and pawn promotion.  In the case of castling or
            // promotion, we need to make an additional modification to the
            // board, chessboard.js does not handle those cases automatically.
            var position = this.board.position();

            if (next_move.length === 5) { 
                // Pawn promotion case
                var promoted_piece = next_move[4].toUpperCase();
                position[sq_from] = position[sq_from][0] + promoted_piece;
                this.board.position(position);
            } else if (position[sq_from] == "bK" && sq_from == "e8") {
                if (sq_to == "g8") {
                    this.board.move("h8-f8", animate);
                } else if (sq_to == "c8") {
                    this.board.move("a8-d8", animate);
                }
            } else if (position[sq_from] == "wK" && sq_from == "e1") {
                if (sq_to == "g1") {
                    this.board.move("h1-f1", animate);
                } else if (sq_to == "c1") {
                    this.board.move("a1-d1", animate);
                }
            }

            this.board.move(fmt_move, animate);
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

