#!/usr/bin/env bash
docker run --rm \
    --platform linux/amd64 \
    -u "$(id -u):$(id -g)" \
    -v "$PWD:/work" \
    -w /work \
    compilerbook-amd64-env "$@"