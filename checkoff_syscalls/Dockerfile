from alpine:3.21 as build_intermediate

WORKDIR /app

RUN apk update && apk add gcc musl-dev

COPY ./src ./src

#RUN gcc -nostdlib -s -Os -static -o main src/main.c
RUN gcc -Os -s -nostdlib -fno-builtin -static -o main src/main.c src/syscall.h src/syscall.c
#RUN gcc -g -nostdlib -static -o main src/main.c src/syscall.h src/syscall.c

CMD ["./main"]

from scratch

WORKDIR /app
COPY --from=build_intermediate /app/main .
CMD ["./main", "8080"]
