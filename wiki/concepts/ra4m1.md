---
title: RA4M1 (R7FA4M1AB3CFM)
type: concept
tags: [mcu, renesas, cortex-m4]
sources:
  - raw/arduino-uno-r4-minima/ra4m1-datasheet.pdf
  - raw/arduino-uno-r4-wifi/ra4m1-datasheet.pdf
  - raw/arduino-uno-r4-wifi/ABX00087-full-pinout.pdf
created: 2026-08-10
updated: 2026-08-10
---

# RA4M1 (R7FA4M1AB3CFM)

[[arduino-uno-r4-minima|UNO R4 Minima]] および
[[arduino-uno-r4-wifi|UNO R4 WiFi]] に搭載されている **Renesas RA4M1 シリーズ** マイコン。
品番は **R7FA4M1AB3CFM#AA0** (LQFP64, 産業用温度範囲)。

> このページはステッピングモーター制御に必要な範囲のみ抜粋する。
> 詳細は raw の `ra4m1-datasheet.pdf` (130ページ) を参照。

## クイック仕様

| 項目 | 値 |
| --- | --- |
| コア | Arm Cortex-M4 (FPU 付き) |
| 最高クロック | 48 MHz |
| フラッシュ / SRAM / データフラッシュ | 256 kB / 32 kB / 8 kB |
| 動作電圧 | 1.6 V – 5.5 V (UNO R4 Minima では **5 V** で使用) |
| GPIO 電流 | 最大 **8 mA / pin**, **全体 60 mA** |

## Arduino との対応

UNO R4 系の Arduino ピン (D0–D13, A0–A5) は RA4M1 の **Pxxx 端子** にマップされる。
**Minima と WiFi で一部 Arduino ピンの対応する RA4M1 端子が異なる** ため注意
(同じ R7FA4M1AB3CFM チップだが、ボード設計者が Arduino ピンに割り当てる RA4M1
端子を選んでいるため)。

ステッピングモーター制御で使う D8〜D11:

| Arduino | Minima (ABX00080) | WiFi (ABX00087) |
| --- | --- | --- |
| D8 | P304 | P304 |
| D9 | P303 | P303 |
| D10 | P112 | **P103** |
| D11 | P109 | **P411** |

→ **本プロジェクト (WiFi) では D10 = P103, D11 = P411**。
D8 / D9 は両 variant で共通。

その他の Arduino ピンの対応は [[arduino-uno-r4-wifi#ピン配置 digital/analog headers]]
(WiFi) / [[arduino-uno-r4-minima]] (Minima) を参照。

## 周辺機能 (このプロジェクトで関係する範囲)

- **GPIO**: 5 V tolerant。HIGH = 5 V / LOW = 0 V。
- **タイマー (GPT)**: PWM 用。ステッピングモーター制御では使わない。
- **ADC**: 14-bit。A0–A5 で利用可能。
- **DAC**: 12-bit。**A0 のみ**。
- **I2C / SPI / UART / CAN**: 各 1ch。I2C は A4/A5 (=D18/D19) 共有。

## 5V 動作の注意点

RA4M1 は本来 1.6–5.5 V で動作する低消費電力マイコンだが、
UNO R4 Minima では **5 V 系** で使われている。これは旧 UNO 互換のため。
GPIO 出力 HIGH は約 5 V が出るため、3.3 V デバイスを直接駆動する場合は
レベルシフタが必要。

## 関連ページ

- [[arduino-uno-r4-minima]] — RA4M1 搭載ボード (本プロジェクトでは不採用)
- [[arduino-uno-r4-wifi]] — RA4M1 + ESP32-S3 搭載ボード (**本プロジェクトで使用**)
- [[stepper-motor]]
