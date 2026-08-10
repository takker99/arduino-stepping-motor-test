---
title: Arduino UNO R4 Minima
type: concept
tags: [arduino, mcu, board, renesas, superseded]
sources:
  - raw/arduino-uno-r4-minima/ABX00080-datasheet.pdf
  - raw/arduino-uno-r4-minima/datasheet.md
  - raw/arduino-uno-r4-minima/user-manual.md
  - raw/arduino-uno-r4-minima/ABX00080-full-pinout.pdf
  - raw/arduino-uno-r4-minima/ABX00080-schematics.pdf
created: 2026-08-10
updated: 2026-08-10
---

# Arduino UNO R4 Minima

SKU: **ABX00080**。32bit 化した UNO シリーズ。

> ⚠️ **2026-08-10 方針変更**: 本プロジェクトでは [[arduino-uno-r4-wifi|UNO R4 WiFi]] に
> 移行しました。本ページは旧方針の記録として残してあります。
> 新規スケッチ・配線は [[arduino-uno-r4-wifi]] 側のページを参照してください。

![Top view](raw/arduino-uno-r4-minima/ABX00080-full-pinout.pdf) (※ raw を直接参照)

## 主要スペック

| 項目 | 値 |
| --- | --- |
| MCU | [[ra4m1]] (R7FA4M1AB3CFM#AA0) |
| コア | 48 MHz Arm Cortex-M4 + FPU |
| フラッシュ / SRAM / EEPROM | 256 kB / 32 kB / 8 kB |
| 動作電圧 | **5 V** |
| VIN (barrel jack) | 6 – 24 V (typ 7 V) |
| 5V ピン最大電流 | VIN 給電時 **1.2 A** / USB 給電時 **2 A** (理論値。実機・実装による) |
| 3.3V ピン | VCC_USB から直接 (降圧コンバータ非経由)。**5 V 系 I/O とは別系統** |
| GPIO 1 ピンあたり電流 | **最大 8 mA** |
| GPIO 合計電流 (全ピン) | **最大 60 mA** |
| USB | USB-C (Native USB; Serial は内部 MCU に直結) |

ステッピングモーター駆動では GPIO 8 mA / pin の制約が重要。
[[28byj-48]] のコイル抵抗は 50 Ω (5V, 100 mA/coil) なので、
**GPIO で直接駆動できない**。[[uln2003]] (ダーリントン・アレイ) を必ず介す。

## ピン配置 (digital/analog headers)

公式 full pinout (`ABX00080-full-pinout.pdf`) より。
D13 側に SPI, D0/D1 に UART, A4/A5 (=D18/D19) に I2C。

| Arduino 名 | RA4M1 端子 | 主な用途 |
| --- | --- | --- |
| D0/RX | P301 | UART RX |
| D1/TX | P302 | UART TX |
| D2 | P105 | GPIO / 割り込み |
| D3 | P104 | GPIO / PWM (GTIOC1B) / 割り込み |
| D4 | P103 | GPIO (CAN TX) |
| D5 | P102 | GPIO / PWM (GTIOC2B) (CAN RX) |
| D6 | P106 | GPIO / PWM (GTIOC0B) |
| D7 | P107 | GPIO |
| **D8** | **P304** | **GPIO (ステッパー相 1)** |
| **D9** | **P303** | **GPIO / PWM (ステッパー相 2)** |
| **D10** | **P112** | **GPIO / PWM (ステッパー相 3)** |
| **D11** | **P109** | **GPIO / PWM (ステッパー相 4)** |
| D12 | P110 | GPIO (SPI CIPO) |
| D13 | P111 | GPIO / SPI SCK / LED_BUILTIN |
| A0 | P014 | アナログ / DAC |
| A1 – A3 | P000 – P002 | アナログ / OPAMP |
| A4 / D18 | P101 | I2C SDA |
| A5 / D19 | P100 | I2C SCL |

**ステッピングモーター用に D8〜D11 を割り当てると [[stepper-library|Stepper ライブラリ]]
のサンプルと同じピン番号になり流用できる**。

## PWM ピン (analogWrite 用)

`analogWrite()` 可能なのは **D3, D5, D6, D9, D10, D11** の 6 本。
タイマー (GPT の GTIOC チャネル) は user-manual.md の表を参照。
本プロジェクトでは PWM は使わない (モーター制御は digital on/off のみ)。

## DAC

A0 (D14) に 12-bit DAC あり。`analogWrite(A0, value)` で 0-4095 を
analog 電圧 (0–5 V) に出せる。本プロジェクトでは未使用。

## 電源ツリー

`ABX00080-schematics.pdf` と user-manual.md より。

```
VIN (barrel) ─┐
              ├─→ Schottky (D2 PMEG6020AELRX) ─→ ISL854102FRZ (降圧) ─→ +5V rail
USB-C VBUS ───┘                                                            │
                                                                           ├→ +5V pin (header)
                                                                           ├→ RA4M1 VCC
                                                                           └→ VCC_USB (3.3 V) → 3V3 pin
```

- **VIN 給電**: 6–24 V → Schottky → buck → 5 V。1.2 A 程度まで。
- **USB 給電**: VBUS → Schottky → buck → 5 V。ただし Schottky drop で実測 ~4.7 V。
- **3.3V ピン**: VCC_USB ピンから直接給電される (降圧コンバータ経由ではない)。

## I2C プルアップ

PCB には実装されていないが、**フットプリントはある**。
A4/A5 を I2C として使う場合は **外付けでプルアップ抵抗を追加**する。

## 推奨されない設計

- **5V ピンから大電流 (サーボなど) を引っ張らない** (user-manual.md)。
  ステッピングモーター用 [[uln2003]] 駆動電源もここに繋ぐと理論上は動くが、
  MCU と同じ電源なのでノイズ・電圧降下の観点で望ましくない。
- **GPIO から直接コイルを駆動しない** (8 mA 制限を超える)。
- **モーター駆動用に +5V ピンを過信しない**。外部 5 V 電源を用意するのが定石。

## ボード操作

- **ボード識別**: SKU ABX00080、Arduino IDE の boards manager で
  "Arduino UNO R4 Boards" をインストール。
- **シリアル出力**: USB-C で `Serial` オブジェクトが直接使える。
- **ボード修復**: スケッチが USB をブロックしたら、電源投入後すぐに
  リセットボタンをダブルタップしてブートローダーモードに入る。

## 関連ページ

- [[arduino-uno-r4-wifi]] — **本プロジェクトで採用する後継ボード** (WiFi + API server 化)
- [[ra4m1]] — 搭載 MCU の詳細 (Minima / WiFi で共通)
- [[stepper-motor]] — 駆動対象モーターの概念
- [[28byj-48]] — 本プロジェクトで使うモーター
- [[uln2003]] — 駆動用ドライバ IC
- [[stepper-library]] — Arduino 公式ライブラリ (4-wire 制御)
- [[tutorials/unipolar-stepper-motor]] — 5線式ユニポーラのチュートリアル
- [[tutorials/stepper-library-examples]] — Stepper.h サンプル集
