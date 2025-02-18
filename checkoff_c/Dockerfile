from alpine:3.21 as build_intermediate

WORKDIR /app

RUN apk update && apk add gcc make ca-certificates wget
RUN apk add git
RUN apk add musl-dev

RUN update-ca-certificates \
  && wget --version \
  && echo "check_certificate = off" >> /etc/wgetrc \
  && echo "ca_certificate = /etc/ssl/certs/ca-certificates.crt" >> /etc/wgetrc \
  && git config --global http.sslVerify false

RUN wget --no-check-certificate https://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-1.0.1.tar.gz \
  && tar zxvf libmicrohttpd-1.0.1.tar.gz \
  && cd libmicrohttpd-1.0.1 \
  && ./configure --disable-httpupgrade --disable-messages --disable-epoll \
  && make \
  && make install \
  && make clean

RUN wget --no-check-certificate https://github.com/zserge/jsmn/archive/refs/tags/v1.1.0.tar.gz \
  && tar zxvf v1.1.0.tar.gz \
  && cp jsmn-1.1.0/jsmn.h .

COPY ./src ./src
RUN cp jsmn.h src/

RUN gcc -s -Os -static -o main src/main.c src/json.c src/server.c -lmicrohttpd

CMD ["./main"]

from scratch

LABEL libmicrohttpd_source="https://www.gnu.org/software/libmicrohttpd/"
LABEL libmicrohttpd_license="LGPL"

WORKDIR /app
COPY --from=build_intermediate /app/main .
CMD ["./main", "8080"]
