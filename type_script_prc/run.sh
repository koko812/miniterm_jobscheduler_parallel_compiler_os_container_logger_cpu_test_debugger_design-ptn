#!/bin/sh
docker run \
    --rm \
    --network app-net \
    --name ts-api \
    --ulimit nofile=65535:65535 \
    -it \
    --cpus 0.5 \
    --memory 128m \
    -v "$(pwd)":/app \
    -w /app \
    node:22-alpine \
    sh -lc "npm ci && npx tsx watch src/server_express.ts"

#  -e LOG_REQUESTS=1 \
#  -p 3000:3000 \