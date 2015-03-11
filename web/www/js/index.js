require(["jquery"], function($) {
    $.get("games/", function(games) {

        games.sort(function(a, b) {
            var adate = new Date(a.mtime);
            var bdate = new Date(b.mtime);
            return bdate.getTime() - adate.getTime();
        });

        $.each(games, function(i, game) {
            var path = "game?game=" + game.name;
            var displayname = game.name.substr(0, game.name.indexOf(" on "));

            var date = new Date(game.mtime);
            var month = date.getMonth() + 1;
            var day = date.getDay() + 1;
            var year = date.getFullYear();
            var datestr = month + "/" + day + "/" + year;

            var newlink = $("<a>")
                .attr("href", path)
                .text(displayname + ", " + datestr);

            var li = $("<li>").append(newlink);
            $("#game-list").append(li);
        });
    });
});

