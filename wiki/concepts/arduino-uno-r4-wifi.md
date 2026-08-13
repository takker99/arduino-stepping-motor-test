---
title: Arduino UNO R4 WiFi
type: concept
tags: [arduino, mcu, board, wifi, esp32, renesas]
sources:
  - raw/arduino-uno-r4-wifi/datasheet.md
  - raw/arduino-uno-r4-wifi/user-manual.md
  - raw/arduino-uno-r4-wifi/ABX00087-datasheet.pdf
  - raw/arduino-uno-r4-wifi/ABX00087-full-pinout.pdf
  - raw/arduino-uno-r4-wifi/ABX00087-schematics.pdf
  - raw/arduino-uno-r4-wifi/esp32-s3-mini-1_mini-1u_datasheet_en.pdf
  - raw/arduino-uno-r4-wifi/ra4m1-datasheet.pdf
created: 2026-08-10
updated: 2026-08-10
---

# Arduino UNO R4 WiFi

SKU: **ABX00087**。本プロジェクトの **コントローラ**。
[[arduino-uno-r4-minima|UNO R4 Minima]] の置き換え先 (2026-08-10 方針変更)。

[[ra4m1|RA4M1]] (R7FA4M1AB3CFM#AA0) を **メイン MCU** として搭載し、
**Wi-Fi® / Bluetooth® LE コプロセッサ** として [[esp32-s3-mini-1|ESP32-S3-MINI-1-N8]] を
搭載した 2-MCU 構成の UNO R4 派生ボード。

## 主要スペック

| 項目 | 値 |
| --- | --- |
| メイン MCU | [[ra4m1]] (R7FA4M1AB3CFM#AA0) |
| コプロセッサ | [[esp32-s3-mini-1|ESP32-S3-MINI-1-N8]] (Wi-Fi 4 / BT 5 LE) |
| コア | 48 MHz Arm Cortex-M4 + FPU (RA4M1) |
| フラッシュ / SRAM / EEPROM (RA4M1) | 256 kB / 32 kB / 8 kB |
| 動作電圧 (RA4M1 GPIO) | **5 V** |
| 動作電圧 (ESP32-S3) | **3.3 V** |
| VIN (barrel jack) | 6 – 24 V (typ 7 V) |
| 5V ピン最大電流 | VIN 給電時 **1.2 A** / USB 給電時 **2 A** |
| GPIO 1 ピンあたり電流 | **最大 8 mA** |
| GPIO 合計電流 (全ピン) | **最大 60 mA** |
| USB | USB-C (Native USB; デフォルトで ESP32-S3 を経由して RA4M1 に到達) |
| Wi-Fi | 802.11 b/g/n (2.4 GHz) 最大 150 Mbps |
| Bluetooth | Bluetooth® 5 LE 最大 2 Mbps (Wi-Fi とアンテナ共有で排他) |
| LED Matrix | 12 × 8  赤色 LED (charlieplexing) |
| DAC | A0 (最大 12-bit) |
| OPAMP | A1 (+), A2 (-), A3 (OUT) |
| RTC | 内蔵 + VRTC ピン (1.6 – 3.6 V バックアップ) |
| その他 | Qwiic コネクタ (3.3 V, Wire1), CAN (D4/D5) |

ステッピングモーター駆動では GPIO 8 mA / pin の制約が重要。
[[28byj-48]] のコイル抵抗は 50 Ω (5V, 100 mA/coil) なので、
**GPIO で直接駆動できない**。[[uln2003]] (ダーリントン・アレイ) を必ず介す。
(これは Minima と同じ制約。)

## 2-MCU 構成とレベル変換

RA4M1 は 5 V、ESP32-S3 は 3.3 V で動作する。両者間の通信線は
**TXB0108DQSR** (5 V ↔ 3.3 V 双方向レベル変換) を介して接続されている。
ユーザが両チップ間の配線を直接扱うことは通常ない。

> ⚠️ ESP32-S3 のピン (3.3 V) を RA4M1 のピン (5 V) に直結しないこと。
> ESP ヘッダ (`ESP_IO42`, `ESP_IO41`, `ESP_TXD0`, `ESP_RXD0`, `ESP_DOWNLOAD`, GND)
> は 3.3 V 系。[[uln2003]] 等の 5 V 機器を接続してはならない。

## USB プログラミング

UNO R4 Minima と異なり、**デフォルトで USB は ESP32-S3 を経由**して RA4M1 に届く。
(これは ESP32-S3 が Serial bridge として動作するため。)
PlatformIO / Arduino IDE からの操作は透過的だが、内部トポロジを意識する必要がある。

RA4M1 に USB を直結したい場合は:
- **ソフト的**: `pinMode(21, OUTPUT); digitalWrite(21, HIGH);` を `setup()` に書く
  (D21 を HIGH → スイッチ U2/U6 が切り替わり RA4M1 直結)
- **ハード的**: 基板裏の "RA4M1 USB" パッドをハンダブリッジ

通常はデフォルトのままとする。

## 電源トポロジ

```
                ┌─────────────────────┐
                │   9V 1.3A ACアダプタ  │
                └────────┬────────────┘
                         │ (barrel 5.5×2.1 mm)
                         ▼
                ┌─────────────────────┐
                │  [[mb102|MB102]]     │
                │  AMS1117-5.0/3.3    │
                └────┬───────────┬────┘
                     │           │
                  5V rail     3.3V rail
                     │           │
            ┌────────┴──┐    (unused or Qwiic)
            │           │
            ▼           ▼
        28BYJ-48     breadboard
        (5V coil)

        Arduino UNO R4 WiFi は
        USB-C (開発時) または
        別 USB 充電器 (本番) で給電
        ──> 5V レールを共有しない
```

開発時は Arduino を USB-C で PC から給電し、MB102 はモーター専用 5V レールとして使う。
本番運用 (ヘッドレス API server) では Arduino を別の USB 充電器 (5V) で給電する。
([[tutorials/mb102-power-wiring]] 参照)

## ピン配置 (digital/analog headers)

公式 full pinout (`ABX00087-full-pinout.pdf`) より。
**D0–D13、A0–A5 の Arduino ピン番号は R4 Minima と同一**。
[[uln2003]] ドライバーボードとの接続は本プロジェクトでは **D7–D4**
(IN1–IN4, 2026-08-12 確定。旧 D8–D11 から変更)。

| Arduino 名 | RA4M1 端子 | 主な用途 | 本プロジェクトでの用途 |
| --- | --- | --- | --- |
| D0/RX | P301 | UART RX | (Serial1 RX, デバッグ用) |
| D1/TX | P302 | UART TX | (Serial1 TX, デバッグ用) |
| D2 | P104 | GPIO / 割り込み | (空き) |
| ~D3 | P105 | GPIO / PWM / 割り込み | (空き) |
| D4 | P106 | GPIO | (空き, CANRX 兼用) |
| ~D5 | P107 | GPIO / PWM | (空き, CANTX 兼用) |
| ~D6 | P111 | GPIO / PWM | (空き) |
| D7 | P112 | GPIO | (空き) |
| D8 | P304 | GPIO | **ULN2003 IN1 (Blue)** |
| ~D9 | P303 | GPIO / PWM | **ULN2003 IN2 (Pink)** |
| ~D10 | P103 | GPIO / PWM / SPI CS / CANTX | **ULN2003 IN3 (Yellow)** |
| ~D11 | P411 | GPIO / PWM / SPI COPI | **ULN2003 IN4 (Orange)** |
| D12 | P410 | GPIO / SPI CIPO | (SPI 使うなら) |
| D13 | P102 | GPIO / SPI SCK / CANRX / LED_BUILTIN | (SPI 使うなら) |
| A0 | P014 | アナログ入力 / DAC | (DAC, アナログ入力) |
| A1 | P000 | アナログ入力 / OPAMP+ | (アナログ入力) |
| A2 | P001 | アナログ入力 / OPAMP- | (アナログ入力) |
| A3 | P002 | アナログ入力 / OPAMP OUT | (アナログ入力) |
| A4 / D18 | P101 | アナログ入力 / I2C SDA | (I2C) |
| A5 / D19 | P100 | アナログ入力 / I2C SCL | (I2C) |

## 公式 PWM 対応ピン

| Arduino | RA4M1 | Timer |
| --- | --- | --- |
| ~D3 | P105 | GTIOC1A |
| ~D5 | P107 | GTIOC0A |
| ~D6 | P111 | GTIOC3A |
| ~D9 | P303 | GTIOC7B |
| ~D10 | P103 | GTIOC2A |
| ~D11 | P411 | GTIOC6A |

## 通信バスの数

- **UART (Serial1)**: D0 (RX), D1 (TX) — `Serial` は USB-C 経由
- **SPI**: D10 (CS), D11 (COPI), D12 (CIPO), D13 (SCK) + ICSP ヘッダ
- **I2C #0 (Wire)**: D18 (SDA), D19 (SCL) — 5 V 系, A4/A5 兼用
- **I2C #1 (Wire1)**: Qwiic コネクタ — 3.3 V 系
- **CAN**: D4 (CANTX) / D5 (CANRX) — 外部トランシーバ必要

> ⚠️ Qwiic は **3.3 V のみ**。5 V I2C デバイスを繋がないこと。

## Wi-Fi / Bluetooth の使い方

Wi-Fi はボードパッケージに内蔵の **[[api/wifis3-library|WiFiS3]] ライブラリ** を使う。
Bluetooth LE は `ArduinoBLE` を使う。**Wi-Fi と BT はアンテナを共有するため同時使用不可**。

ライブラリ所管:
- Wi-Fi: `WiFiS3.h` — Arduino UNO R4 Boards パッケージに同梱
- BT: `ArduinoBLE.h` — 別途ライブラリマネージャからインストール

## 開発環境

- **ボードパッケージ**: "Arduino UNO R4 Boards" (ボードマネージャ)
- **PlatformIO 環境**: `env:uno_r4_wifi` (platform = renesas-ra, board = uno_r4_wifi)
- **フレームワーク**: arduino
- **ツールチェーン**: aarch64 Linux では `toolchain-gccarmnoneeabi@~1.120301.0` で上書きが必要
  (詳細は [[tutorials/platformio-setup]])

## LED Matrix

12 × 8 赤色 LED が charlieplexing で接続されており、`Arduino_LED_Matrix` ライブラリで
アニメーション表示ができる。本プロジェクトでは状態表示 (Wi-Fi 接続状況 / 動作中フラグ)
に利用する想定。

## Minima からの差分まとめ

| 項目 | UNO R4 Minima | UNO R4 WiFi |
| --- | --- | --- |
| SKU | ABX00080 | ABX00087 |
| MCU | [[ra4m1]] | [[ra4m1]] (同一) |
| コプロセッサ | なし | [[esp32-s3-mini-1|ESP32-S3-MINI-1-N8]] |
| USB 経路 | RA4M1 直結 | デフォルト ESP32-S3 経由 |
| LED Matrix | なし | 12 × 8 |
| DAC | なし | A0 (12-bit) |
| OPAMP | なし | A1/A2/A3 |
| Qwiic | なし | あり (Wire1, 3.3V) |
| CAN | なし | D4/D5 (外部トランシーバ要) |
| RTC バックアップ | なし | VRTC ピン (1.6-3.6 V) |
| Wi-Fi / BT | なし | Wi-Fi 4 + BT 5 LE |
| GPIO 8 mA 制約 | あり | あり (RA4M1 共通) |
| ピン番号 D0-D13, A0-A5 | 同じ | **同じ** (流用可) |

## 関連ページ

- [[arduino-uno-r4-minima]] — 旧コントローラ (Minima)。本プロジェクトでは不採用
- [[ra4m1]] — メイン MCU 詳細
- [[esp32-s3-mini-1]] — Wi-Fi コプロセッサ詳細
- [[mb102]] — 電源モジュール
- [[uln2003]] — モータードライバ
- [[28byj-48]] — 駆動対象モーター
- [[tutorials/wifi-api-server]] — Wi-Fi API server 構築手順 (MVP)
- [[tutorials/wifi-api-design-notes]] — API 設計ノート (将来拡張用)
- [[sources/arduino-uno-r4-wifi-datasheet]] — 公式 datasheet サマリ
