---
title: ULN2003A データシート (SLRS027)
type: source
tags: [datasheet, uln2003, darlington, ti]
sources:
  - raw/uln2003/uln2003a-slrs027o-ti.pdf
created: 2026-08-10
updated: 2026-08-10
---

# ULN2003A データシート (SLRS027)

Texas Instruments 公式データシート。
<https://www.ti.com/lit/ds/symlink/uln2003a.pdf>

リビジョン: **SLRS027T** (March 2025)、原初 December 1976。
当 raw は Rev T 取得時点のファイル (PDF 41 ページ)。

> 旧リビジョン (SLRS027O 以前) では Family 名の表記やピン配置が微妙に異なる場合がある。
> 最新版を参照すること。

## ファミリー

ULN2003A は次のシリーズの 1 つで、すべて **7 ch NPN ダーリントン・アレイ**:

| 型番 | シリーズベース抵抗 (R_B) | 用途 |
| --- | --- | --- |
| ULN2002A | 7 V ツェナー + 直列抵抗 | 14–25 V PMOS 駆動用 |
| **ULN2003A** | **2.7 kΩ** | **TTL / 5V CMOS 直接駆動 (本プロジェクトで使用)** |
| ULN2003AI | 2.7 kΩ | 産業用温度範囲版 |
| ULQ2003A | 2.7 kΩ | 自動車用温度範囲版 |
| ULN2004A | 10.5 kΩ | 6–15 V CMOS 直接駆動 |
| ULQ2004A | 10.5 kΩ | 自動車用温度範囲版 |

> ULN2003A = TTL/5V CMOS 直接駆動用の標準品。

## パッケージ

| 型番 | パッケージ | サイズ |
| --- | --- | --- |
| ULN200xAD | SOIC-16 | 9.90 × 3.91 mm |
| ULN200xAN | PDIP-16 | 19.30 × 6.35 mm |
| ULN200xANS | SOP-16 | 10.30 × 5.30 mm |
| ULN200xAPW | TSSOP-16 | 5.00 × 4.40 mm |
| ULN2003ADYY | SOT-16 | 4.20 × 2.00 mm |

> 28BYJ-48 + ULN2003 driver board に同梱されているのは通常 PDIP (ULN2003AN)。
> SparkFun / Kiatronics モジュールでは PDIP 実装品。

## 主要スペック (SLRS027T §5.1 / §5.6 / §5.10)

### 絶対最大定格 (§5.1)

- 出力耐圧: **50 V**
- 1 チャネル出力電流: **500 mA**
- 入力電圧 (順方向): 30 V
- 動作温度: **-40 °C 〜 +105 °C**

### ULN2003A 電気的特性 (§5.6, T_A=25°C)

| 記号 | パラメータ | 条件 | Min | Typ | Max | Unit |
| --- | --- | --- | --- | --- | --- | --- |
| V_I(on) | ON 入力電圧 | V_CE = 2V, I_C = 300 mA | – | – | **2.4** | V |
| V_I(off) | OFF 入力電圧 | I_C = 500 µA, T_A = 85°C | **0.5** | – | – | V |
| I_I(on) | ON 入力電流 | V_I = 3.85 V | – | – | 1.35 | mA |
| V_CE(SAT) | 出力飽和電圧 | I_I = 250 µA, I_C = 100 mA | – | 0.9 | **1.1** | V |
| I_CEX | 出力漏れ電流 | V_CE = 50 V | – | – | 50 | µA |
| h_FE | DC 電流利得 | V_CE = 2V, I_C = 350 mA | 1000 | – | – | – |

### スイッチング特性 (§5.10)

- t_PLH (Low→High 遅延): typ 0.25 µs (条件依存)
- t_PHL (High→Low 遅延): typ 0.25 µs
- → 数十 kHz 〜 100 kHz のステップパルスに十分追従可能

## 等価回路 (1 チャネル, §7.2 / Figure 7-2)

```
Input B ──[2.7 kΩ]──┬──[NPN Q1]──┐
                    │              ├── Output C
                    │   ┌──[7.2 kΩ]──┤
                    │   │             │
                    └───┤             ├──[3 kΩ]── E (GND)
                        │  [NPN Q2]   │
                        └──────[base]─┘

  Output C ──|>── COM (フライバックダイオード)
```

## ブロックダイアグラム (Pin Configuration, §4)

16 ピン SOIC/PDIP/SO/TSSOP (Top View):

```
  1B  1 ── 16  1C
  2B  2     15  2C
  3B  3     14  3C
  4B  4     13  4C
  5B  5     12  5C
  6B  6     11  6C
  7B  7     10  7C
   E  8  ──  9  COM
```

> 入力と出力でピン番号が **逆順**。1B は pin 1、1C は pin 16。
> 角 (pin 1 マーク) から遠い側が COM と E。

## 代表的アプリケーション (§8 Application and Implementation)

データシート §8.3 "System Examples" に次の例がある:

- **Figure 8-4**: P-MOS to Load (ULN2002A)
- **Figure 8-5**: **TTL to Load (ULN2003A / ULQ2003A)** — 本プロジェクトで使用する構成
- **Figure 8-6**: Buffer for Higher Current Loads (ULN2004A)
- **Figure 8-7**: Use of Pullup Resistors to Increase Drive Current (ULN2003A)

§8.2.2 駆動設計式:

```
I_COIL = (V_SUP − V_CE(SAT)) / R_COIL
P_D    = Σ(V_OLi × I_Li)   (i = 1..N)
P_D(MAX) = (T_J(MAX) − T_A) / θ_JA
```

> 28BYJ-48 (50 Ω, 5V) の場合:
> `I_COIL ≈ (5 − 1) / 50 ≈ 80 mA` (V_CE(SAT) ≈ 1 V と仮定)
> `P_D ≈ 4 ch × 80 mA × 1 V = 320 mW` → 自然空冷で問題なし。

## Power Supply Recommendations (§8.4)

> "The COM pin is typically tied to the system power supply. When this is the case,
> it is very important to ensure that the output voltage does not heavily exceed the
> COM pin voltage. This discrepancy heavily forward biases the fly-back diodes and
> causes a large current to flow into COM, potentially damaging the on-chip metal or
> over-heating the device."

つまり **COM ピン = 負荷電源電圧** に必ず合わせること。
本プロジェクトでは 28BYJ-48 の COM (Red) は +5V なので、ULN2003 の COM ピンも
+5V に繋ぐ。

## Layout Guidelines (§8.5)

- 入力トレース: 低電流なので細くて良い
- 出力トレース: 大電流が流れるので太くする
- 入力チャネル間はクロストーク回避のため分離

## 関連ページ

- [[uln2003]] — 派生した概念ページ
- [[28byj-48]] — 駆動対象モーター
- [[unipolar-vs-bipolar]]
