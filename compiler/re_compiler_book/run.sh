#!/usr/bin/env bash
docker run --rm \
    --platform linux/amd64 \
    --cap-add=SYS_PTRACE \
    --security-opt seccomp=unconfined \
    -u "$(id -u):$(id -g)" \
    -v "$PWD:/work" \
    -w /work \
    compilerbook-amd64-env "$@"