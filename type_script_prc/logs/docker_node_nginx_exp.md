# Docker + Node + Nginx 実験ログ

このメモは、Node（Express）と Nginx を Docker で分けて動かし、
接続やネットワーク挙動を試した内容のまとめ。

## 1. 目的
- Node コンテナと Nginx コンテナを分けて起動
- Nginx → Node の proxy 接続を確認
- 負荷テスト時の挙動・限界を理解

## 2. 重要な前提

### コンテナ名での通信
- `proxy_pass http://ts-api:3000;` の `ts-api` は **コンテナ名**
- 同じユーザー定義ネットワークに入れる必要がある

### ホストからのアクセス
- ホスト（自分のPC）からは `ts-api` という名前は解決できない
- ホストから叩くなら `http://localhost:PORT`
- `ts-api` は **コンテナ間通信専用の名前**

## 3. ネットワークの作り方（方針）

- `docker network create app-net`
- Node と Nginx を **同じネットワーク**に入れる
- Nginx だけ外部ポートを公開する

## 4. nginx.conf の考え方

- 最小構成は `events {}` / `http {}` / `server {}`
- `proxy_pass http://ts-api:3000;` を書く
- 設定ファイルは `/etc/nginx/nginx.conf` にマウント

## 5. よくあるミス

- `nginx.conf` のマウント先を間違える
  - 正: `/etc/nginx/nginx.conf`
  - 誤: `/etc/nginx/nginx.fonf`

- `run_nginx.sh` の最後に `\` を付けてコマンドが壊れる

- Nginx コンテナで Node を起動してしまう
  - Nginx は **Nginx を起動するだけ**でよい

## 6. ポートの考え方

- **Nginx が入口**なら、外に公開するのは Nginx だけ
- Node は内部ネットワーク専用にする
- 例: `-p 3000:80` を Nginx に付ける

## 7. Nginx を入れてもエラーが消えない理由

- Nginx は接続管理が得意だが、**上限そのものは変えられない**
- `somaxconn` など OS 側の上限が残る
- Node 側が詰まると RST は発生する

## 8. keep-alive / backlog の理解

- keep-alive は速いが接続が溜まりやすい
- backlog を増やしても `somaxconn` に丸められる
- macOS の `somaxconn=128` は小さく、すぐ詰まりやすい

## 9. 学んだこと（まとめ）

- Nginx は「接続管理役」、Node は「処理役」
- **コンテナ名での通信はネットワークが前提**
- ホストからは `ts-api` を直接叩けない
- エラーを完全に消すには OS レベルの上限も重要
- Docker Desktop（mac）は c10k 再現が厳しい

