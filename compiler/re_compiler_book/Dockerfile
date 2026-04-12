FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    llvm \
    make \
    gdb \
    file \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work