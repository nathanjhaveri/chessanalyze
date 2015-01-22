require(["jquery"], function($) {
    $.get("/chess/games/", function(dirhtml) { 
        var links = $(dirhtml).find("a");
        $.each(links, function(i, a) {
            if (i === 0) {
                return;
            }

            var game = a.getAttribute("href"); // Don't want domain
            var path = "/chess/game?game=" + game;
            var newlink = $("<a>")
                .attr("href", path)
                .text(a.text.replace(".json", ""));

            var li = $("<li>").append(newlink);
            $("#game-list").append(li);
        });
    });
});

