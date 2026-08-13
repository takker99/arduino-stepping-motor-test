---
title: Wi-Fi API Server 実装 (MVP)
type: tutorial
tags: [wifi, api, http, rest, mvp, esp32, stepper]
sources:
  - raw/arduino-uno-r4-wifi/datasheet.md
  - raw/arduino-uno-r4-wifi/user-manual.md
created: 2026-08-10
updated: 2026-08-12
---

# Wi-Fi API Server 実装 (MVP)

[[arduino-uno-r4-wifi|UNO R4 WiFi]] を HTTP API サーバとして立ち上げ、
同一 LAN 内のスマホ / PC から [[28byj-48]] ステッピングモーターを無線制御する最小実装。
2026-08-10 決定の **STA モード + シンプル API** に基づく。

> 📌 このページは MVP。将来 [[tutorials/wifi-api-design-notes|設計ノート]] に沿って
> 高機能化する可能性がある (認証、AP フォールバック、ジョブキューなど)。

## API エンドポイント

| メソッド | パス | 用途 | ボディ / クエリ |
| --- | --- | --- | --- |
| `GET` | `/` | **OpenAPI ドキュメント (JSON)** (2026-08-13 追加) | — |
| `GET` | `/index.html` | ヘルプ (HTML) | — |
| `GET` | `/status` | 現在状態 (JSON) | — |
| `POST` | `/step` | ステップ実行 | クエリ `?steps=N&dir=cw\|ccw&speed=RPM` または body `N cw` |
| `POST` | `/stop` | 通電遮断 (全ピン LOW, ホールド解除) | — |

> `GET /` を叩くと **OpenAPI 3.0.3 の JSON** が返る。エンドポイント一覧・
> パラメータ・レスポンスのスキーマ (components/schemas) が全部載っているので、
> API を忘れたらまずこれを見る。`servers[0].url` は現在の IP が自動で入る。
> Swagger UI / Redoc / Postman に貼り付けて見ることもできる。

### レスポンス例

```http
GET / HTTP/1.1
```

```json
{
  "openapi": "3.0.3",
  "info": { "title": "UNO R4 WiFi Stepper API", "version": "0.3.0" },
  "servers": [{ "url": "http://192.168.11.3" }],
  "paths": { "/": {}, "/status": {}, "/step": {}, "/stop": {} },
  "components": { "schemas": { "Status": {}, "StepResult": {} } }
}
```

```http
GET /status HTTP/1.1
```

```json
{
  "state": "idle",         // "idle" | "running" | "error"
  "position": 0,           // 累積ステップ数 (任意)
  "speed": 5,              // 現在の速度 rpm (2026-08-13 追加)
  "ssid": "MyHomeWiFi",
  "ip": "192.168.1.42",
  "rssi": -55
}
```

```http
POST /step?steps=2048&dir=cw&speed=2 HTTP/1.1
```

```json
{ "ok": true, "requested": 2048, "direction": "cw", "speed": 2 }
```

