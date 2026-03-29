# SNS（SQLite + 簡易API）構築で学んだこと

このメモは、SQLite を使った簡易SNSの設計〜API化までで
実際に理解した内容を整理したもの。

## 1. DB設計の基本
- **エンティティを洗い出す**: users / posts / follows が最小構成
- **主キー（PK）**: `users.id`, `posts.id` のように一意IDを持つ
- **外部キー（FK）**: `posts.user_id` が `users.id` を参照する
- **中間テーブル**: `follows(follower_id, followee_id)` のように関係を表現

## 2. schema の意味
- `created_at` は作成時刻の保存用カラム
- ISO8601 形式なら **文字列でも並び順＝時刻順**になる
- `CREATE INDEX` は検索の高速化
  - 例: `posts.user_id`, `follows.follower_id` を索引化

## 3. JOIN の理解
- JOIN は「テーブル同士の列を対応づける」ための構文
- 今回は
  - `posts.user_id = follows.followee_id`
  - `follows.follower_id = ?`
  で「自分がフォローしている人の投稿」を絞り込む
- JOIN を使わず `IN (subquery)` でも同じ結果になる

## 4. タイムラインSQL
- フォロー中の投稿を新しい順に取得
- `ORDER BY created_at DESC` で降順
- `LIMIT` で表示件数を制限

## 5. SQLite の挙動
- `sqlite3` は Python 標準ライブラリで使える
- `conn` は DB 接続、`cursor` は SQL を実行するための作業台
- `SELECT` は commit 不要
- `INSERT/UPDATE/DELETE` は commit が必要

## 6. API の仕組み理解
- `http.server` で GET/POST を分けて処理
- クエリは URL の **リクエストライン**に入る
- ヘッダやボディではない
- クエリは `urllib.parse` で辞書にして取得

## 7. 実装済みAPI
- **タイムライン取得**: `/timeline?user_id=1&limit=20`
- **フォロー追加**: `POST /follow?follower_id=1&followee_id=2`
- **投稿追加**: `POST /post?user_id=2&body=hello`

## 8. URLクエリの注意点
- 空白は `+` / `%20` にエンコードする必要あり
- 日本語もURLエンコードが必要
- POSTでもクエリは使えるが、通常はbodyを使う

## 9. ダミーデータ投入
- `seed_sns.py` で users / posts / follows を生成
- レコード数の確認は
  `SELECT COUNT(*) FROM users;` で可能

## 10. バックアップの取り方
- SQLite はファイルDBなので **コピーでバックアップ可能**
- 併せて `integrity_check` や `count` を記録すると状態が分かる
- `.schema` や `.tables` を残しておくと構造確認が容易

## 11. 実装中に出た注意点
- `JOIN` の列対応を間違えると結果がずれる
- `INSERT OR IGNORE` と `INSERT OR REPLACE` は挙動が違う
- `created_at` は ISO8601 に揃えると扱いやすい

## 12. 次にやりたい拡張
- フォロー解除
- 投稿削除
- 自分の投稿一覧
- タイムラインに自分の投稿を混ぜる
- いいね（likes）機能追加
- ログイン/セッション