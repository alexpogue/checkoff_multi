from alpine:3.21 as build_intermediate

WORKDIR /app

RUN apk update && apk add gcc musl-dev

#RUN wget --no-check-certificate https://github.com/upx/upx/releases/download/v4.2.4/upx-4.2.4-amd64_linux.tar.xz
#RUN tar -xf upx-4.2.4-amd64_linux.tar.xz

COPY ./src ./src

RUN gcc -Os -s -nostdlib -fno-builtin -static -o main src/main.c src/syscall.c src/mystdlib.c src/myhttp.c
#RUN ./upx-4.2.4-amd64_linux/upx -9 -o main main_base

CMD ["./main"]

from scratch

WORKDIR /app
COPY --from=build_intermediate /app/main .
CMD ["./main", "8080"]
