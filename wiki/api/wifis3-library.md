---
title: WiFiS3 ライブラリ (Arduino UNO R4 WiFi)
type: api
tags: [wifi, library, esp32, api]
sources:
  - raw/arduino-uno-r4-wifi/user-manual.md
created: 2026-08-10
updated: 2026-08-10
---

# WiFiS3 ライブラリ

[[arduino-uno-r4-wifi|UNO R4 WiFi]] で Wi-Fi 機能を使うためのライブラリ。
Arduino UNO R4 Boards パッケージに同梱されている ([[arduino-uno-r4-minima|UNO R4 Minima]]
では使えない)。
内部的には [[esp32-s3-mini-1|ESP32-S3-MINI-1-N8]] を SPI/UART 経由で操作する。

```cpp
#include <WiFiS3.h>
```

> 📌 "WiFiS3" の "S3" は ESP32-S3 由来。ESP32 用 classic `WiFi.h` とは別物なので、
> 通常の ESP32 スケッチを移植する際はインクルードを `<WiFiS3.h>` に書き換える必要あり。

## よく使う関数

### WiFi.begin() — STA 接続開始

```cpp
int status = WiFi.begin(SSID, PASS);
```

- 引数: SSID (文字列), パスワード (文字列)
- 戻り値: `WL_IDLE_STATUS` (0) 〜 `WL_CONNECT_FAILED` (4) など
- 非同期。接続完了は `WiFi.status() == WL_CONNECTED` で確認
- タイムアウト処理は自前で実装 (下記)

```cpp
unsigned long start = millis();
while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
  delay(500);
  Serial.print(".");
}
if (WiFi.status() != WL_CONNECTED) {
  // 失敗処理 (リトライ / AP モード fallback など)
}
```

### WiFi.status() — 現在の接続状態

```cpp
int status = WiFi.status();
```

戻り値:
| 定数 | 意味 |
| --- | --- |
| `WL_IDLE_STATUS` | アイドル |
| `WL_NO_SSID_AVAIL` | SSID が見つからない |
| `WL_SCAN_COMPLETED` | スキャン完了 |
| `WL_CONNECTED` | **接続成功** |
| `WL_CONNECT_FAILED` | 接続失敗 |
| `WL_CONNECTION_LOST` | 接続喪失 |
| `WL_DISCONNECTED` | 未接続 |

### WiFi.localIP() — 自分の IP アドレス取得

```cpp
IPAddress ip = WiFi.localIP();
Serial.println(ip);  // 例: "192.168.1.42"
```

### WiFi.RSSI() — 受信信号強度

```cpp
long rssi = WiFi.RSSI();  // dBm (負の値、0 に近いほど強い)
```

### WiFi.disconnect() — 切断

```cpp
WiFi.disconnect();
```

## AP モード (将来用)

```cpp
WiFi.beginAP("UNO-R4-WiFi-AP", "password123");
IPAddress ip = WiFi.localIP();  // 通常 192.168.4.1
```

STA + AP 両モード同時 (WiFiS3 は対応) も可能な場合あり。
本プロジェクトの [[tutorials/wifi-api-server|MVP]] では STA のみ。

## メモリ消費 (感覚値)

WiFiS3 ライブラリはスタック/ヒープを相応に食う。
UNO R4 WiFi (RA4M1, SRAM 32 KB) で確認すべきポイント:
- Wi-Fi 接続後、起動時より SRAM 余裕が **5–10 KB 減る**
- HTTP サーバ (`WebServer`) を立ち上げるとなお減る
- JSON パーサ (`ArduinoJson`) を入れる場合は静的バッファ + `ARDUINOJSON_USE_LONG_LONG=0` で縮小

`Serial.println(freeRam());` のようなヘルパで起動後に確認すると安全。
(フラッシュに余裕はあるのでライブラリ追加はサイズ的には OK。RAM が律速。)

## 関連ページ

- [[api/webserver-library]] — WiFiS3 の上で動く HTTP サーバ
- [[tutorials/wifi-api-server]] — MVP 実装チュートリアル
- [[esp32-s3-mini-1]] — WiFiS3 の物理層を担うコプロセッサ
- [[arduino-uno-r4-wifi]] — ボード全体
