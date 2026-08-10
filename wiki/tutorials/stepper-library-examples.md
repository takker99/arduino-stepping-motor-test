---
title: Stepper Motors with Arduino (公式チュートリアル)
type: tutorial
tags: [tutorial, stepper, stepper-library, examples]
sources:
  - raw/arduino/stepper-library-examples.md
  - raw/arduino/stepper-library-src/examples/MotorKnob/MotorKnob.ino
  - raw/arduino/stepper-library-src/examples/stepper_oneRevolution/stepper_oneRevolution.ino
  - raw/arduino/stepper-library-src/examples/stepper_oneStepAtATime/stepper_oneStepAtATime.ino
  - raw/arduino/stepper-library-src/examples/stepper_speedControl/stepper_speedControl.ino
created: 2026-08-10
updated: 2026-08-10
---

# Stepper Motors with Arduino

公式チュートリアル "Arduino and Stepper Motor Configurations" の要約。
出典: <https://docs.arduino.cc/learn/electronics/stepper-motors>

## 概要

ステッピングモーターは、フィードバック機構なしで高精度な角度制御ができるモーター。
シャフトの回転は **電磁コイルへの決まった順序のパルス** で行われ、1 ステップずつ進む。
**2 種類** あり、回路が異なる:

- **unipolar**: U2004 (ULN2004) Darlington Array で駆動
- **bipolar**: SN754410NE H-Bridge で駆動

サンプルコードはどちらのモーターでも動く。

## 必要なハードウェア

- Arduino ボード
- ステッピングモーター
- **U2004 (ULN2004) Darlington Array** ← unipolar の場合
- **SN754410NE H-Bridge** ← bipolar の場合
- モーター用の適切な電源
- ジャンパワイヤ + ブレッドボード

> **Note**: 本プロジェクト ([[28byj-48]] + [[uln2003]]) は **unipolar**。
> ULN2004 と ULN2003A は同等 (どちらも 7 ch Darlington アレイ)。
> モーター電源は外部供給推奨 (Arduino から直接は給電しない)。

## 回路

公式チュートリアルでは **デジタルピン 8, 9, 10, 11** を 4 相制御に使う。
これはサンプルコードと一致するピン番号。

### unipolar 配線 (本プロジェクト)

```
Arduino D8  ──→ ULN2003 IN1 ──→ 28BYJ-48 Blue
Arduino D9  ──→ ULN2003 IN2 ──→ 28BYJ-48 Pink
Arduino D10 ──→ ULN2003 IN3 ──→ 28BYJ-48 Yellow
Arduino D11 ──→ ULN2003 IN4 ──→ 28BYJ-48 Orange
Arduino GND ──→ ULN2003 GND

5V  ──→ 28BYJ-48 Red (COM)
5V  ──→ ULN2003 COM
```

詳細は [[28byj-48]] と [[uln2003]] を参照。

## サンプルコード

[[stepper-library]] を使う。
`Stepper(stepsPerRevolution, pin1, pin2, pin3, pin4)` でインスタンス作成、
`setSpeed(rpm)` で速度、`step(n)` でステップ数を指定して動かす。

### 1. MotorKnob

**可変抵抗 (A0) の変化量に応じてモーターが追従する**。典型的な「つまみ型」
ステッパー制御。

```cpp
#include <Stepper.h>

#define STEPS 100  // モーターのステップ数

Stepper stepper(STEPS, 8, 9, 10, 11);

int previous = 0;

void setup() {
  stepper.setSpeed(30);
}

void loop() {
  int val = analogRead(0);
  stepper.step(val - previous);
  previous = val;
}
```

> 100 steps に設定しているが、これはモーターが 100 steps/rev の前提
> (例: 1.8°/step の NEMA モーター)。
> **[[28byj-48]] は 4096 steps/rev (減速後) なので、4096 に変更する**。

### 2. StepperOneRevolution

**1 回転 → 反対方向に 1 回転** を繰り返す最もシンプルな動作確認用コード。

```cpp
#include <Stepper.h>

const int stepsPerRevolution = 200;  // モーターに合わせて変更

Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  myStepper.setSpeed(60);
  Serial.begin(9600);
}

void loop() {
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  delay(500);

  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  delay(500);
}
```

### 3. StepperOneStepAtATime

**1 ステップずつ** 進める超低速コード。配線が正しいか確認するのに最適。

```cpp
#include <Stepper.h>

const int stepsPerRevolution = 200;

Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

int stepCount = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  myStepper.step(1);
  Serial.print("steps:");
  Serial.println(stepCount);
  stepCount++;
  delay(500);
}
```

> 全ステップが同じ方向に進むなら、配線 OK。
> あるステップで逆方向に進むなら、コイル端の対応 (例: A/C 入れ替え) を疑う。

### 4. StepperSpeedControl

**可変抵抗 (A0) の値で速度を変える**。 potentiometer でモーターを連続的に
コントロールする例。

```cpp
#include <Stepper.h>

const int stepsPerRevolution = 200;

Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

int stepCount = 0;

void setup() {
  // nothing
}

void loop() {
  int sensorReading = analogRead(A0);
  int motorSpeed = map(sensorReading, 0, 1023, 0, 100);
  if (motorSpeed > 0) {
    myStepper.setSpeed(motorSpeed);
    myStepper.step(stepsPerRevolution / 100);
  }
}
```

## コード改造のポイント (28BYJ-48 用)

1. **stepsPerRevolution を 4096 に変更** (減速後 1 回転)
   - 内部 2 相励磁 × 64 ギア比 = 内部 64 step × 64 = 4096 step/rev (概算)
   - もしくは安全マージンを見て 2048 でも可 (2 phase × 64 × 32)
2. **ピンは D8, D9, D10, D11 のまま** (サンプルと一致)
3. **電源はモーター用に 5V 外部電源を別途用意** (Arduino 5V から直接は取らない)
4. **GND は共通化**

## トラブルシュート

- モーターが震えるだけで回らない: 配線が間違っている可能性。
  `StepperOneStepAtATime` で 1 step ずつ検証。
- モーターが熱くなる: 励磁シーケンスが正しくない (2 相 → 1 相が半々に)。
  もしくは電流が定格超過。
- 音がうるさい: 速度が速すぎる。`setSpeed()` を下げる。
- トルクが足りない: 電源電圧・電流を確認。

## 関連ページ

- [[stepper-motor]]
- [[stepper-library]]
- [[28byj-48]]
- [[uln2003]]
- [[unipolar-vs-bipolar]]
- [[tutorials/unipolar-stepper-motor]]
