# TypeScript 学習ログ（ビルドまわり）

このファイルは、TypeScript をブラウザで動かすための「最小構成」と、
Vite を使った場合の「開発体験の違い」を整理したメモ。

## 1. npm コマンドの基礎

### `npm init -y`
- `package.json` を「質問なしで」作る。
- `-y` は「全部デフォルトでOK」という意味。

### `npm i` と `npm install`
- `npm i` は `npm install` の短縮。
- 実行したディレクトリに `node_modules/` が作られ、そこに依存が入る。
- `package.json` に依存が記録される。
- `package-lock.json` が更新される。

### `-D` / `--save-dev`
- 開発時にだけ使う依存として入れる指定。
- `package.json` の `devDependencies` に入る。

#### 例
```bash
npm i react          # 実行時に必要な依存（dependencies）
npm i -D typescript  # 開発時にだけ必要（devDependencies）
```

### `-g`（グローバル）
- `npm i -g <pkg>` でグローバルに入れる。
- グローバルは「どのプロジェクトからでもコマンドとして使いたいツール向け」。
- 実体の場所は `npm config get prefix` で確認できる。

## 2. 最小構成の TypeScript（バンドラーなし）

### 目的
- TypeScript を **コンパイルして JS にするだけ**。
- 開発サーバーやバンドルは自分で用意する。

### 手順
```bash
npm init -y
npm i -D typescript
```

`tsconfig.json`
```json
{
  "compilerOptions": {
    "target": "ES2017",
    "module": "ESNext",
    "outDir": "dist",
    "rootDir": "src",
    "strict": true
  }
}
```

`src/main.ts`
```ts
const el = document.getElementById("app");
if (el) {
  el.textContent = "Hello TypeScript";
}
```

`index.html`
```html
<!doctype html>
<html>
  <head><meta charset="utf-8" /></head>
  <body>
    <div id="app"></div>
    <script type="module" src="./dist/main.js"></script>
  </body>
</html>
```

ビルド
```bash
npx tsc
```

ブラウザで開く
- `index.html` を直接開くか、ローカルサーバー推奨。
- `type="module"` は `file://` だと制限があるため、サーバー起動が安全。

## 3. Vite を使う場合

### 目的
- **開発サーバー + HMR + 最適化ビルド** を一括で提供。
- TypeScript を即使えるテンプレートあり。

### 例（Vite の基本）
```bash
npm create vite@latest
npm i
npm run dev
```

- `npm run dev` で開発サーバーが立つ
- HMR（ホットリロード）が高速
- 本番ビルドは `npm run build`

## 4. Vite と素の TypeScript の違い（まとめ）

### 素の TypeScript
- できること: `ts` → `js` の変換だけ
- 開発サーバー: なし
- HMR: なし
- バンドル/最適化: なし
- 設定: 自分で全部用意

### Vite
- 開発サーバー: あり
- HMR: あり（速い）
- バンドル/最適化: あり（本番ビルド）
- 設定: ほぼ不要

## 5. Vite と create-react-app の速度差

### CRA（create-react-app）
- 開発中でも **全部バンドル** してから配信する（Webpack）。
- プロジェクトが大きくなるほど起動・更新が重くなりやすい。

### Vite
- 開発中は **バンドルしない**。
- ES Modules を使って「必要な分だけ読み込み」。
- 更新は部分的に反映（HMR が高速）。

### 体感差の理由
- 起動: CRA は全体ビルド、Vite は必要分だけ配信
- 更新: CRA は再バンドル、Vite は部分更新

## 6. まとめ（使い分けの指針）

- 仕組みを理解したい → 素の TypeScript
- すぐ作って快適に開発したい → Vite

