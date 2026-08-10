---
title: MB102 Power Supply Datasheet (Handson Technology)
type: source
tags: [mb102, datasheet, power, regulator, addicore, ywrobot]
sources:
  - raw/mb102/mb102-ps.pdf
created: 2026-08-10
updated: 2026-08-10
---

# MB102 Power Supply Datasheet (Handson Technology)

Handson Technology による MB102 互換電源モジュールの datasheet。
回路図 + Block diagram を含む 4 ページ。
"A1117-5" / "A1117-3.3" という表記 ([[ams1117|AMS1117]] と同等品) が使われている。
基板のシルクは "YwRobot Power MB V2" で、Addicore が OEM 提供している variant。

## 主要スペック

| 項目 | 値 |
| --- | --- |
| 入力電圧 | 6.5 – 12 V DC **または 5 V USB** |
| 出力電圧 | 3.3 V / 5 V (ジャンパ切替) |
| 最大出力電流 | **< 700 mA** (この datasheet の表記) |
| 寸法 | 5.3 cm × 3.5 cm |
| 互換性 | MB102 規格ブレッドボード |

> 📌 7-pin ヘッダ J6 (上段ジャンパ) / 4-pin ヘッダ J1, J3 / 出力 J2, J4, J5 など、
> 物理ピン配置は AZ-Delivery 版と若干異なるが、機能・回路は同等。

## 回路 (Block diagram より)

```
   DC IN (barrel) ─► SW1 ─► 1N4007 ─► A1117-5  ─► 5V rail
                                      A1117-3.3 ─► 3.3V rail

   出力先: ブレッドボード電源レール (両側独立)
          + 外部出力ヘッダ (3.3V/5V/GND)
          + USB-A (5V, 出力のみ)
```

## AZ-Delivery 版との差分

| 項目 | AZ-Delivery 版 | Handson 版 |
| --- | --- | --- |
| 最大出力電流 (記載値) | 300 mA | < 700 mA |
| USB からの入力 | 不可 | 可能 (5V) |
| ジャンパ形式 | 4 ピン 2 系統 | 4 ピン 2 系統 + 8 ピン J6 |
| 基板シルク | "MB102" | "YwRobot Power MB V2" |

ハード的には同等 (AMS1117 ベース) と考えてよい。
本プロジェクトでは AZ-Delivery 版 ([mb102] で参照) を採用。

## 関連ページ

- [[mb102]] — concept ページ (AZ-Delivery 版基準)
- [[sources/mb102-datasheet]] — AZ-Delivery 版 datasheet
- [[ams1117]] — 搭載 LDO 詳細
