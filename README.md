# ペイント
画像を表示できるアプリケーションです。

## Makefile ファイル
Makefile を使用してアプリケーションを生成します。

### make build
ソースをコンパイルして実行可能ファイルを生成します。

### make clean
生成した一時ファイルを削除します。

### make debug
冗長化オプションを指定してメイクします。

### make install
Desktop Entry や GSettings を設置します。

### make native
最適化オプションとアーキテクチャを指定してメイクします。

### make release
最適化オプションを指定してメイクします。

### make uninstall
Desktop Entry や GSettings を破棄します。

## Locale ディレクトリ
メッセージ カタログを生成します。

## Paint ライブラリ
静的ライブラリを生成します。

### paint/header.h
コンパイル済みヘッダーを生成します。

### paint/paint.c
ライブラリ関数を実装します。

### paint/paint.h
ライブラリ関数を公開します。

## Viewer アプリケーション
画像を表示するアプリケーションを生成します。

### viewer/app.c
Viewer Application クラスを実装します。
アプリケーションが起動した際は Viewer Window を表示します。

### viewer/canvas.c
Viewer Canvas クラスを実装します。
ユーザーが開いた画像を保持します。
ウィジェット上に画像を表示します。

### viewer/header.h
コンパイル済みヘッダーを生成します。

### viewer/menus.ui
アプリケーションのメニューを定義します。

### viewer/viewer.h
Viewer アプリケーションの機能を公開します。

### viewer/window.c
Viewer Window クラスを実装します。
ユーザーが開いたファイルを保持します。
メニュー バーを表示します。
画像を表示するウィジェットを所有します。

-----------------------------------
Copyright (C) 2026 Taichi Murakami.
