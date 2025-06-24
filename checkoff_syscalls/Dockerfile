FROM alpine:3.21 AS build_intermediate

WORKDIR /app

RUN apk update && apk add git clang lld musl-dev linux-headers

RUN GIT_SSL_NO_VERIFY=true git clone https://github.com/wtarreau/nolibc.git

#RUN wget --no-check-certificate https://github.com/upx/upx/releases/download/v4.2.4/upx-4.2.4-amd64_linux.tar.xz
#RUN tar -xf upx-4.2.4-amd64_linux.tar.xz

COPY ./src ./src

RUN clang -O1 -flto -Wl,--strip-all -fuse-ld=lld -fno-stack-protector \
    -ffunction-sections -fdata-sections -Wl,--gc-sections \
    -nostdlib -static -fno-ident -fno-builtin \
    -o main src/main.c src/syscall.c src/mystdlib.c src/myhttp.c src/my_http_request.c

#RUN ./upx-4.2.4-amd64_linux/upx -9 -o main_small main && mv main_small main
RUN chmod +x main

# Detect architecture and get syscall numbers. Uses `linux-headers` apk package
RUN ARCH=$(uname -m) && \
    echo "Detected arch: $ARCH" && \
    if [ "$ARCH" = "x86_64" ]; then \
        cp /usr/include/asm/unistd_64.h ./unistd.h; \
    elif [ "$ARCH" = "i386" ] || [ "$ARCH" = "i486" ] || [ "$ARCH" = "i586" ] || [ "$ARCH" = "i686" ]; then \
        cp /usr/include/asm/unistd_32.h ./unistd.h; \
    elif [ "$ARCH" = "aarch64" ]; then \
        cp /usr/include/asm/unistd.h ./unistd.h; \
    elif [ "$ARCH" = "armv7l" ]; then \
        cp /usr/include/asm/unistd.h ./unistd.h; \
    elif [ "$ARCH" = "ppc64le" ] || [ "$ARCH" = "ppc64" ]; then \
        cp /usr/include/asm/unistd.h ./unistd.h; \
    elif [ "$ARCH" = "s390x" ]; then \
        cp /usr/include/asm/unistd.h ./unistd.h; \
    elif [ "$ARCH" = "mips" ] || [ "$ARCH" = "mips64" ]; then \
        cp /usr/include/asm/unistd.h ./unistd.h; \
    elif [ "$ARCH" = "riscv64" ]; then \
        cp /usr/include/asm/unistd.h ./unistd.h; \
    else \
        echo "Unsupported arch: $ARCH" \
        exit 1; \
    fi && \
    grep '^#define __NR_' ./unistd.h | grep -E '__NR_(read|write|socket|setsockopt|bind|listen|accept|close|exit) ' | \
    awk 'BEGIN { print "#ifndef UNISTD_H\n#define UNISTD_H" } { print } END { print "#endif /* UNISTD_H */" }' > trimmed_unistd.h

CMD ["./main"]

FROM scratch

WORKDIR /app
COPY --from=build_intermediate /app/trimmed_unistd.h /usr/include/asm/unistd.h
COPY --from=build_intermediate /app/main .
CMD ["./main", "8080"]
