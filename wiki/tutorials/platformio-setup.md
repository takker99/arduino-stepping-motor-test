---
title: PlatformIO 開発環境セットアップ
type: tutorial
tags: [tutorial, platformio, build, toolchain, setup]
created: 2026-08-10
updated: 2026-08-10
---

# PlatformIO 開発環境セットアップ

Arduino スケッチを PlatformIO で書くための環境構築メモ。
本プロジェクト ([[arduino-uno-r4-minima]] + [[28byj-48]]) での実績ベース。

## プロジェクト構成

```
├── platformio.ini   # ビルド設定
├── src/main.cpp     # スケッチ本体 (Arduino IDE の .ino 相当)
└── .pio/            # ビルド成果物 (git 管理外)
```

- Arduino IDE の `.ino` は PlatformIO では `src/main.cpp` に置く
- `.pio/` と `.vscode/` は `.gitignore` に追加済み

## platformio.ini

```ini
[env:uno_r4_minima]
platform = renesas-ra
board = uno_r4_minima
framework = arduino
monitor_speed = 9600
lib_deps =
  arduino-libraries/Stepper
platform_packages =
  platformio/toolchain-gccarmnoneeabi@~1.120301.0
```

- `board = uno_r4_minima` — [[arduino-uno-r4-minima]] (RA4M1, 256KB Flash / 32KB RAM)
- `lib_deps` — Stepper ライブラリは framework に同梱されていないため明示的に追加
- `platform_packages` — **aarch64 対応のための必須記述** (後述)

## ビルド・書き込み・モニタ

```bash
pio run                 # ビルド
pio run -t upload       # 書き込み (USB 接続時)
pio device monitor      # シリアルモニタ (9600 baud)
```

## トラブルシュート

### aarch64 (ARM64) 環境で toolchain が入らない

**症状**: `pio run` が以下のエラーで失敗

```
UnknownPackageError: Could not find the package with
'platformio/toolchain-gccarmnoneeabi @ ~1.70201.0' requirements for
your system 'linux_aarch64'
```

**原因**: renesas-ra プラットフォーム (1.9.0 時点) が要求する
toolchain-gccarmnoneeabi **~1.70201.0 は linux_aarch64 ビルドが存在しない**。

**解決**: aarch64 対応済みの ~1.120301.0 (GCC 12.3.1) を
`platform_packages` で明示的に指定して上書きする。

```ini
platform_packages =
  platformio/toolchain-gccarmnoneeabi@~1.120301.0
```

バージョン対応表:

| バージョン | aarch64 | 備考 |
| --- | --- | --- |
| ~1.70201.0 | なし | renesas-ra のデフォルト要求 |
| ~1.120301.0 | あり | 本プロジェクトで使用 (GCC 12.3.1) |

### Stepper.h が見つからない

**症状**: `src/main.cpp:2:10: fatal error: Stepper.h: No such file or directory`

**原因**: Arduino の Stepper ライブラリは PlatformIO の
framework-arduinorenesas-uno に同梱されていない。

**解決**: `lib_deps` に `arduino-libraries/Stepper` を追加する。

## ビルド結果 (2026-08-10 時点)

- RAM: 8.6% (2828 / 32768 bytes)
- Flash: 13.1% (34464 / 262144 bytes)

## 関連ページ

- [[arduino-uno-r4-minima]]
- [[stepper-library]]
- [[tutorials/stepper-library-examples]]
- [[overview]]
