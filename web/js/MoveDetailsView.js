define(["jquery", "backbone"],
function($,        Backbone) {
var MoveDetailsView = Backbone.View.extend({
    initialize: function() {
        this.model.on("selected: move", this.render, this);
    },

    render: function(move) {
        var anal = move.details.analysis;
        this.$("#move-no").text(move.move_number);
        this.$("#move").text(move.move);
        this.$("#score").text("Score: " + anal.score);
        this.$("#depth").text("Depth: " + anal.depth);
        this.$("#best-line").text("Best line: " + anal.next_moves);

        return this;
    },
});

return MoveDetailsView;
});

