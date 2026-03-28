# Native TS / Vite / CRA の比較メモ

ここでは「素の TypeScript」「Vite」「create-react-app（CRA）」の違いを、
開発体験・仕組み・依存関係の観点で整理する。

## 1. ざっくり結論

- **素の TypeScript**: 変換だけ（`tsc`）。開発サーバーや HMR は自分で用意。
- **Vite**: 開発サーバー + 高速 HMR + 本番ビルドを一括提供。
- **CRA**: 充実した開発体験をまとめて提供するが、起動や更新が重くなりやすい。

## 2. 依存パッケージの違い（代表例）

### 素の TypeScript
- `typescript`

### Vite（Vanilla + TS）
- `vite`
- `typescript`

### CRA（create-react-app）
- `react`
- `react-dom`
- `react-scripts`
  - Webpack（バンドル）
  - Babel（変換）
  - ESLint（lint）
  - 開発サーバー・HMR など

## 3. 役割の違い

### 素の TypeScript
- `ts` → `js` の変換のみ
- 型情報は消える（実行時型チェックはしない）
- 変換後の JS を HTML から読み込む

### Vite
- 開発サーバー（`npm run dev`）
- 高速 HMR（ホットリロード）
- ES Modules を使ったオンデマンド配信
- 本番ビルドは Rollup ベースで最適化
- TS は esbuild で高速トランスパイル（型チェックは別途）

### CRA
- `react-scripts` が全部内包
- 開発中もバンドル（Webpack）
- Babel で変換
- ESLint など開発体験込み

## 4. Vite が速い理由

- 開発中は **バンドルしない**（必要な分だけ配信）
- 更新は差分だけ反映（HMR が高速）
- 依存を事前最適化して起動を高速化

## 5. まとめ（使い分け）

- **仕組みを理解したい** → 素の TypeScript
- **すぐ動かして快適に開発したい** → Vite
- **過去資産や CRA 前提のプロジェクト** → CRA


## 6. CRA は今どういう位置づけ？（2025-02-14 以降）

- React 公式は **2025-02-14 に CRA を新規アプリ向けに非推奨** と表明。
- CRA は **メンテナンスモード（長期的な停滞）** と明記。
- 既存の CRA プロジェクトは動くが、新規なら Vite やフレームワーク推奨。

参考: https://react.dev/blog/2025/02/14/sunsetting-create-react-app

