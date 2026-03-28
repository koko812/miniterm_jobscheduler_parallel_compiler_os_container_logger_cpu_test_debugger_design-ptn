#!/bin/sh
docker run \
    --rm \
    --network app-net \
    --name nx-api \
    --ulimit nofile=65535:65535 \
    -it \
    -p 3000:80 \
    --cpus 1 \
    --memory 1280m \
    -v "$(pwd)":/app \
    -v "$(pwd)/nginx/nginx.conf":/etc/nginx/nginx.conf:ro \
    -w /app \
    nginx:alpine