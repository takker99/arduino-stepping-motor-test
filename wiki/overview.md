---
title: Overview — arduino-stepping-motor-test
type: overview
created: 2026-08-10
updated: 2026-08-12
---

# Overview — arduino-stepping-motor-test

この wiki は **Arduino UNO R4 WiFi + 28BYJ-48 + ULN2003A + MB102** でステッピング
モーターを Wi-Fi 経由で制御するためのナレッジベースである。

> 📌 **2026-08-10 方針変更**: 旧 [[arduino-uno-r4-minima]] ベースの有線制御 →
> 新 [[arduino-uno-r4-wifi]] ベースの **Wi-Fi HTTP API サーバ化** に移行。
> 詳細は [[log]] を参照。

## プロジェクトの状態 (2026-08-10)

**フェーズ**: 方針確定・資料 ingest 完了。実装はこれから。

- ハードウェア選定確定:
  - **コントローラ**: [[arduino-uno-r4-wifi]] (RA4M1 + ESP32-S3)
  - **モーター**: [[28byj-48]] (5V, 1/64 減速, 4096 step/rev)
  - **ドライバ**: [[uln2003]] (7ch ダーリントン, 500 mA/ch)
  - **電源モジュール**: [[mb102]] + 9V 1.3A AC アダプタ (モーター 5V 専用)
  - **Arduino 給電**: USB-C (開発時 = PC, 本番 = 別 USB 充電器)
- API サーバ方針:
  - **STA モード** で Wi-Fi 接続 (MVP)。将来 AP フォールバック検討 ([[tutorials/wifi-api-design-notes]])
  - REST over HTTP。MVP は `POST /step` + `GET /status` の最小構成 ([[tutorials/wifi-api-server]])
- PlatformIO 環境: 旧 `env:uno_r4_minima` → 新 `env:uno_r4_wifi`
  ([[tutorials/platformio-setup]])
- wiki: raw/arduino-uno-r4-wifi/, raw/mb102/ の資料を新規 ingest 完了

## 主要トピック

### ハードウェア

- [[arduino-uno-r4-wifi]] — コントローラ (RA4M1 + ESP32-S3, 5 V, 14 dig + 6 ana)
- [[esp32-s3-mini-1]] — Wi-Fi/BT コプロセッサ (3.3 V, 内部 trace-antenna)
- [[28byj-48]] — 駆動対象モーター (5 V, 4 相, 1/64, 4096 step/rev)
- [[uln2003]] — 駆動用ダーリントンアレイ (Arduino GPIO 8 mA 制約 → 増幅)
- [[mb102]] — ブレッドボード電源モジュール (AMS1117-5.0 / -3.3)
- [[ams1117]] — MB102 搭載 LDO (熱設計の根拠)

### 概念

- [[stepper-motor]] — ステッピングモーター一般
- [[unipolar-vs-bipolar]] — 駆動方式による分類
- [[ra4m1]] — UNO R4 共通のメイン MCU

### API

- [[api/wifis3-library]] — WiFiS3 (Wi-Fi 接続)
- [[api/webserver-library]] — WebServer (HTTP サーバ)
- [[api/stepper-library]] — Stepper.h (モーター制御)
- [[api/pinMode]] / [[api/digitalWrite]] / [[api/delay]]

### 配線 / チュートリアル

- [[tutorials/mb102-power-wiring]] — 電源トポロジ + 配線
- [[tutorials/wifi-api-server]] — Wi-Fi API サーバ MVP 実装
- [[tutorials/wifi-api-design-notes]] — 将来拡張の設計メモ
- [[tutorials/stepper-library-examples]] — Stepper ライブラリのサンプル集
- [[tutorials/unipolar-stepper-motor]] — ライブラリを使わない最小例
- [[tutorials/platformio-setup]] — PlatformIO 環境構築・ビルド手順

### 一次資料

- [[sources/arduino-uno-r4-wifi-datasheet]] — ABX00087
- [[sources/esp32-s3-mini-1-datasheet]]
- [[sources/mb102-datasheet]] — AZ-Delivery 版
- [[sources/mb102-ps-datasheet]] — Handson 版 (互換)
- [[sources/ams1117-datasheet]]
- [[sources/arduino-uno-r4-minima-datasheet]] — 旧方針の記録
- [[sources/28byj-48-datasheet]]
- [[sources/uln2003a-datasheet]]

