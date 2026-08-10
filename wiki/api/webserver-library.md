---
title: WiFiServer + WiFiClient (UNO R4 WiFi の HTTP 実装)
type: api
tags: [http, server, wifi, library, api, rest, wifiserver]
sources:
  - raw/arduino-uno-r4-wifi/user-manual.md
created: 2026-08-10
updated: 2026-08-10
---

# WiFiServer + WiFiClient (UNO R4 WiFi の HTTP 実装)

[[arduino-uno-r4-wifi|UNO R4 WiFi]] で HTTP サーバを建てる方法。
**Arduino UNO R4 Boards パッケージには `WebServer.h` ライブラリが含まれていない**
(`arduino-libraries/WebServer` は AVR 系用)。UNO R4 WiFi では
`WiFiS3` パッケージ同梱の **WiFiServer + WiFiClient** を直接使い、
HTTP リクエストを手動パースする。Arduino 公式サンプル
(`SimpleWebServerWiFi.ino`, `WiFiWebServer.ino`) もこの方式。

> 📌 本ページは UNO R4 WiFi 固有の話。ESP32 や Arduino AVR の `WebServer.h` は
> 高レベル API だが、UNO R4 WiFi では使えない。

## よく使う API

### WiFiServer — TCP 待受

```cpp
WiFiServer server(80);  // ポート 80
server.begin();
```

| 関数 | 用途 |
| --- | --- |
| `WiFiServer server(port)` | コンストラクタ |
| `server.begin()` | 待受開始 |
| `server.available()` | 接続中クライアント取得 (なければ空オブジェクト) |
| `WiFiClient c = server.available()` | ループ内でクライアントを取り出す典型形 |

### WiFiClient — 1 接続

| 関数 | 用途 |
| --- | --- |
| `c.connected()` | 接続中か |
| `c.available()` | 読み取り可能バイト数 |
| `c.read()` | 1 バイト読み |
| `c.print(...)` / `c.println(...)` | 送信 |
| `c.stop()` | 切断 (重要: 必ず呼ぶ) |

`WiFiClient` は `Print` を継承しているので `print` / `println` で
HTTP レスポンスを流せる。

## 最小実装パターン

公式 `SimpleWebServerWiFi.ino` をベースに:

```cpp
WiFiServer server(80);

void setup() {
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  server.begin();
}

void loop() {
  WiFiClient c = server.available();
  if (c) {
    // 1 行目 ("METHOD PATH HTTP/x.x") を読む
    String reqLine;
    while (c.connected()) {
      if (c.available()) {
        char ch = c.read();
        if (ch == '\n') break;       // 最初の改行で 1 行目終わり
        if (ch != '\r') reqLine += ch;
      }
    }

    // method と path を抽出
    int sp1 = reqLine.indexOf(' ');
    int sp2 = reqLine.indexOf(' ', sp1 + 1);
    String method = reqLine.substring(0, sp1);
    String path   = reqLine.substring(sp1 + 1, sp2);

    // レスポンス送信
    c.println("HTTP/1.1 200 OK");
    c.println("Content-Type: text/plain");
    c.println("Connection: close");
    c.println();
    c.print("Hello from ");
    c.print(method);
    c.print(' ');
    c.print(path);

    c.stop();
  }
}
```

## HTTP パースの詳細

実用的な API サーバでは:
- リクエストラインを method / path / query に分割
- クエリ文字列 (`?key=value&...`) をパース
- POST の body を読む (Content-Length バイト分)
- Content-Type (URL-encoded / JSON / text/plain) 別のパース
- レスポンスの `Content-Length` を必ず付ける (`Print` 系だけだと `Connection: close` 依存で動くが堅牢ではない)

MVP スケッチ ([[tutorials/wifi-api-server]]) で
`handleClient()`, `sendResponse()`, `urlDecode()` ヘルパを使った
実装パターンを示している。

## ノンブロッキング化

`WiFiClient` を使った最小実装は 1 接続処理中にブロックする。
[[tutorials/wifi-api-design-notes|設計ノート]] で
ステートマシン化 (loop 内で少しずつステップを進める) を議論している。

## メモリ消費 (感覚値)

- WiFiS3 + WiFiServer + HTTP 手動パース で RA4M1 (SRAM 32 KB) 上は
  - 起動後 free RAM: **~27 KB**
  - クライアント 1 接続中: 数百バイト + URL バッファ
- WiFiSSLClient 関連は **本プロジェクトでは使わない** (平文 HTTP のみ)

実測: [[tutorials/platformio-setup]] のビルド結果参照。

## 関連ページ

- [[api/wifis3-library]] — Wi-Fi 接続 (本 API の前提)
- [[tutorials/wifi-api-server]] — MVP 実装
- [[tutorials/wifi-api-design-notes]] — 将来拡張 (認証、ジョブキュー等)
- [[arduino-uno-r4-wifi]]
