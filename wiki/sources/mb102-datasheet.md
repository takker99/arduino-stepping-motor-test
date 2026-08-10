---
title: MB102 Netzteil Adapter Datasheet (AZ-Delivery)
type: source
tags: [mb102, datasheet, power, regulator]
sources:
  - raw/mb102/MB102_Netzteil_Adapter_Datenblatt_AZ-Delivery.pdf
  - raw/mb102/MB102_Netzteil_Adapter_Schematic_AZ-Delivery.pdf
created: 2026-08-10
updated: 2026-08-10
---

# MB102 Netzteil Adapter Datasheet (AZ-Delivery)

AZ-Delivery 版の MB102 datasheet。本文 4 ページ + schematic 1 ページ (別 PDF)。
ドイツ語。MB102 モジュール本体のみの簡易仕様で、LDO は AMS1117 相当品 (AM1117)。

## 主要スペック

| 項目 | 値 |
| --- | --- |
| 入力電圧 | **6.5 – 12 V DC** (5.5 × 2.1 mm barrel) |
| 出力電圧 | **3.3 V / 5 V** (各レール独立にジャンパ選択) |
| 最大出力電流 | **300 mA** (この datasheet の表記) |
| スイッチ | ON/OFF |
| インジケータ | LED |
| 出力先 | ブレッドボード電源レール + 外部出力ヘッダピン + USB-A (出力専用) |

> 📌 注意: 同じ MB102 ハードウェアでも [[sources/mb102-ps-datasheet|Handson Technology 版 datasheet]]
> では最大出力 700 mA と記載されている。datasheet によって数値が異なるが、
> 同一ハード。本プロジェクトでは安全マージンを取って 500 mA 未満で使う。

## ジャンパ設定 (datasheet 図より)

各レール (上段/下段) 独立に `OFF` / `3.3V` / `5V` を選択可能。
両方のジャンパが `OFF` のときは LED もレールも通電されない。

## Schematic (別 PDF)

- DC IN → SW1 → 1N4007 → AM1117-5 → 5V rail
- AM1117-5 → AM1117-3.3 → 3.3V rail
- 100 µF/16V バルク + 10 nF 入出力コンデンサ
- VD1 LED + 470 Ω
- USB-A は 5V 出力のみ

## 関連ページ

- [[mb102]] — concept ページ (詳細まとめ)
- [[ams1117]] — 搭載 LDO の詳細
- [[sources/mb102-ps-datasheet]] — Handson Technology 版 datasheet