> **`speed` パラメータ (2026-08-13 追加)**: 回転速度を rpm で指定 (1–60、デフォルト 5)。
> 省略時は現在の速度を維持。指定すると以降の `/step` も同じ速度を使う。
> 28BYJ-48 の起動周波数 (pull-in >600 Hz) を考慮し、起動確認は **speed=1 か 2** から。
> `STEPS_PER_REV=2048` なので rpm 表示は実速度と一致 (2026-08-13 実機確認)。
> 詳細は [[log#2026-08-13]] のエントリ参照。

## 必要なもの

- [[arduino-uno-r4-wifi|UNO R4 WiFi]] (RA4M1 + ESP32-S3)
- [[uln2003|ULN2003]] driver board + [[28byj-48]] (D7–D4)
- [[mb102|MB102]] + 9V 1.3A アダプタ (モーター 5V 専用)
- USB-C ケーブル (Arduino 給電 + 書き込み)
- Wi-Fi 2.4 GHz ネットワーク (SSID + パスワード)

## 配線

[[arduino-uno-r4-wifi#ピン配置 digital/analog headers|D7–D4 → ULN2003 IN1–IN4]]
(実機配線, 2026-08-12 確定。コードは src/main.cpp 参照)。電源トポロジは
[[arduino-uno-r4-wifi#電源トポロジ]] 参照 (Arduino は USB 給電、
モーターは MB102 から)。

## スケッチ (MVP)

完成版スケッチは **`src/main.cpp`** (2026-08-10 決定)。このページには完全なコピーを
置かない (二重管理で内容がドリフトするため、2026-08-13 整理)。骨格は以下:

```cpp
#include <Arduino.h>
#include <Stepper.h>
#include <WiFiS3.h>
#include "secrets.h"   // WIFI_SSID / WIFI_PASS (git 管理外, 本番はダミー値)

// 28BYJ-48: 2048 step/rev (4 相 2 相励磁)。第 2・3 引数は相順のため入れ替え必須
const int STEPS_PER_REV = 2048;
const int PIN_IN1 = 7;   // Blue   → D7
const int PIN_IN2 = 5;   // Pink   → D5
const int PIN_IN3 = 6;   // Yellow → D6
const int PIN_IN4 = 4;   // Orange → D4
Stepper myStepper(STEPS_PER_REV, PIN_IN1, PIN_IN3, PIN_IN2, PIN_IN4);
```

> 📌 **実装上の注意**: UNO R4 WiFi のボードパッケージには
> Arduino 標準の `WebServer.h` が含まれていない。
> `WiFiS3` 同梱の **WiFiServer + WiFiClient** を直接使い、
> HTTP リクエストを手動パースする (公式 `SimpleWebServerWiFi.ino` と同じアプローチ)。
> 詳細は [[api/webserver-library]]。
>
> 完全な実装は `src/main.cpp` 参照:
> - `sendResponse()` / `urlDecode()` / `handleClient()` のディスパッチ骨格
> - `GET /` の OpenAPI ドキュメント (`kOpenApiHead` / `kOpenApiTail` / `openApiDoc()`)
> - `handleStep()` の `speed` パラメータ処理 (1–60 rpm, 省略時は現在値維持)
> - `POST /stop` の全ピン LOW による通電遮断 (RUNNING 中は 409)

## ビルド & 書き込み

`platformio.ini` (詳細は [[tutorials/platformio-setup]]):

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

```bash
pio run -t upload
pio device monitor    # シリアルで IP アドレスを確認
```

## 動作確認

シリアルモニタに IP が出たら、同じ LAN 内の端末から:

```bash
# API 仕様 (OpenAPI JSON)
curl http://192.168.1.42/

# ヘルスチェック (HTML)
curl http://192.168.1.42/index.html

# ステータス
curl http://192.168.1.42/status

# 半回転 (2048 step) 時計回り
curl -X POST "http://192.168.1.42/step?steps=2048&dir=cw"

# 低速での起動確認 (推奨: speed=1 か 2)
curl -X POST "http://192.168.1.42/step?steps=2048&dir=cw&speed=1"

# text/plain body でも可
curl -X POST --data-binary "2048 cw" -H "Content-Type: text/plain" \
  http://192.168.1.42/step

# 逆回転
curl -X POST --data-binary "-2048" http://192.168.1.42/step
```

ブラウザから `http://<IP>/index.html` を開くと HTML ヘルプが見える
(`GET /` は OpenAPI JSON を返す)。

## 既知の制約 (MVP)

- **STA 専用**: Wi-Fi 接続失敗時サーバは立ち上がるが到達不能
- **`/stop` は通電遮断のみ (2026-08-13 実装)**: ホールド中の 2 相を全ピン LOW で
  遮断し、保持トルクを解除する。`step()` が同期的にブロックするため
  **実行中の動作は中断できない** (RUNNING 中は 409 を返す。要ノンブロッキング化)
- **認証なし**: 同一 LAN 内の誰でも操作可能 (本プロジェクトは自宅 LAN 想定)
- **JSON パースなし**: クエリ or text/plain のみ。JSON body は [[api/webserver-library]]
  の `ArduinoJson` 例を参照
- **LED Matrix 未使用**: 状態表示の余地あり

これらは [[tutorials/wifi-api-design-notes|設計ノート]] で段階的に解消する想定。

## 関連ページ

- [[api/wifis3-library]] — Wi-Fi 接続
- [[api/webserver-library]] — HTTP サーバ (WiFiServer + 手動パース)
- [[api/stepper-library]] — モータ制御
- [[tutorials/wifi-api-design-notes]] — 将来拡張の設計メモ
- [[tutorials/platformio-setup]] — ビルド環境 + WiFiS3 パッチ手順
- [[arduino-uno-r4-wifi#電源トポロジ]] — 配線
- [[mb102]] — 電源モジュール
