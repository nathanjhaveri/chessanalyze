FROM nginx

COPY chessanalyze.nginx.conf /etc/nginx/conf.d/default.conf

COPY www/ /var/www/chessanalyze/web/
COPY games/ /var/www/chessanalyze/games

CMD nginx -g "daemon off;"
EXPOSE 80

