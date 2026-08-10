---
title: Unipolar Stepper Motor (公式チュートリアル)
type: tutorial
tags: [tutorial, stepper, unipolar, uln2003, no-library]
sources:
  - raw/arduino/unipolar-stepper-motor-tutorial.md
created: 2026-08-10
updated: 2026-08-10
---

# Unipolar Stepper Motor (公式チュートリアル)

公式チュートリアル "Unipolar Stepper Motor" の要約。
出典: <https://docs.arduino.cc/tutorials/generic/unipolar-stepper-motor>

## 概要

旧 5.25 インチフロッピードライブ等から取り出したユニポーラ・ステッピングモーターを
Arduino + [[uln2003]] (ULN2003A) + 10kΩ potentiometer で駆動するチュートリアル。

本プロジェクトの [[28byj-48]] + [[uln2003]] と **同じ構成**。

## 必要なハードウェア

- Arduino ボード
- ユニポーラ・ステッピングモーター (旧フロッピー等)
- **ULN2003A driver** (IC 単体 or driver board)
- 10 kΩ potentiometer
- ジャンパワイヤ
- Arduino IDE

## ハードウェア構成

```
                 ┌──────────────┐
                 │  ULN2003A    │
Arduino D8 ─────→│ IN1      OUT1│──→ Motor Coil 1
Arduino D9 ─────→│ IN2      OUT2│──→ Motor Coil 2
Arduino D10 ────→│ IN3      OUT3│──→ Motor Coil 3
Arduino D11 ────→│ IN4      OUT4│──→ Motor Coil 4
                 │           COM │──→ +5V (or motor VCC)
GND ─────────────┤ GND          │
                 └──────────────┘

A0 ──→ 10kΩ pot wiper (両端を 5V / GND)
```

> モーターの Red (COM) は 5V (または別電源) に繋ぐ。
> GND は Arduino GND と共通。

## Example 1: シンプル 1 方向回転

`digitalWrite()` を順番に切り替えて 1 相励磁で回す。[[stepper-library]] を使わない、
最もプリミティブな例。

```cpp
int motorPin1 = 8;
int motorPin2 = 9;
int motorPin3 = 10;
int motorPin4 = 11;
int delayTime = 500;

void setup() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
}

void loop() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(delayTime);

  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(delayTime);

  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
  delay(delayTime);

  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
  delay(delayTime);
}
```

これは **1 相励磁シーケンス** (1 step ごとに 1 ピンだけ HIGH)。
delay を短くすると速度が上がるが、脱出周波数を超えると脱調する。

> **Note**: 公式チュートリアルでは 5 線式モーター (赤が +V、橙・黒が coil 1、
> 茶・黄が coil 2) と書かれている。これは 28BYJ-48 の典型例と同じ。

## Example 2: Potentiometer で速度・方向制御

`analogRead(A0)` の値で `delayTime` を動的に変更し、左右対称で 540 を中立に
して ± で正逆方向を切り替える高度版。

```cpp
int motorPins[] = {8, 9, 10, 11};
int count = 0;
int count2 = 0;
int delayTime = 500;
int val = 0;

void setup() {
  for (count = 0; count < 4; count++) {
    pinMode(motorPins[count], OUTPUT);
  }
}

void moveForward() {
  if ((count2 == 0) || (count2 == 1)) {
    count2 = 16;
  }
  count2 >>= 1;
  for (count = 3; count >= 0; count--) {
    digitalWrite(motorPins[count], count2 >> count & 0x01);
  }
  delay(delayTime);
}

void moveBackward() {
  if ((count2 == 0) || (count2 == 1)) {
    count2 = 16;
  }
  count2 >>= 1;
  for (count = 3; count >= 0; count--) {
    digitalWrite(motorPins[3 - count], count2 >> count & 0x01);
  }
  delay(delayTime);
}

void loop() {
  val = analogRead(0);
  if (val > 540) {
    // val 大 → delay 小 → 高速 → 正転
    delayTime = 2048 - 1024 * val / 512 + 1;
    moveForward();
  } else if (val < 480) {
    // val 小 → delay 小 → 高速 → 逆転
    delayTime = 1024 * val / 512 + 1;
    moveBackward();
  } else {
    delayTime = 1024;
  }
}
```

ビット操作で step パターンを生成する。`count2` を 16 → 8 → 4 → 2 → 1 → 16 → … と
ローテーションし、その 4 bit を逆順 (moveForward) または順方向 (moveBackward) で
出力ピンに反映する。

`count2` の bit 列 (LSB = pin1, MSB = pin4):

| count2 | bit3 | bit2 | bit1 | bit0 |
| --- | --- | --- | --- | --- |
| 16 = 10000 | 1 | 0 | 0 | 0 |
| 8 = 01000 | 0 | 1 | 0 | 0 |
| 4 = 00100 | 0 | 0 | 1 | 0 |
| 2 = 00010 | 0 | 0 | 0 | 1 |

ループするたびに `count2 >>= 1` でシフトしているので、
10000 → 01000 → 00100 → 00010 → 00001 → (初期化で 16 に戻す) → 10000。

## スケッチの比較

| 項目 | Example 1 | Example 2 |
| --- | --- | --- |
| 速度 | 固定 | 可変 (potentiometer) |
| 方向 | 1 方向のみ | 双方向 (pot 中央で中立) |
| コード量 | 少ない | 多い (ビット演算) |
| 教育的価値 | 基本理解 | タイミング・パターンの動的生成 |

## 実プロジェクトへの適用

- 速度・方向制御が要らないなら **Example 1 で十分**
- potentiometer で速度調整したいなら **Example 2** を参考にする
- 複数モーターを扱う・複雑なパターンを生成するなら [[stepper-library]]
  の方がコードが読みやすい

## 関連ページ

- [[stepper-motor]]
- [[uln2003]]
- [[28byj-48]]
- [[unipolar-vs-bipolar]]
- [[tutorials/stepper-library-examples]]
- [[api/pinMode]]
- [[api/digitalWrite]]
- [[api/delay]]
