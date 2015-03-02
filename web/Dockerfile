FROM ubuntu

RUN apt-get update && apt-get install -y \
    nginx

RUN rm /etc/nginx/sites-enabled/default
COPY chessanalyze.nginx.conf /etc/nginx/sites-enabled/

COPY www/ /var/www/chessanalyze/web/
COPY games/ /var/www/chessanalyze/games

CMD nginx -g "daemon off;"
EXPOSE 80

