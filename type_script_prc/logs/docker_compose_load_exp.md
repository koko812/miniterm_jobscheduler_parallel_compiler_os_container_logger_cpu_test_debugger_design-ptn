# Docker Compose 負荷実験メモ（更新版）

## 目的
- docker compose で Node + Nginx を同時起動
- 負荷テストでエラー・遅延の原因と対策を把握

## 構成
- Node: `ts-api`
- Nginx: `nx-api`
- compose で同時起動

## 調整した設定

### 1. `ulimit` を引き上げ
- `ulimit -n` を上げて **Too many open files** を回避
- 同時接続数を増やしても落ちにくくなった

### 2. `UV_THREADPOOL_SIZE` を調整
- Node の libuv スレッドプール数を拡大（例: 32）
- `pbkdf2` など重い処理の並列数が増えて安定
- CPU 使用率は上がる（多コア使用）

### 3. 同期 vs 非同期
- `pbkdf2Sync` は詰まりやすくエラー多発
- `pbkdf2`（async + promisify）に変えると安定

### 4. Nginx の `worker_connections`
- `worker_connections` を増やすことで接続ドロップが減少
- `not_enough_worker` の発生が抑えられた
- Nginx の入口側の詰まりが改善

## リトライ有無での差

### リトライなし
```
python3 test_load.py http://localhost:3000/a_heavy_hash 10000 5000 0 0
```
結果:
- `errors` 多発（約 467）
- p50/p95 は改善されるが失敗が多い

### リトライあり（3回、50ms）
```
python3 test_load.py http://localhost:3000/a_heavy_hash 10000 5000 3 50
```
結果:
- `errors: 0`
- `retries: 243`
- p99 が伸びる（遅延のトレードオフ）

## 観察結果（要点）
- `hello` 系は高い並列でも安定
- `heavy_hash` 系は並列数を上げると詰まりやすい
- async 版ハッシュに切り替えるとエラーが激減
- CPU 使用率はピークで **150〜250%（1.5〜2.5コア相当）**
- Nginx は 100% 前後で動作
- メモリは `ts-api` が 150MiB 前後、`nx-api` は数MiB

## 学び
- 同時接続数を増やすには **ulimit と worker_connections が重要**
- 重い処理は **同期 → 非同期**で安定性が大きく改善
- `UV_THREADPOOL_SIZE` は効くが CPU を食う
- リトライは **成功率を上げるがレイテンシを悪化**させる

