---
title: Index — 全ページカタログ
type: index
created: 2026-08-10
updated: 2026-08-10
---

# Index — 全ページカタログ

wiki の全ページを **カテゴリ別** に整理したカタログ。
新しいページを wiki に追加したら、ここにも追記する (AGENTS.md 参照)。

## Concepts (部品・概念)

| ページ | 一行要約 | タグ |
| --- | --- | --- |
| [[arduino-uno-r4-wifi]] | **現行コントローラ** (RA4M1 + ESP32-S3, 5V, Wi-Fi 4) | arduino, mcu, board, wifi |
| [[arduino-uno-r4-minima]] | 旧コントローラ (RA4M1, 5V, Wi-Fi なし) — superseded | arduino, mcu, board, superseded |
| [[ra4m1]] | UNO R4 共通のメイン MCU (R7FA4M1AB3CFM#AA0) | mcu, renesas |
| [[esp32-s3-mini-1]] | UNO R4 WiFi 搭載の Wi-Fi/BT コプロセッサ (3.3V) | esp32, wifi, bluetooth |
| [[mb102]] | ブレッドボード電源モジュール (AMS1117-5/3.3, 9V→5V/3.3V) | power, breadboard, regulator |
| [[ams1117]] | MB102 搭載 1A LDO (熱設計の根拠) | ldo, regulator |
| [[stepper-motor]] | ステッピングモーター一般 (unipolar/bipolar, 励磁シーケンス) | stepper, theory |
| [[unipolar-vs-bipolar]] | unipolar vs bipolar の構造・駆動比較 | stepper, theory |
| [[28byj-48]] | 5V, 4 相, 1/64 減速, 4096 step/rev unipolar モーター | stepper, unipolar |
| [[uln2003]] | 7ch NPN ダーリントンアレイ (500 mA/ch) | driver, darlington |
| [[triaxial-test]] | 最終用途: 三軸試験機 (ひずみゲージ + 80 Hz 計測, ノイズ分析) | test-equipment, strain-gauge, measurement |

## Sources (一次資料のサマリ)

| ページ | 一行要約 | 元ファイル |
| --- | --- | --- |
| [[sources/arduino-uno-r4-wifi-datasheet]] | UNO R4 WiFi 公式 datasheet (ABX00087) | ABX00087-datasheet.pdf |
| [[sources/esp32-s3-mini-1-datasheet]] | ESP32-S3-MINI-1/1U datasheet | esp32-s3-mini-1_mini-1u_datasheet_en.pdf |
| [[sources/mb102-datasheet]] | MB102 AZ-Delivery 版 datasheet | MB102_Netzteil_Adapter_Datenblatt_AZ-Delivery.pdf |
| [[sources/mb102-ps-datasheet]] | MB102 Handson Technology 版 (互換) | mb102-ps.pdf |
| [[sources/ams1117-datasheet]] | AMS1117 UMW datasheet (LDO) | ams1117-ds1117.pdf |
| [[sources/arduino-uno-r4-minima-datasheet]] | UNO R4 Minima datasheet (旧方針, 参考) | ABX00080-datasheet.pdf |
| [[sources/28byj-48-datasheet]] | 28BYJ-48 Kiatronics datasheet | step-motor-5v-28byj48-datasheet.pdf |
| [[sources/uln2003a-datasheet]] | ULN2003A TI datasheet (SLRS027) | uln2003a-slrs027o-ti.pdf |

## Tutorials (チュートリアル)

| ページ | 一行要約 | 元ファイル / 備考 |
| --- | --- | --- |
| [[tutorials/wifi-api-server]] | **Wi-Fi HTTP API サーバ MVP 実装** (STA, POST /step + GET /status) | — (独自実装) |
| [[tutorials/wifi-api-design-notes]] | API サーバの将来設計 (AP フォールバック, 認証, ノンブロッキング化等) | — (設計メモ) |
| [[tutorials/mb102-power-wiring]] | MB102 + 9V 1.3A + Arduino USB 給電 の電源トポロジ | — (独自実装) |
| [[tutorials/stepper-library-examples]] | 公式 "Stepper Motors with Arduino" 4 サンプル | stepper-library-examples.md |
| [[tutorials/unipolar-stepper-motor]] | 公式 "Unipolar Stepper Motor" ライブラリ不使用例 | unipolar-stepper-motor-tutorial.md |
| [[tutorials/platformio-setup]] | PlatformIO 環境構築・ビルド手順 (aarch64 の罠含む) | — (実績メモ) |

## API (Arduino API リファレンス)

| ページ | 一行要約 | 元ファイル |
| --- | --- | --- |
| [[api/wifis3-library]] | UNO R4 WiFi 用 Wi-Fi 接続ライブラリ (WiFiS3) | (ボードパッケージ同梱) |
| [[api/webserver-library]] | Arduino 標準 HTTP サーバ (WiFiS3 上で動作) | (ボードパッケージ同梱) |
| [[api/stepper-library]] | `Stepper.h` ライブラリ API | Stepper.cpp, api.md |
| [[api/pinMode]] | デジタルピンモード設定 | pinMode.adoc |
| [[api/digitalWrite]] | デジタルピンに HIGH/LOW 出力 | digitalWrite.adoc |
| [[api/delay]] | 指定ミリ秒停止 | delay.adoc |

## システムページ

| ページ | 一行要約 |
| --- | --- |
| [[overview]] | プロジェクト俯瞰・current state |
| [[AGENTS]] | wiki 運用スキーマ |
| [[log]] | 時系列ログ (append-only) |

## タグ別クロスリファレンス

### stepper

- [[stepper-motor]], [[unipolar-vs-bipolar]], [[28byj-48]]
- [[tutorials/stepper-library-examples]], [[tutorials/unipolar-stepper-motor]]
- [[api/stepper-library]]

### arduino / mcu

- [[arduino-uno-r4-wifi]], [[arduino-uno-r4-minima]], [[ra4m1]], [[esp32-s3-mini-1]]
- [[api/pinMode]], [[api/digitalWrite]], [[api/delay]]

### wifi / api server

- [[api/wifis3-library]], [[api/webserver-library]]
- [[tutorials/wifi-api-server]], [[tutorials/wifi-api-design-notes]]

### power

- [[mb102]], [[ams1117]]
- [[tutorials/mb102-power-wiring]]

### driver

- [[uln2003]]

### test-equipment / measurement

- [[triaxial-test]]

### datasheet

- [[sources/arduino-uno-r4-wifi-datasheet]]
- [[sources/arduino-uno-r4-minima-datasheet]]
- [[sources/esp32-s3-mini-1-datasheet]]
- [[sources/mb102-datasheet]]
- [[sources/mb102-ps-datasheet]]
- [[sources/ams1117-datasheet]]
- [[sources/28byj-48-datasheet]]
- [[sources/uln2003a-datasheet]]

## 更新方法

1. 新しいページを wiki に追加したら、上の対応するカテゴリに 1 行追記する
2. 既存ページのリンクを更新する場合もここを最新に保つ
3. frontmatter の `updated` フィールドも同時に更新する
