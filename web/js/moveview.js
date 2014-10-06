define(["jquery", "underscore", "backbone"],
function($, _, Backbone) {
"use strict";

var MoveView = Backbone.View.extend({
    template: _.template($("script.moves-template").html()),

    events: {
        "click #moves-table tr": "move_click"
    },

    initialize: function() {
    },

    render: function() {
        var html = this.template(this.model.attributes);
        this.$el.html(html);
        return this;
    },

    move_click: function(event) {
        var rowIndex = event.currentTarget.rowIndex;
        var moveData  = this.model.attributes.positions[rowIndex];
        this.model.trigger("move-selected", moveData);
    }
});

return MoveView;
});
