# use latest rust as base image with cargo installed
FROM rust:latest AS build

# add musl target
RUN rustup target add x86_64-unknown-linux-musl
RUN apt update && apt install -y musl-tools musl-dev

# set working directory inside container
WORKDIR /app

# Copy files from machine to docker image
COPY . /app

# Build
RUN cargo build --target x86_64-unknown-linux-musl --release

# compress with UPX
# FROM gruebel/upx:latest as compress
# COPY --from=build /app/target/x86_64-unknown-linux-musl/release/checkoff_rust /checkoff_rust_original
# RUN upx --best --lzma -o /checkoff_rust /checkoff_rust_original

# Distroless execution image
FROM scratch

# copy build results 
COPY --from=build /app/target/x86_64-unknown-linux-musl/release/checkoff_rust /

# run
CMD ["./checkoff_rust"]
