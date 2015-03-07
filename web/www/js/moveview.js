define(["jquery", "underscore", "backbone"],
function($, _, Backbone) {
"use strict";

var MoveView = Backbone.View.extend({
    template: _.template($("script.moves-template").html()),

    events: {
        "click .moves-table tr": "move_click",
        "keydown": "key_action",
        "click .best-line-link": "best_line_click"
    },

    initialize: function() {
        this.selected_move = -1;
        $(window).keydown(_.bind(this.key_action, this));
    },

    render: function() {
        var html = this.template(this.model.attributes);
        this.$el.html(html);
        return this;
    },

    move_click: function(event) {
        // -1 for header
        event.stopPropagation();
        if (event.currentTarget.rowIndex % 2 == 0) {
            return;
        }

        var move_number = event.currentTarget.getAttribute("data-move");
        this.select_move(move_number); 
        var $target = $(event.target);
        if (!($target.closest("td").attr("colspan") > 1)) {
            var next = $target.closest("tr").next();
            if (next.css("display") === "none") {
                next.css("display", "table-row");
                next.find("p").slideToggle();
            } else {
                next.find("p").slideToggle({done: function() {next.toggle();}});
            }
        }
    },

    best_line_click: function(event) {
        var $movelink = $(event.currentTarget);
        var move = $movelink.attr("data-basemove");
        var submove = $movelink.attr("data-submove");
        var move_data  = this.model.attributes.positions[move];
        var prevmove = this.model.attributes.positions[move - 1];
        this.model.trigger("selected:sub-move", move_data, submove, prevmove);
    },

    key_action: function(e) {
        var LEFT = 37;
        var RIGHT = 39;
        var J = 74;
        var K = 75;
        if (e.keyCode == LEFT || e.keyCode == J) {
            this.select_move(this.selected_move - 1);
        } else if (e.keyCode == RIGHT || e.keyCode == K) {
            this.select_move(this.selected_move + 1);
        }
    },

    select_move: function(move) {
        if (move < 0) {
            move = 0;
        } else if (this.model.attributes.positions.length - 1 < move) {
            move = this.model.attributes.positions.length - 1;
        }

        if (this.selected_move > -1) {
            // reset css on current selected move
            this.$('.moves-table tr:nth-child(' + (this.selected_move * 2 + 1) + ')').css({'background-color': ''});
        }

        var selector = '.moves-table tr:nth-child(' + (move * 2 + 1) + ')';
        this.$(selector).css({'background-color': '#87DFFA'});

        this.selected_move = move;

        var move_data  = this.model.attributes.positions[move];
        var prevmove = this.model.attributes.positions[move - 1];
        this.model.trigger("selected:move", move_data, prevmove);
    }
});

return MoveView;
});
