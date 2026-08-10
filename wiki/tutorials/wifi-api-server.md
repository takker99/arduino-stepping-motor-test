---
title: Wi-Fi API Server 実装 (MVP)
type: tutorial
tags: [wifi, api, http, rest, mvp, esp32, stepper]
sources:
  - raw/arduino-uno-r4-wifi/datasheet.md
  - raw/arduino-uno-r4-wifi/user-manual.md
created: 2026-08-10
updated: 2026-08-10
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
| `GET` | `/` | ヘルスチェック (HTML) | — |
| `GET` | `/status` | 現在状態 (JSON) | — |
| `POST` | `/step` | ステップ実行 | クエリ `?steps=N&dir=cw\|ccw` または body `N cw` |
| `POST` | `/stop` | 実行中の動作を中断 | — |

### レスポンス例

```http
GET /status HTTP/1.1
```

```json
{
  "state": "idle",         // "idle" | "running" | "error"
  "position": 0,           // 累積ステップ数 (任意)
  "ssid": "MyHomeWiFi",
  "ip": "192.168.1.42",
  "rssi": -55
}
```

```http
POST /step?steps=2048&dir=cw HTTP/1.1
```

```json
{ "ok": true, "requested": 2048, "direction": "cw" }
```

## 必要なもの

- [[arduino-uno-r4-wifi|UNO R4 WiFi]] (RA4M1 + ESP32-S3)
- [[uln2003|ULN2003]] driver board + [[28byj-48]] (D8–D11)
- [[mb102|MB102]] + 9V 1.3A アダプタ (モーター 5V 専用)
- USB-C ケーブル (Arduino 給電 + 書き込み)
- Wi-Fi 2.4 GHz ネットワーク (SSID + パスワード)

## 配線

