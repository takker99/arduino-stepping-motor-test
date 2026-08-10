---
title: Arduino UNO R4 WiFi Datasheet (ABX00087)
type: source
tags: [arduino, datasheet, abx00087, wifi]
sources:
  - raw/arduino-uno-r4-wifi/datasheet.md
  - raw/arduino-uno-r4-wifi/ABX00087-datasheet.pdf
  - raw/arduino-uno-r4-wifi/user-manual.md
  - raw/arduino-uno-r4-wifi/ABX00087-full-pinout.pdf
  - raw/arduino-uno-r4-wifi/ABX00087-schematics.pdf
created: 2026-08-10
updated: 2026-08-10
---

# Arduino UNO R4 WiFi Datasheet (ABX00087)

Arduino 公式の UNO R4 WiFi データシート。
PDF 全 46 ページと、それと等価な内容の markdown 版 (datasheet.md) が raw/ にある。
markdown 版は PDF のテキストをほぼそのまま起こしたもので、リンク・図参照 (`assets/...`) は
元 PDF を参照する必要がある。

## 構成

1. Description / Target areas (Maker, beginner, education)
2. **Features** — RA4M1 + ESP32-S3-MINI-1-N8 のスペック
3. Recommended Operating Conditions — VIN 6-24V, VUSB 4.8-5.5V
4. Functional Overview — Block diagram, board topology
5. Microcontroller (RA4M1) — 5 V, 256 kB flash, 32 kB SRAM
6. Wi-Fi / BT Module (ESP32-S3) — 3.3 V, 802.11 b/g/n, BT 5 LE
7. ESP Header (上級者向け)
8. USB Bridge (デフォルト ESP32-S3 経由)
9. USB Connector (USB-C)
10. LED Matrix (12x8, charlieplexing)
11. DAC (A0)
12. Qwiic I2C Connector (Wire1, 3.3 V)
13. **Power Options** — VIN (ISL854102 buck) / USB-C (5V direct)
14. Power Tree diagram
15. **Pinout** — Analog / Digital / OFF / ICSP
16. Mounting Holes & Board Outline
17. Board Operation / Getting Started
18. Certifications (CE, FCC, IC, SRRC)

## 本プロジェクトで特に参照するページ

| 章 | 参照理由 |
| --- | --- |
| Features | MCU / コプロセッサ / メモリ仕様 |
| Microcontroller | RA4M1 の周辺機能 (DAC, OPAMP, RTC, ADC 14-bit) |
| Wi-Fi / BT Module | アンテナ共有、3.3 V 系の制約 |
| USB Bridge | ESP32-S3 経由のデフォルト経路、D21 での切替方法 |
| Power Options | VIN 6-24V → 5V 1.2A の電力バジェット |
| Power Tree | VIN/USB → 5V → 3.3V の系統図 |
| Pinout | D0-D13, A0-A5 の対応表 (本プロジェクトは D8-D11 を使用) |

## 図面系 PDF (別途参照)

- `ABX00087-full-pinout.pdf` (4 ページ) — フルピンアウト図 (Pxxx 端子との対応)
- `ABX00087-schematics.pdf` (3 ページ) — 回路図
- `esp32-s3-mini-1_mini-1u_datasheet_en.pdf` (53 ページ) — コプロセッサ datasheet
- `ra4m1-datasheet.pdf` (130 ページ) — メイン MCU datasheet

## 関連ページ

- [[arduino-uno-r4-wifi]] — 本ページの subject の concept ページ
- [[ra4m1]] — メイン MCU の concept ページ
- [[esp32-s3-mini-1]] — コプロセッサの concept ページ