## 配線 (最終形)

```
                  ┌─────────────────────────────┐
                  │  Arduino UNO R4 WiFi         │
                  │                              │
│  D7  ──────► IN1 ┐           │
│  D6  ──────► IN2 ├──┐        │
│  D5  ──────► IN3 ├──┤        │
│  D4  ──────► IN4 ├──┤        │
                  │                  │  │        │
                  │  GND ────────────┼──┼─┐      │
                  └─────────────────┼──┼─┼──────┘
                                    │  │ │
              ┌─────────────────────┼──┼─┘
              │                     │  │
              │  ┌──────┐  ┌────────┴──┴────────────┐
              │  │  +   │  │  ULN2003 driver board  │
              │  │      │  │                         │
              │  │  −   │  │  OUT1 ──► Blue         │
              │  │Bread │  │  OUT2 ──► Pink          │
              │  │board │  │  OUT3 ──► Yellow        │
              │  └──────┘  │  OUT4 ──► Orange       │
              │     ▲  ▲    │  COM  ◄── 5V (ブレッド +レール) │
              │     │  │    │  GND  ◄── ブレッド −レール │
              │  ┌──┴──┴─┐  └────────┬────────────────┘
              │  │MB102 │           │
              │  │  5V  │           │
              │  │  GND │           ▼
              │  └──┬───┘        ┌──────────┐
              │     │            │ 28BYJ-48 │
              │     │            │ Red ◄────┼── 5V (MB102 5V rail)
              │     │            └──────────┘
              │     │
              │  ┌──┴─────┐
              │  │ 9V 1.3A│
              │  │ AC adp │
              │  └────────┘
              │
   Arduino UNO R4 WiFi は USB-C 経由で
   PC (開発時) / 別 USB 充電器 (本番) から給電
```

> ⚠️ **実機配線 (2026-08-12 確定)**: IN1–IN4 → **D7–D6–D5–D4**。
> 旧記述 (D8–D11) から訂正。コード `src/main.cpp` も 7, 6, 5, 4 に変更済み。

## 電源トポロジ (2026-08-10 決定)

- **9V 1.3A AC アダプタ** → MB102 DC IN
- MB102 内部 AMS1117-5.0 → ブレッドボード +5V レール → 28BYJ-48 (赤) + ULN2003 COM
- MB102 内部 AMS1117-3.3 → ブレッドボード +3.3V レール (本プロジェクトでは未使用)
- **Arduino UNO R4 WiFi は USB-C 経由で別系統給電** (MB102 5V からは取らない)

選定理由は [[arduino-uno-r4-wifi#電源トポロジ]] / [[tutorials/mb102-power-wiring]]。
要点: AMS1117 の 700 mA 枠を Arduino (≈ 250 mA peak) + 28BYJ-48 (≈ 400 mA peak)
で分け合うと Wi-Fi ピーク + 2 相励磁の最悪条件で熱的に厳しいため分離。

## Wi-Fi API サーバ (MVP)

| メソッド | パス | 用途 |
| --- | --- | --- |
| `GET` | `/` | ヘルスチェック (HTML) |
| `GET` | `/status` | 現在状態 (JSON) |
| `POST` | `/step?steps=N&dir=cw\|ccw&speed=RPM` | ステップ実行 (speed 1–60 rpm, 省略時は現在値) |
| `POST` | `/stop` | 通電遮断 (全ピン LOW, ホールド解除) |

STA モード (自宅 Wi-Fi に接続) で `192.168.1.x:80` で待ち受け。
スケッチは [[tutorials/wifi-api-server]] 参照。

## 次のアクション

現状: **モーター回転確認済み** (2026-08-13, 相順修正で解決)。
Wi-Fi API サーバ疎通済み (192.168.11.3)、`POST /step?steps=N&dir=cw|ccw&speed=RPM` 動作中。
詳細は [[log#2026-08-13]] のエントリ群参照。

1. (任意) 連続動作確認: `dir=ccw` 逆回転 / 複数回転 / 角度指定 (steps=512 = 90°)
2. STEPS_PER_REV 2048・相順問題の知見を次回の回路設計に反映
3. 将来: AP フォールバック / `/stop` のノンブロッキング化 (実行中の中断) / 認証

## 関連ページ

- [[AGENTS]] — wiki 運用スキーマ
- [[index]] — 全ページカタログ
- [[log]] — 時系列ログ
