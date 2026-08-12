---
title: PlatformIO 開発環境セットアップ
type: tutorial
tags: [tutorial, platformio, build, toolchain, setup]
created: 2026-08-10
updated: 2026-08-12
---

# PlatformIO 開発環境セットアップ

Arduino スケッチを PlatformIO で書くための環境構築メモ。
本プロジェクト ([[arduino-uno-r4-wifi]] + [[28byj-48]] + [[mb102]]) での実績ベース。
([[arduino-uno-r4-minima]] から 2026-08-10 に方針変更)

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
[env:uno_r4_wifi]
platform = renesas-ra
board = uno_r4_wifi
framework = arduino
monitor_speed = 9600
lib_deps =
  arduino-libraries/Stepper
platform_packages =
  platformio/toolchain-gccarmnoneeabi@~1.120301.0
```

- `board = uno_r4_wifi` — [[arduino-uno-r4-wifi]] (RA4M1 + ESP32-S3, 256KB Flash / 32KB RAM)
- `lib_deps` — Stepper ライブラリは framework に同梱されていないため明示的に追加。
  WiFi 機能を使う分には追加 lib 不要 ([[api/wifis3-library|WiFiS3]] /
  [[api/webserver-library|WebServer]] はボードパッケージに同梱)
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

### WSL2 で USB デバイスが見えない (UNO R4 WiFi)

**症状**: `pio device list` にデバイスが現れない。`ls /dev/ttyACM*` も空。
(Windows 側では COM ポートとして認識されている)

**原因**: WSL2 はデフォルトで USB をゲストに通さない。Windows 側の
**usbipd-win** で明示的に attach する必要がある (WSL1 は非対応、WSL2 専用)。

**解決**:

1. 管理者 PowerShell で usbipd をインストール (一度だけ):

   ```powershell
   winget install usbipd
   ```

2. デバイスを確認 (管理者 PowerShell):

   ```powershell
   usbipd list
   # BUSID  VID:PID    DEVICE                            STATE
   # 2-1    2341:1002  USB シリアル デバイス (COM5)      Detached / Shared
   ```

   VID 2341 = Arduino。実機では UNO R4 WiFi が `2341:1002 (COM5)` だった。

3. bind (一度だけ。以後 persisted):

   ```powershell
   usbipd bind -b 2-1
   ```

4. WSL に attach (セッションごと。常時化は末尾に `--auto-attach`):

   ```powershell
   usbipd attach --wsl -b 2-1                  # 手動
   usbipd attach --wsl -b 2-1 --auto-attach    # 以後自動
   ```

5. WSL 側で確認:

   ```bash
   ls -la /dev/ttyACM*    # UNO R4 WiFi は ttyACM0 (CDC) になる
   pio device list
   ```

**確認結果 (2026-08-12 実機)**:

```
/dev/ttyACM0
------------
Hardware ID: USB VID:PID=2341:1002 SER=64E83361C5C0 LOCATION=1-1:1.1
Description: UNO WiFi R4 CMSIS-DAP - TinyUSB CDC
```

UNO R4 WiFi は CMSIS-DAP デバッガ + シリアル (CDC) を兼ねた 1 ポートで、
`pio run -t upload` も `pio device monitor` もこの 1 ポートで動く。

**パーミッション**: WSL2 では `/dev/ttyACM0` が `crw-rw-rw-` (666, root:dialout)
で作成され、追加設定なしでアクセス可能だった。Permission denied が出る場合は
`sudo usermod -aG dialout $USER` → 再ログイン、または `sudo chmod 666 /dev/ttyACM0`。

### Stepper.h が見つからない

**症状**: `src/main.cpp:2:10: fatal error: Stepper.h: No such file or directory`

**原因**: Arduino の Stepper ライブラリは PlatformIO の
framework-arduinorenesas-uno に同梱されていない。

**解決**: `lib_deps` に `arduino-libraries/Stepper` を追加する。

### WiFiS3 で `byte` 曖昧性エラー (GCC 12.3.1)

**症状**: `pio run` 時に以下:

```
error: reference to 'byte' is ambiguous
note: 'enum class std::byte' (from <cmath> via ArduinoAPI.h)
note: 'typedef uint8_t byte' (from Common.h)
```

**原因**: `framework-arduinorenesas-uno/libraries/WiFiS3/src/WiFiSSLClient.{h,cpp}`
が `const byte cert[]` のような生 Arduino `byte` を使っており、
GCC 12.3.1 の `<cmath>` が `std::byte` を持っていて衝突する既知バグ。
(SSL 機能を使っていなくても WiFiS3 ライブラリ全体がコンパイル対象になるため影響する。)

**解決 (暫定パッチ)**: `byte` を `uint8_t` に置換する。1 度だけでいい:

```bash
F=$(ls -d ~/.platformio/packages/framework-arduinorenesas-uno* | head -1)/libraries/WiFiS3/src/WiFiSSLClient
sed -i 's/const byte cert\[\]/const uint8_t cert[]/g; s/const byte\* _ecc_cert/const uint8_t* _ecc_cert/g' \
  "$F.h" "$F.cpp"
```

> 📌 PlatformIO のフレームワーク再インストール時にパッチが消えるため、
> CI や別マシンで使う場合は `scripts/patch-wifis3.sh` 等に切り出して
> `platformio.ini` の `extra_scripts = pre:scripts/patch-wifis3.sh` で
> 自動適用する運用が望ましい。
> Arduino 公式 upstream の修正を待つのも一案。

### `WebServer.h` が見つからない

**症状**: `#include <WebServer.h>` で not found

**原因**: Arduino UNO R4 Boards パッケージには `WebServer.h` が
**含まれていない**。`arduino-libraries/WebServer` (ライブラリマネージャ) は
AVR 系用で UNO R4 WiFi では動かない。

**解決**: [[api/webserver-library|UNO R4 WiFi では WiFiServer + WiFiClient を直接使う]]
(公式 `SimpleWebServerWiFi.ino` と同じアプローチ)。スケッチ実装は
[[tutorials/wifi-api-server|MVP]] 参照。

## ビルド結果 (2026-08-10 時点)

- 旧 Minima: RAM 8.6% (2828 / 32768 bytes), Flash 13.1% (34464 / 262144 bytes)
- **WiFi (MVP スケッチ実測)**: RAM **16.0%** (5252 / 32768 bytes),
  Flash **22.1%** (57812 / 262144 bytes) — byte パッチ + WiFiS3 + Stepper + WiFiServer 込み
- 余裕: RAM 84%, Flash 78% — WebServer 風の高機能化や ArduinoJson 追加でもまだ余裕あり

## 関連ページ

- [[arduino-uno-r4-wifi]]
- [[arduino-uno-r4-minima]] — 旧コントローラ (本プロジェクトでは不採用)
- [[api/stepper-library]]
- [[tutorials/stepper-library-examples]]
- [[tutorials/wifi-api-server]] — WiFi + API server スケッチ
- [[overview]]
