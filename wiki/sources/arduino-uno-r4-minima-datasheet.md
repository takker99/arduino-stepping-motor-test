---
title: Arduino UNO R4 Minima データシート (ABX00080)
type: source
tags: [datasheet, arduino, r4, minima, abx00080]
sources:
  - raw/arduino-uno-r4-minima/ABX00080-datasheet.pdf
  - raw/arduino-uno-r4-minima/datasheet.md
created: 2026-08-10
updated: 2026-08-10
---

# Arduino UNO R4 Minima データシート

公式 PDF: <https://docs.arduino.cc/resources/datasheets/ABX00080-datasheet.pdf>
GitHub 上の元 markdown: <https://github.com/arduino/docs-content/blob/main/content/hardware/02.uno/boards/uno-r4-minima/datasheet/datasheet.md>

最終リビジョン: **Rev 5 (2025-10-29, Mechanical drawing update)** / 当 raw は
Rev 5 取得時点のファイル。

## ファイル構成

| ファイル | 用途 |
| --- | --- |
| `ABX00080-datasheet.pdf` | 公式 PDF (36 ページ) |
| `ABX00080-full-pinout.pdf` | ピンアウト図 (4 ページ) |
| `ABX00080-schematics.pdf` | 回路図 (1 ページ + ライセンス 1 ページ) |
| `datasheet.md` | 公式 PDF の markdown 版 (GitHub docs-content) |
| `user-manual.md` | User Manual / cheat sheet (GitHub docs-content) |
| `ra4m1-datasheet.pdf` | 搭載 MCU の Renesas 公式データシート (130 ページ) |

## 内容のまとめ

### 主要スペック (Rev 5)

- **MCU**: [[ra4m1]] (R7FA4M1AB3CFM#AA0)
  - 48 MHz Arm Cortex-M4 + FPU
  - 256 kB Flash / 32 kB SRAM / 8 kB Data Flash (EEPROM)
  - 5 V 動作
- **ピン**:
  - 14× digital (D0–D13)
  - 6× analog (A0–A5) — A0 は DAC, A1–A3 は OPAMP
  - 6× PWM: D3, D5, D6, D9, D10, D11
- **周辺**: USB 2.0 FS, RTC, MPU, 14-bit ADC, 12-bit DAC, OPAMP, CAN
- **電源**:
  - VIN 6 – 24 V (typ 7 V)
  - USB-C 5 V
  - Schottky ダイオードで逆極性・過電圧保護
- **通信**: UART × 1 (D0/D1), SPI × 1 (D10–D13), I2C × 1 (A4/A5),
  CAN × 1 (D4/D5, 外部トランシーバ必要)

### 推奨動作条件 (Rev 5 datasheet §2)

| Symbol | 説明 | Min | Typ | Max | Unit |
| --- | --- | --- | --- | --- | --- |
| V_IN | VIN / DC Jack 入力電圧 | 6 | 7.0 | 24 | V |
| V_USB | USB 入力電圧 | 4.8 | 5.0 | 5.5 | V |
| T_OP | 動作温度 | -40 | 25 | 85 | °C |

### 重要: GPIO 電流制約

> "The GPIOs on the R7FA4M1AB3CFM#AA0 microcontroller can handle up to **8 mA**.
> Never connect devices that draw higher current directly to a GPIO."
> — datasheet.md §9.3

これはステッピングモーター駆動に直結する重要情報。[[28byj-48]] の 1 相は
50 Ω / 5 V で 100 mA 必要なので、直接 GPIO から駆動できない。
**[[uln2003]] などの駆動 IC が必須**。

### 5V ピンの供給能力 (user-manual.md より)

| 給電元 | 5V ピンが提供できる電流 |
| --- | --- |
| VIN (barrel / DC jack) | 最大 **1.2 A** |
| USB-C | 最大 **2 A** (理論値、ボードの保護回路による) |

ただし、サーボなど大電流アクチュエータを 5V ピンから引っ張ることは推奨されない
(user-manual.md 警告)。ステッピングモーターも同様に、できれば **外部 5V 電源** を
用意して GND のみ共通にするのが定石。

### ピン配置 (公式 pinout 4 ページより)

- p.1: 標準ピン配置図 (legend 付き)
- p.2: USB-C, SPI (3 ピン ICSP), SWD (10 ピン) の位置
- p.3: WARNING (Advanced Section)
- p.4: Advanced — 全ピンの RA4M1 ペリフェラル機能マップ
  (UART, SPI, I2C, CAN, IRQ, ADC, CMP, OPAMP, Timer, Other)

### 回路図 (ABX00080-schematics.pdf)

主なブロック:
- USB-C コネクタ (J3, CX90B-16P)
- ESD 保護 D4 (PRTR5V0U2X,215)
- 電源経路: VIN → Schottky D2 (PMEG6020AELRX) → Buck U2 (ISL854102FRZ-T) → +5V
  (L3 = 10 µH 2.3A PMF42-103MN, 出力容量 C12–C18)
- MCU U1 (R7FA4M1AB3CFM#AA0)
- 16 MHz 水晶 Y1 (PTX16.000M125M3S3030)
- AREF / +3V3 フィルタ: L1, L2 (BLM18PG471SN1D)
- LED: DL1 (TX, P012), DL2 (RX, P013), DL3 (LED_BUILTIN, P111), DL4 (+5V Power)
- ICSP (J1, 6 ピン) と SWD (J2, 10 ピン)

## Rev. 5 までの変更履歴 (datasheet.md §Change Log より)

| Date | Rev | Changes |
| --- | --- | --- |
| 2025-10-29 | 5 | Mechanical drawing update |
| 2024-04-25 | 4 | Updated link to new Cloud Editor |
| 2024-03-28 | 3 | Update Rated Current |
| 2023-07-25 | 2 | Update Pin Table |
| 2023-06-19 | 1 | First Release |

## 関連ページ

- [[arduino-uno-r4-minima]] — 派生した概念ページ
- [[ra4m1]] — 搭載 MCU
- [[28byj-48]] — 駆動対象モーター
- [[uln2003]] — 駆動 IC
