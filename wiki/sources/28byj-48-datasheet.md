---
title: 28BYJ-48 データシート
type: source
tags: [datasheet, stepper, 28byj-48]
sources:
  - raw/28byj-48/step-motor-5v-28byj48-datasheet.pdf
created: 2026-08-10
updated: 2026-08-10
---

# 28BYJ-48 データシート

メーカー: **Kiatronics (Welten Holdings Ltd, New Zealand)**
出典 (SparkFun ミラー):
<https://cdn.sparkfun.com/assets/8/e/0/8/e/step-motor-5v-28byj48-datasheet.pdf>
(元データは Mouser にあるが、アクセス制限のため SparkFun CDN を採用 — raw/SOURCES.md のメモ参照)

> 当 raw の PDF は 1 ページのみ。テキスト+簡単な図のみだが、
> モーター駆動に必要な情報は網羅されている。

## 内容

データシートは以下のセクションから成る (テキスト抽出より):

1. **タイトル**: "28BYJ-48 – 5V Stepper Motor"
2. **説明文**: "The 28BYJ-48 is a small stepper motor suitable for a large range of applications."
3. **スペック表** (下記)
4. **内部接続図** (5 線 pin 配置、コイル巻き線模式図)
5. **外形寸法図** (直径 28 mm、奥行き 19 mm、軸径 5 mm 等)

## スペック表 (原文ママ)

| 項目 | 値 |
| --- | --- |
| Rated voltage | 5 VDC |
| Number of Phase | 4 |
| Speed Variation Ratio | 1/64 |
| Stride Angle | 5.625° /64 |
| Frequency | 100 Hz |
| DC resistance | 50 Ω ±7% (25 °C) |
| Idle In-traction Frequency | > 600 Hz |
| Idle Out-traction Frequency | > 1000 Hz |
| In-traction Torque | > 34.3 mN·m (120 Hz) |
| Self-positioning Torque | > 34.3 mN·m |
| Friction torque | 600 – 1200 gf·cm |
| Pull in torque | 300 gf·cm |
| Insulated resistance | > 10 MΩ (500 V) |
| Insulated electricity power | 600 VAC / 1 mA / 1 s |
| Insulation grade | A |
| Rise in Temperature | < 40 K (120 Hz) |
| Noise | < 35 dB (120 Hz, No load, 10 cm) |
| Model | 28BYJ-48 – 5V |

## "5.625° /64" の読み方

データシートの "Stride Angle: 5.625°/64" は、

- 内部ロータのステップ角: **5.625°/step** (= 360°/64 ステップ)
- ギア減速比: **1/64**

を合わせた結果と読める。

> "Stride Angle 5.625°/64" は (5.625°) / 64 と解釈するのが妥当。
> つまり出力軸 1 ステップは 5.625/64 ≈ **0.08789 °**、
> 1 回転は **4096 ステップ**。

## 5 線コネクタの色と機能

データシート内部接続図より:

```
        ┌──── 1: Blue   ──┐
        │                  │
   ┌────┤    Coil pair    ├─── 5: Red (COM = +5V)
   │    │                  │
   │    └──── 2: Pink  ──┐ │
   │                    │ │
   └──── 4: Orange ─────┘ │
        │                │
        └──── 3: Yellow ──┘
```

| Pin | Wire color | 機能 |
| --- | --- | --- |
| 1 | Blue | Phase A (coil pair 1 端) |
| 2 | Pink | Phase B (coil pair 2 端) |
| 3 | Yellow | Phase C (coil pair 2 端) |
| 4 | Orange | Phase D (coil pair 1 端) |
| 5 | Red | **COM (共通 +5V)** |

## 外形寸法 (図より)

| 項目 | 寸法 (mm) |
| --- | --- |
| 直径 (φ) | 28 |
| 厚み | 19 |
| 軸径 | 5 |
| 軸長 (本体より) | 約 10 |
| 取付穴 2 個 | 2-φ 4.2、中心距離 35 ± 0.2、R3.5 |
| 取付ボス高さ | 7 |
| ケーブル | AWG26 UL 1061X5, PVC |
| コネクタ | JST XHP-5 (SXH-001T-P0.6) |

## データの信頼性

このデータシートは 2012 年発行 (CreationDate: 2012-07-09, 改訂 2014-02-26) と古いが、
主要スペック (5 V, 4 相, 1/64, 50 Ω, 5.625° step) はメーカー公式で広く参照されており
信頼性は高い。

ただし **タイミング・騒音・トルク** などの項目は typical 値で、
個体差・温度・駆動シーケンスにより変化する。

## 関連ページ

- [[28byj-48]] — 派生した概念ページ
- [[stepper-motor]] — ステッピングモーター一般
- [[uln2003]] — 駆動 IC
- [[unipolar-vs-bipolar]]
