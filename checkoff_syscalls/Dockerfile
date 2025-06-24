FROM alpine:3.21 AS build_intermediate

WORKDIR /app

RUN apk update && apk add clang lld musl-dev

#RUN wget --no-check-certificate https://github.com/upx/upx/releases/download/v4.2.4/upx-4.2.4-amd64_linux.tar.xz
#RUN tar -xf upx-4.2.4-amd64_linux.tar.xz

COPY ./src ./src

RUN clang -O1 -flto -Wl,--strip-all -fuse-ld=lld -fno-stack-protector \
    -ffunction-sections -fdata-sections -Wl,--gc-sections \
    -nostdlib -static -fno-ident -fno-builtin \
    -o main src/main.c src/syscall.c src/mystdlib.c src/myhttp.c src/my_http_request.c #src/tinyprintf.c


#RUN ./upx-4.2.4-amd64_linux/upx -9 -o main_small main && mv main_small main
RUN chmod +x main

CMD ["./main"]

FROM scratch

WORKDIR /app
COPY --from=build_intermediate /app/main .
CMD ["./main", "8080"]
