---
title: ステッピングモーター
type: concept
tags: [stepper, motor, hardware]
sources:
  - raw/28byj-48/step-motor-5v-28byj48-datasheet.pdf
  - raw/arduino/stepper-library-examples.md
  - raw/arduino/unipolar-stepper-motor-tutorial.md
created: 2026-08-10
updated: 2026-08-10
---

# ステッピングモーター (Stepper motor)

**パルス列で角度制御できるモーター**。DC モーターと異なり、
位置フィードバック (エンコーダ等) なしで正確な角度制御ができる。
代价として、駆動回路が複雑になる。

## 原理 (超要約)

ステッピングモーターのシャフトは **複数の磁極 (マグネット)** と **複数の
ステータコイル** からなる。コイルに **決まった順序** で電流を流すと、
シャフトが **1 ステップずつ回転** する。

1 ステップあたりの角度を **ステップ角** という。
内部の磁極数 × 駆動シーケンス (励磁パターン) で決まる。
出力軸では **ギア減速比** によってさらに細かくなる (例: 28BYJ-48 は 1/64)。

## unipolar / bipolar

ステッピングモーターはコイルの巻き方・駆動方式で 2 種類に大別される。
詳しくは [[unipolar-vs-bipolar]] を参照。

| 種類 | コイル端 | 駆動 | 例 |
| --- | --- | --- | --- |
| unipolar | センタータップ付き (5/6 線) | 各相を片側から GND に流す | 28BYJ-48, 旧フロッピーのモーター |
| bipolar | センタータップなし (4 線) | H ブリッジで両方向から電流を流す | NEMA ステッパー, 多くの産業用モーター |

本プロジェクトの [[28byj-48]] は **unipolar 5 線式**。

## 励磁シーケンス

### 1 相励磁 (wave drive)

| Step | C0 | C1 | C2 | C3 |
| --- | --- | --- | --- | --- |
| 0 | 1 | 0 | 0 | 0 |
| 1 | 0 | 1 | 0 | 0 |
| 2 | 0 | 0 | 1 | 0 |
| 3 | 0 | 0 | 0 | 1 |

### 2 相励磁 (full-step)

| Step | C0 | C1 | C2 | C3 |
| --- | --- | --- | --- | --- |
| 0 | 1 | 0 | 1 | 0 |
| 1 | 0 | 1 | 1 | 0 |
| 2 | 0 | 1 | 0 | 1 |
| 3 | 1 | 0 | 0 | 1 |

### half-step (1相 + 2相 交互)

両者を交互に繰り返す。ステップ角を半分にでき、振動も少ない。

[[stepper-library]] の 4 ピン実装は **2 相励磁** シーケンスを採用している
(`Stepper.cpp` の `stepMotor()` switch table)。

## 駆動回路の必要性

ステッピングモーターのコイルは **数十 mA 〜 数 A** の電流を必要とする。
Arduino などのマイコン GPIO は 8 mA 程度しか流せないため、**必ず駆動回路** を
介す:

- **unipolar 5/6 線**: ダーリントンアレイ ([[uln2003]])
- **bipolar 4 線**: H ブリッジ (L293, SN754410, DRV8833 など)

## 関連ページ

- [[unipolar-vs-bipolar]] — 2 種類の違いを詳細に
- [[28byj-48]] — 本プロジェクトで使う 5 線 unipolar
- [[uln2003]] — unipolar 用駆動 IC
- [[stepper-library]] — Arduino 公式制御ライブラリ
- [[tutorials/stepper-library-examples]] — サンプル集
- [[tutorials/unipolar-stepper-motor]] — 5線式ユニポーラのチュートリアル
