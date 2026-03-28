# TypeScript で最小の Node サーバーを作った流れ

このログは、`curl` で叩ける最小の HTTP サーバーを
TypeScript + Node 標準モジュールだけで動かすまでの過程をまとめたもの。

## 1. 目的
- Node.js の標準 `http` で最小サーバーを立てる
- TypeScript で書いて `tsc` で JS に変換する
- `curl` で JSON を返す

## 2. 最小サーバー例

```ts
import * as http from "node:http";

const server = http.createServer((req, res) => {
  if (req.url === "/hello") {
    res.writeHead(200, { "Content-Type": "application/json" });
    res.end(JSON.stringify({ message: "hello" }));
    return;
  }

  res.writeHead(404, { "Content-Type": "application/json" });
  res.end(JSON.stringify({ error: "not found" }));
});

server.listen(3000, () => {
  console.log("listening on http://localhost:3000");
});
```

## 3. エラー対応の流れ

### エラー1
```
Cannot find module 'node:http' or its corresponding type declarations.ts(2307)
```

**原因**
- TypeScript が Node の型定義を知らない。

**対応**
```bash
npm i -D @types/node
```

---

### エラー2
```
Module '"node:http"' has no default export.
```

**原因**
- `node:http` には `default export` がないため、
  `import http from "node:http"` は使えない。

**対応（解決した書き方）**
```ts
import * as http from "node:http";
```

## 4. まとめ

- Node 標準モジュールを TS で使うときは `@types/node` が必須。
- `node:http` は default export を持たないので、
  `import * as http from "node:http";` が安全。

## 5. write / end / ヘッダーの関係

### `write` と `end` の違い
- `write` は本文を「追加で送る」だけ。
- `end` は本文の最後を送り、**レスポンスを完了する**。
- `write` だけだと接続が閉じないので、最後に必ず `end` が必要。

#### 例（分割送信）
```ts
res.writeHead(200, { "Content-Type": "text/plain" });
res.write("hello ");
res.write("world");
res.end();
```

#### 例（1回で完了）
```ts
res.writeHead(200, { "Content-Type": "text/plain" });
res.end("hello world");
```

### ヘッダーは `write` では送れない
- ヘッダーは **レスポンスの最初** にまとめて送る必要がある。
- そのための API が `writeHead`（または `setHeader` + `statusCode`）。
- いったん本文を書き始めると、ヘッダーは変更できない。

### ヘッダーは必須か？
- **必須ではない**。送らなくてもレスポンスは動く。
- ただし、JSON を返すなら `Content-Type: application/json` を付けるのが安全。


## 6. ERR_HTTP_HEADERS_SENT が出た件

### 症状
```
Error [ERR_HTTP_HEADERS_SENT]: Cannot write headers after they are sent to the client
```

### 典型的な原因
- `/hello` の処理が終わった後に 404 の処理まで流れて
  `writeHead` が二重に呼ばれる。

### 今回の原因
- `tsc` を忘れていて `dist/server.js` が古いままだった。
- 修正済みの `server.ts` が反映されておらず、
  古いロジックで二重送信が起きていた。

### 対策
- `npx tsc && node dist/server.js` をセットで実行する。
- もしくは `tsx watch` でビルド無し実行にする。

