# Docker ネットワーク/負荷テスト 実験ログ

このメモは、Node/Express を Docker で動かして
負荷テスト・リソース制限・接続管理を試した内容をまとめたもの。

## 1. 実験の目的
- Docker コンテナで動かした Node/Express サーバーの挙動を観察
- 同時接続数・リソース制限の影響を体感
- `connection reset by peer` の原因を掘る

## 2. 使った構成

### Dockerfile（本番寄り）
```Dockerfile
FROM node:22-alpine
WORKDIR /app
COPY package*.json ./
RUN npm ci
COPY . .
RUN npx tsc
CMD ["node", "dist/server_express.js"]
```

### 開発用 run.sh（ホットリロード）
```sh
#!/bin/sh
docker run \
  --rm \
  --name ts-api \
  --ulimit nofile=65535:65535 \
  -it \
  -p 3000:3000 \
  --cpus 1 \
  --memory 128m \
  -v "$(pwd)":/app \
  -w /app \
  node:22-alpine \
  sh -lc "npm ci && npx tsx watch src/server_express.ts"
```

## 3. 負荷テスト

### ツール
- `hey`

### 例
```bash
hey -n 2000 -c 50 http://localhost:3000/hello
```

### 観察
- 低めの `-c` なら安定
- `-c` を上げると `connection reset by peer` が発生

## 4. `connection reset by peer` の原因切り分け

### 起きていた症状
- 同時接続数を上げると `read: connection reset by peer`
- CPU/メモリを変えても発生タイミングが大きく変わらない

### やったこと
- `ulimit nofile` を上げる
- ログ出力を OFF にする
- `-disable-keepalive` を試す

### 結果
- `-disable-keepalive` でエラーが消えたが **大幅に遅くなった**
- keep-alive の接続維持が詰まり要因の一つだと判断

## 5. keep-alive の理解

### 何が起きていたか
- keep-alive を使うと接続が長く維持される
- 同時ソケット数が増えると受付が詰まりやすい
- その結果、RST（connection reset）が発生

### 対策候補
- 同時接続数を下げる
- backlog を増やす
- `keepAliveTimeout` を短くする
- Node を複数プロセス化する
- Nginx を挟んで接続管理を任せる

## 6. `somaxconn` の影響

- `sysctl kern.ipc.somaxconn` の値は **128**
- 128 付近が接続待ち行列の実効上限
- `-c 100` までは安定、`-c 150` で詰まりやすい

## 7. Nginx の位置づけ

- Node でも接続管理はできるが得意分野ではない
- Nginx は大量接続の管理に強い
- ただし **`somaxconn` の上限自体は変わらない**
- エラーが減る可能性はあるが、上限を増やすには OS 側の設定が必要

## 8. Docker リソースの学び

### リソース制限
```bash
docker run --rm -p 3000:3000 --cpus 0.5 --memory 256m ts-api
```

### 体感
- `--cpus 0.1` は起動が極端に遅くなる
- `--cpus 0.5` でも桁が変わるほど性能が落ちる

### 仕組み
- Docker は cgroups で CPU 時間を制限
- `--cpus 0.5` は「100ms 中 50ms だけ使える」イメージ

## 9. Docker の現在の上限確認

```bash
docker info | grep -E "CPUs|Total Memory"
```

例:
```
CPUs: 8
Total Memory: 7.654GiB
```

## 10. `docker inspect` での確認

通信・CPU・メモリをまとめて見る例:
```bash
docker inspect ts-api --format '
Name: {{.Name}}
NetworkMode: {{.HostConfig.NetworkMode}}
PortBindings: {{json .HostConfig.PortBindings}}
Ports: {{json .NetworkSettings.Ports}}
IP: {{.NetworkSettings.IPAddress}}

CPU (NanoCpus): {{.HostConfig.NanoCpus}}
CPU (CpuShares): {{.HostConfig.CpuShares}}
CPU (CpuQuota): {{.HostConfig.CpuQuota}}
CPU (CpuPeriod): {{.HostConfig.CpuPeriod}}

Memory: {{.HostConfig.Memory}}
MemorySwap: {{.HostConfig.MemorySwap}}
'
```

## 11. Dockerfile / run の学び

- `CMD` は **コンテナ起動時**に実行される
- `docker build` では `CMD` は実行されない
- `--rm` で起動したコンテナは **停止時に自動削除**
- `-p 3000:3000` を付けないと外から叩けない

## 12. `tsx` とマウント

- `tsx` は TS を直接実行するランナー
- ボリュームマウントすると `node_modules` が隠れる
- 対策:
  - `npm i -D tsx` を入れてローカル依存化
  - `-v node_modules` を別ボリュームにする

## 13. まとめ（気づき）

- メモリ制限だけでは接続数の限界は動かないことが多い
- keep-alive は速いが、接続数の上限で詰まりやすい
- `somaxconn` が小さいと同時接続の上限が早く来る
- Nginx は接続管理に強いが上限そのものは変えない
- CPU 制限は体感に直結し、`0.1` はほぼ止まる

