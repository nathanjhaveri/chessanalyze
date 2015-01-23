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

        var line = anal.next_moves.join(', ');
        this.$("#best-line").text("Best line: " + line);

        return this;
    },
});

return MoveDetailsView;
});

