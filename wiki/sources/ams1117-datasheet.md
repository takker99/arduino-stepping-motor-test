---
title: AMS1117 Datasheet (UMW)
type: source
tags: [ldo, regulator, datasheet, ams1117]
sources:
  - raw/mb102/ams1117-ds1117.pdf
created: 2026-08-10
updated: 2026-08-10
---

# AMS1117 Datasheet (UMW)

友台半导体 (UMW) 製の AMS1117 datasheet。492 行のテキスト抽出済み。
固定出力版 (1.2 / 1.5 / 1.8 / 2.5 / 3.3 / 5.0 V) と ADJ (可変) 版がある。
[[mb102|MB102]] には 5.0 V 版 (AMS1117-5.0) と 3.3 V 版 (AMS1117-3.3) が使われている。
(MB102 schematic では "AM1117-5" / "AM1117-3.3" と表記されているが、同一品の互換。)

## 構成

- Description / Features (1A LDO, 1.2V dropout @ 1A)
- Pin Configuration & Nomenclature
- Block Diagram (BandGap + TSD + Current Limit)
- Absolute Maximum Ratings
- Electrical Characteristics (Line/Load Regulation, Dropout)
- Typical Performance Characteristics
- Application Notes
- Package Information (SOT-223, TO-252, SOT-89)

## 主要スペック

| 項目 | 値 |
| --- | --- |
| 出力電圧 (固定版) | 1.2 / 1.5 / 1.8 / 2.5 / 3.3 / 5.0 V (±1.5%) |
| 最大出力電流 | **1 A** |
| ドロップアウト | typ **1.2 V @ 1 A** / max 1.25 V @ 500 mA |
| 入力電圧 (max) | **18 V** |
| 静止電流 | typ 5 mA, max 10 mA |
| 動作温度 | -40 ～ +125 ℃ |
| 保護 | 過電流制限, 熱遮断 (TSD) |
| Thermal Resistance (SOT-223) | Rja ≈ **88 ℃/W** |

## 関連ページ

- [[ams1117]] — concept ページ (本プロジェクトでの熱設計含む)
- [[mb102]] — AMS1117 を 2 個搭載する電源モジュール
