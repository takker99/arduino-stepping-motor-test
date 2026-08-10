---
title: Wi-Fi API 設計ノート (将来拡張)
type: tutorial
tags: [wifi, api, design, future, esp32]
sources: []
created: 2026-08-10
updated: 2026-08-10
---

# Wi-Fi API 設計ノート (将来拡張)

[[tutorials/wifi-api-server|MVP]] を高機能化するための設計メモ。
**実装はしない** (2026-08-10 時点)。要件が固まった段階で本ノートから実装ページを起こす。

## Wi-Fi モード切替

### 背景

MVP は STA (子機) 専用。Wi-Fi 接続に失敗すると API サーバに到達できなくなる。
屋外や Wi-Fi 不在環境でもスタンドアロンで使いたいケースに対応するため、
**AP (親機) モードへのフォールバック** が候補。

### 設計案

```cpp
// 疑似コード
bool sta_ok = tryConnectSta(SSID, PASS, 15000);
if (!sta_ok) {
  Serial.println("STA failed, fallback to AP");
  WiFi.beginAP("UNO-R4-WiFi-Stepper", "setup1234");
}
// HTTP サーバはどちらでも立ち上げ
```

クライアント視点:
- STA 成功 → 自宅 LAN の IP (`192.168.1.x`) でアクセス
- AP 失敗 → スマホ等を "UNO-R4-WiFi-Stepper" に接続し、`192.168.4.1` でアクセス
- 切替はスケッチ内部で自動。ユーザは意識しなくてよい

### トレードオフ

- AP モード中はインターネット出られない (UNO 自身が親機になるため)
- 同時 STA + AP (WiFiS3 で対応) も検討可だが、複雑化
- 認証なし AP はセキュリティリスク → 最低 WPA2-PSK + ランダム PW を EEPROM に保存

→ まずは「STA 失敗時に AP フォールバック」の単純な実装から。

## API エンドポイント設計 (高機能版)

MVP を超えた候補。要件次第で採用判断。

### 案 A: 高機能 REST (中粒度)

| メソッド | パス | 用途 | ボディ (JSON) |
| --- | --- | --- | --- |
| `GET` | `/api/status` | 状態・位置 | — |
| `POST` | `/api/rotate` | 角度指定回転 | `{"degrees": 90, "direction": "cw"}` |
| `POST` | `/api/rotate` (相対) | 相対角度 | `{"delta_degrees": -15}` |
| `POST` | `/api/home` | 原点復帰 | `{"speed": 15}` |
| `POST` | `/api/stop` | 即時停止 | `{"decel": "immediate"\|"ramp"}` |
| `POST` | `/api/speed` | 速度変更 | `{"rpm": 20}` |
| `GET` | `/api/info` | デバイス情報 (SSID, IP, FW version) | — |

単位が "度" だと人間に扱いやすい。`degrees = steps * 360 / 4096`。

### 案 B: シーケンス / ジョブキュー (高機能)

| メソッド | パス | 用途 |
| --- | --- | --- |
| `POST` | `/api/jobs` | ジョブ投入 (`[{action: "step", args: {...}}, ...]`) |
| `GET` | `/api/jobs/:id` | ジョブ状態 |
| `DELETE` | `/api/jobs/:id` | キャンセル |
| `WS` | `/ws` | WebSocket で進捗通知 |

長時間動作をバックグラウンド実行 + 進捗 push 通知。
RA4M1 SRAM 32 KB でジョブキューのリングバッファを自前実装するか、
`ArduinoJson` の `StaticJsonDocument` で小サイズキューを保持。

### 案 C: gRPC / MQTT

リソース的に Arduino で gRPC は非現実的。
MQTT は [[sources/arduino-uno-r4-wifi-datasheet|Arduino Cloud 統合]] の文脈で有力だが、
本プロジェクトでは REST で十分。

### 推奨順序

1. **MVP (シンプル POST /step + GET /status)** ← 2026-08-10 着手
2. AP フォールバック + 認証
3. 案 A の高機能 REST
4. (必要なら) 案 B のジョブキュー

## ノンブロッキング化

MVP の最大の問題は `step()` が同期ループで数万ステップを処理するため、
その間 HTTP リクエストが来ても `handleClient()` が呼ばれず、
`/stop` が事実上 no-op になること。

### 対策案: ステートマシン化

```cpp
// loop() 内で常に少しずつステップを進める
void loop() {
  server.handleClient();

  if (state == RUNNING && stepTarget != stepDone) {
    int batch = min(STEPS_PER_LOOP, abs(stepTarget - stepDone));
    myStepper.step(batch * stepDir);
    stepDone += batch * stepDir;
    if (stepDone == stepTarget) state = IDLE;
  }
}
```

- `STEPS_PER_LOOP = 4` 程度なら 1 ループあたり ~10 ms で済み、
  HTTP レスポンス遅延は許容範囲
- `/stop` は `state = IDLE` するだけ。`loop()` が次回 step を打たなくなる
- スムーズさが必要な場合は [[tutorials/stepper-library-examples|AccelStepper ライブラリ]]
  (要確認: UNO R4 WiFi + arduino-libs で提供されているか)

## 認証

MVP は LAN 内誰でもアクセス可。自宅 LAN なら問題ないが、
共有スペースや将来インターネット公開するなら要 Basic 認証 or Token。

```cpp
// Basic 認証の最小実装 (平文: デモ用)
const char* AUTH_USER = "admin";
const char* AUTH_PASS = "changeme";

bool checkAuth() {
  if (!server.hasHeader("Authorization")) return false;
  String h = server.header("Authorization");
  // h = "Basic <base64(user:pass)>"
  // 実装略
  return true;
}
```

本格運用は TLS (WiFiS3 単体では不可。ESP32 直接プログラミングが必要)。

## 状態の永続化

EEPROM (8 KB) で:
- 現在ステップ数 (電源断復帰用)
- Wi-Fi 認証情報
- 設定 (speed, etc.)

`EEPROM.h` を使う。書き込みサイクル 100k 回制限あり (loop 内で書かない)。

## LED Matrix で状態表示

Wi-Fi 接続中 / 動作中 / エラー を 12 × 8 LED で可視化すると運用しやすい。
[[arduino-uno-r4-wifi#LED Matrix|Arduino_LED_Matrix ライブラリ]] で:

- Wi-Fi 未接続: 「?」マーク
- 接続中・待機: 静止パターン (例: ハート)
- 動作中: 回転する矢印
- エラー: 「X」

## 監視・ロギング

- `Serial.println()` で起動時 + 各リクエストのログ
- 本番では RTC ([[arduino-uno-r4-wifi|RTC + VRTC]]) でタイムスタンプ付与
- リモート監視は [[tutorials/wifi-api-server|MVP]] の `GET /status` を
  クライアントがポーリング (1 Hz 程度)

## セキュリティチェックリスト (本格運用前)

- [ ] 認証 (Basic or Token)
- [ ] HTTPS (TLS) — WiFiS3 では不可。ESP32 直接 fw 書き換え必要
- [ ] 入力バリデーション (steps が int 範囲 / 異常に大きくない)
- [ ] リクエストレート制限 (DoS 対策)
- [ ] ログのサニタイズ (PII, 認証情報)

## 関連ページ

- [[tutorials/wifi-api-server]] — MVP 実装
- [[api/wifis3-library]] / [[api/webserver-library]]
- [[arduino-uno-r4-wifi]]
