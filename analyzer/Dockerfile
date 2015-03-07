FROM ubuntu

RUN apt-get update && apt-get -y --no-install-recommends install \
    make \
    g++ \
    python

COPY external/ /usr/local/src/

WORKDIR /usr/local/src/stockfish/src
RUN make clean build install ARCH=x86-64

WORKDIR /usr/local/src/pgn-extract
RUN make clean pgn-extract
RUN mv pgn-extract /usr/local/bin/

WORKDIR /
COPY *.py /analyzerd/
COPY *.sh /analyzerd/

EXPOSE 80

CMD python -u /analyzerd/chessanalyzerd.py

