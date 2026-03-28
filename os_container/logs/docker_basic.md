# Docker 基本メモ

## よく使うオプション
- `-d`: バックグラウンド実行
- `-it`: 対話TTY（`-i` と `-t`）
- `--rm`: 停止時に自動削除
- `--name <name>`: コンテナ名を付与
- `-p <host>:<container>`: ポート公開
- `-v <host>:<container>`: ボリューム/マウント
- `-e KEY=VALUE`: 環境変数
- `--network <net>`: ネットワーク指定
- `--entrypoint <cmd>`: 起動コマンド差し替え
- `--cpus <n>`: CPU 制限
- `--memory <size>`: メモリ制限

## 基本コマンド
- `docker pull <image>`: イメージ取得
- `docker images`: イメージ一覧
- `docker run ... <image>`: 起動（未取得なら自動取得）
- `docker ps`: 起動中のコンテナ一覧
- `docker ps -a`: 全コンテナ一覧
- `docker exec -it <container> <cmd>`: 既存コンテナに入る
- `docker logs <container>`: ログ表示
- `docker stop <container>`: 停止
- `docker rm <container>`: 削除
- `docker rmi <image>`: イメージ削除
- `docker network ls`: ネットワーク一覧
- `docker volume ls`: ボリューム一覧

## イメージとコンテナの違い
- イメージ: 読み取り専用のテンプレート（設計図）。起動の元になる。\n+- コンテナ: イメージから作られた実行中（または停止中）の実体。書き込み可能なレイヤを持つ。

## よく使われるイメージ
- `ubuntu`, `debian`, `alpine`, `busybox`: ベースOS
- `nginx`, `httpd`: Web サーバー
- `redis`, `memcached`: キャッシュ
- `mysql`, `postgres`, `mariadb`: DB
- `mongo`: NoSQL
- `node`, `python`, `golang`, `openjdk`: 開発/実行環境
- `rabbitmq`: メッセージキュー
- `minio/minio`: S3 互換ストレージ