[[arduino-uno-r4-wifi#ピン配置 digital/analog headers|D8–D11 → ULN2003 IN1–IN4]]
は従来通り。電源トポロジは [[arduino-uno-r4-wifi#電源トポロジ]] 参照 (Arduino は USB 給電、
モーターは MB102 から)。

## スケッチ (MVP)

`src/main.cpp` に置く。

> 📌 **実装上の注意**: UNO R4 WiFi のボードパッケージには
> Arduino 標準の `WebServer.h` が含まれていない。
> `WiFiS3` 同梱の **WiFiServer + WiFiClient** を直接使い、
> HTTP リクエストを手動パースする (公式 `SimpleWebServerWiFi.ino` と同じアプローチ)。
> 詳細は [[api/webserver-library]]。

```cpp
#include <Arduino.h>
#include <Stepper.h>
#include <WiFiS3.h>

// ===== Wi-Fi 設定 =====
// TODO: 自分の環境に合わせて変更 (本番では src/secrets.h に分離)
const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASS = "YOUR_PASS";

// ===== ステッパー設定 =====
const int STEPS_PER_REV = 4096;       // 28BYJ-48 (1/64 減速後)
const int PIN_IN1 = 8;
const int PIN_IN2 = 9;
const int PIN_IN3 = 10;
const int PIN_IN4 = 11;
Stepper myStepper(STEPS_PER_REV, PIN_IN1, PIN_IN2, PIN_IN3, PIN_IN4);

// ===== 状態 =====
enum MotorState { IDLE, RUNNING, ERROR };
MotorState state = IDLE;
long currentPos = 0;

// ===== TCP / HTTP サーバ =====
WiFiServer server(80);

String urlDecode(const String& s) {
  String out;
  out.reserve(s.length());
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '+') out += ' ';
    else if (c == '%' && i + 2 < s.length()) {
      char hex[3] = {s[i + 1], s[i + 2], 0};
      out += (char)strtoul(hex, nullptr, 16);
      i += 2;
    } else out += c;
  }
  return out;
}

void sendResponse(WiFiClient& c, int code, const char* ct, const String& body) {
  c.print("HTTP/1.1 ");
  c.print(code);
  c.print(' ');
  c.print(code == 200 ? "OK"
       : code == 400 ? "Bad Request"
       : code == 404 ? "Not Found"
       : code == 409 ? "Conflict"
       : "Error");
  c.print("\r\nContent-Type: ");
  c.print(ct);
  c.print("\r\nContent-Length: ");
  c.print(body.length());
  c.print("\r\nConnection: close\r\n\r\n");
  c.print(body);
}

String jsonStatus() {
  String s = "{\"state\":\"";
  s += (state == IDLE ? "idle" : state == RUNNING ? "running" : "error");
  s += "\",\"position\":";
  s += currentPos;
  s += ",\"ip\":\"";
  s += WiFi.localIP().toString();
  s += "\",\"ssid\":\"";
  s += WIFI_SSID;
  s += "\",\"rssi\":";
  s += WiFi.RSSI();
  s += "}";
  return s;
}

void handleStep(WiFiClient& c, const String& query) {
  if (state == RUNNING) {
    sendResponse(c, 409, "application/json", "{\"ok\":false,\"error\":\"busy\"}");
    return;
  }
  long steps = 0;
  int dir = 1;
  int qStart = query.indexOf('?');
  String params = (qStart >= 0) ? query.substring(qStart + 1) : "";
  while (params.length() > 0) {
    int amp = params.indexOf('&');
    String kv = (amp >= 0) ? params.substring(0, amp) : params;
    int eq = kv.indexOf('=');
    if (eq > 0) {
      String k = urlDecode(kv.substring(0, eq));
      String v = urlDecode(kv.substring(eq + 1));
      if (k == "steps") steps = v.toInt();
      else if (k == "dir" && (v == "ccw" || v == "-1")) dir = -1;
    }
    if (amp < 0) break;
    params = params.substring(amp + 1);
  }
  if (steps == 0) {
    sendResponse(c, 400, "application/json", "{\"ok\":false,\"error\":\"steps=0\"}");
    return;
  }
  state = RUNNING;
  myStepper.step((int)(steps * dir));
  currentPos += steps * dir;
  state = IDLE;
  String resp = "{\"ok\":true,\"requested\":";
  resp += steps;
  resp += ",\"direction\":\"";
  resp += (dir > 0 ? "cw" : "ccw");
  resp += "\"}";
  sendResponse(c, 200, "application/json", resp);
}

void handleClient(WiFiClient& c) {
  String reqLine, method, path;
  unsigned long timeout = millis() + 2000;
  while (c.connected() && millis() < timeout) {
    if (c.available()) {
      char ch = c.read();
      if (ch == '\n') {
        if (reqLine.length() == 0) break;
        int sp1 = reqLine.indexOf(' ');
        int sp2 = reqLine.indexOf(' ', sp1 + 1);
        if (sp1 > 0 && sp2 > sp1) {
          method = reqLine.substring(0, sp1);
          path = reqLine.substring(sp1 + 1, sp2);
        }
        reqLine = "";
      } else if (ch != '\r') {
        reqLine += ch;
      }
    }
  }

  if (method == "GET" && (path == "/" || path.startsWith("/?"))) {
    String html = "<h1>UNO R4 WiFi Stepper Server</h1>"
                  "<ul>"
                  "<li>GET /status</li>"
                  "<li>POST /step?steps=N&amp;dir=cw|ccw</li>"
                  "<li>POST /stop</li>"
                  "</ul>";
    sendResponse(c, 200, "text/html; charset=utf-8", html);
    return;
  }
  if (method == "GET" && path.startsWith("/status")) {
    sendResponse(c, 200, "application/json", jsonStatus());
    return;
  }
  if (method == "POST" && path.startsWith("/step")) {
    handleStep(c, path);
    return;
  }
  if (method == "POST" && path.startsWith("/stop")) {
    sendResponse(c, 200, "application/json",
                 "{\"ok\":true,\"note\":\"MVP: stop is no-op while step() blocks\"}");
    return;
  }
  sendResponse(c, 404, "application/json", "{\"ok\":false,\"error\":\"not found\"}");
}

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);
  Serial.println("Booting UNO R4 WiFi Stepper Server...");

  myStepper.setSpeed(15);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true) delay(1000);
  }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.print("Please upgrade the firmware (current: ");
    Serial.print(fv); Serial.println(")");
  }

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting Wi-Fi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Wi-Fi connect failed. Server will still start.");
  }

  server.begin();
  Serial.println("HTTP server started on port 80.");
}

void loop() {
  WiFiClient c = server.available();
  if (c) {
    handleClient(c);
    c.stop();
  }
}
```

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
# ヘルスチェック
curl http://192.168.1.42/

# ステータス
curl http://192.168.1.42/status

# 半回転 (2048 step) 時計回り
curl -X POST "http://192.168.1.42/step?steps=2048&dir=cw"

# text/plain body でも可
curl -X POST --data-binary "2048 cw" -H "Content-Type: text/plain" \
  http://192.168.1.42/step

# 逆回転
curl -X POST --data-binary "-2048" http://192.168.1.42/step
```

ブラウザから `http://<IP>/` を開くと HTML ヘルプが見える。

## 既知の制約 (MVP)

- **STA 専用**: Wi-Fi 接続失敗時サーバは立ち上がるが到達不能
- **`/stop` は no-op**: `step()` が同期的に長時間ブロックするため、
  実行中の動作を HTTP から割り込む手段がない (要ノンブロッキング化)
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
