---
title: ESP32-S3-MINI-1 / MINI-1U Datasheet
type: source
tags: [esp32, datasheet, wifi, bluetooth]
sources:
  - raw/arduino-uno-r4-wifi/esp32-s3-mini-1_mini-1u_datasheet_en.pdf
created: 2026-08-10
updated: 2026-08-10
---

# ESP32-S3-MINI-1 / MINI-1U Datasheet

Espressif Systems 製の ESP32-S3 モジュール datasheet。53 ページ。
UNO R4 WiFi には "MINI-1-N8" variant (内蔵フラッシュ 8 MB) が搭載されている。
(本 datasheet は MINI-1 と MINI-1U の両 variant をカバー。)

## 構成 (推定、PDF 未詳細読解)

- 1. Overview — Xtensa LX7 デュアルコア, 240 MHz, Wi-Fi 4, BT 5
- 2. Pin Definition — MINI-1 / MINI-1U 別
- 3. Electrical Characteristics
- 4. RF Performance
- 5. Schematics
- 6. Peripheral Schematics
- 7. Physical Dimensions
- 8. Recommended PCB Land Pattern
- ...

本プロジェクトでは概念レベルの参照のみ。詳細タイミング・RF 性能は通常不要。

## 本プロジェクトで参照する情報

- 動作電圧 3.3 V
- Wi-Fi 2.4 GHz, 最大 150 Mbps
- Bluetooth 5 LE
- 内蔵 trace antenna

## 関連ページ

- [[esp32-s3-mini-1]] — concept ページ (詳細まとめ済み)
- [[arduino-uno-r4-wifi]] — 本モジュール搭載ボード
