---
title: Stepper library
type: api
tags: [api, arduino, stepper, library]
sources:
  - raw/arduino/stepper-library-src/src/Stepper.h
  - raw/arduino/stepper-library-src/src/Stepper.cpp
  - raw/arduino/stepper-library-src/README.adoc
  - raw/arduino/stepper-library-src/docs/api.md
  - raw/arduino/stepper-library-src/docs/readme.md
  - raw/arduino/stepper-library-src/examples/MotorKnob/MotorKnob.ino
  - raw/arduino/stepper-library-src/examples/stepper_oneRevolution/stepper_oneRevolution.ino
  - raw/arduino/stepper-library-src/examples/stepper_oneStepAtATime/stepper_oneStepAtATime.ino
  - raw/arduino/stepper-library-src/examples/stepper_speedControl/stepper_speedControl.ino
created: 2026-08-10
updated: 2026-08-10
---

# Stepper library

Arduino 公式のステッピングモーター制御ライブラリ。
出典: <https://github.com/arduino-libraries/Stepper>
バージョン: **1.1.3** (当 raw に shallow clone)

## 概要

- **対応**: unipolar (4/5線), bipolar (4線), 5相5線
- **接続ピン数**: 2 / 4 / 5
- **依存**: `Arduino.h` のみ (追加ライブラリなし)

## クラス概要

```cpp
class Stepper {
 public:
  Stepper(int number_of_steps, int motor_pin_1, int motor_pin_2);
  Stepper(int number_of_steps, int motor_pin_1, int motor_pin_2,
                          int motor_pin_3, int motor_pin_4);
  Stepper(int number_of_steps, int motor_pin_1, int motor_pin_2,
                          int motor_pin_3, int motor_pin_4,
                          int motor_pin_5);

  void setSpeed(long whatSpeed);
  void step(int number_of_steps);
  int version(void);

 private:
  void stepMotor(int this_step);
  int direction;
  unsigned long step_delay;
  int number_of_steps;
  int pin_count;
  int step_number;
  int motor_pin_1, motor_pin_2, motor_pin_3, motor_pin_4, motor_pin_5;
  unsigned long last_step_time;
};
```

## API

### `Stepper(steps, pin1, pin2[, pin3, pin4[, pin5]])`

`Stepper` のインスタンスを作成し、指定したピンを出力モードに設定する。

| 引数 | 意味 |
| --- | --- |
| `steps` | 1 回転あたりのステップ数 |
| `pin1`, `pin2` | モーター制御ピン (2 ピン構成) |
| `pin3`, `pin4` | 追加ピン (4 ピン構成) |
| `pin5` | 追加ピン (5 相 5 線構成) |

```cpp
// 2 ピン (full H-bridge 必須)
Stepper motor(100, 5, 6);

// 4 ピン (unipolar または bipolar)
Stepper motor(200, 8, 9, 10, 11);
```

### `setSpeed(rpms)`

モーターの速度を **RPM** で設定する。実際の動作はせず、次の `step()` 呼び出し
から適用される。

| 引数 | 意味 |
| --- | --- |
| `rpms` | RPM (正の long) |

内部計算:

```cpp
step_delay = 60_000_000 / number_of_steps / rpms;  // us 単位
```

### `step(steps)`

指定したステップ数だけモーターを動かす。

| 引数 | 意味 |
| --- | --- |
| `steps` | ステップ数 (正で正転、負で逆転) |

- **ブロッキング関数** (`delay` 同様、完了まで制御が戻らない)
- 内部で `micros()` を見て `step_delay` 経過後に次のステップへ
- 他にする処理がないなら問題ないが、長時間 step 中にも他の処理をしたいなら
  小さな `steps` 値で複数回に分割する

### `version()`

ライブラリのバージョン番号を返す (現在 `5` を返す)。

## 内部実装

### ステップシーケンス (4 ピン構成)

`Stepper.cpp` の `stepMotor(int thisStep)` switch 文より。
**2 相励磁 (full-step)** シーケンス。

| Step | motor_pin_1 | motor_pin_2 | motor_pin_3 | motor_pin_4 |
| --- | --- | --- | --- | --- |
| 0 | HIGH | LOW | HIGH | LOW |
| 1 | LOW | HIGH | HIGH | LOW |
| 2 | LOW | HIGH | LOW | HIGH |
| 3 | HIGH | LOW | LOW | HIGH |

ヘッダコメント (`Stepper.h`) に記載された表と一致:

```
Step C0 C1 C2 C3
   1  1  0  1  0
   2  0  1  1  0
   3  0  1  0  1
   4  1  0  0  1
```

### タイミング管理

```cpp
void Stepper::step(int steps_to_move) {
  int steps_left = abs(steps_to_move);
  if (steps_to_move > 0) this->direction = 1;
  if (steps_to_move < 0) this->direction = 0;

  while (steps_left > 0) {
    unsigned long now = micros();
    if (now - this->last_step_time >= this->step_delay) {
      this->last_step_time = now;
      // step_number を進めて
      if (this->direction == 1) {
        this->step_number++;
        if (this->step_number == this->number_of_steps) {
          this->step_number = 0;
        }
      } else {
        if (this->step_number == 0) {
          this->step_number = this->number_of_steps;
        }
        this->step_number--;
      }
      steps_left--;
      if (this->pin_count == 5)
        stepMotor(this->step_number % 10);
      else
        stepMotor(this->step_number % 4);
    } else {
      yield();  // 他のタスクに制御を譲る (ESP8266/ESP32 用)
    }
  }
}
```

- `micros()` ベースでタイミング管理 (`delay()` ではない)
- 待機中は `yield()` を呼ぶ → ESP8266/ESP32 等のマルチタスク環境で有用
- 1回転したら `step_number` を 0 に戻す (オーバーフロー防止)

### 2 ピン / 5 ピン構成

- **2 ピン**: H-bridge と外部インバータ回路が必要 (L293 等)
- **5 ピン**: 5 相 5 線ステッピングモーター用 (本プロジェクトでは未使用)

## サンプルコード

### `stepper_oneRevolution`

[[28byj-48]] 用に書き換えた最小スケッチ:

```cpp
#include <Stepper.h>

const int stepsPerRevolution = 4096;  // 28BYJ-48 の減速後 1 回転

// initialize the Stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  myStepper.setSpeed(15);     // RPM (5 rpm 程度が安定)
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

> **Note**: `stepsPerRevolution = 4096` は 28BYJ-48 の減速後 1 回転。
> ただしライブラリは **2 相励磁 (full-step)** シーケンスなので、
> 内部ステップ 4 × ギア比 64 = **256 ステップ/rev** 相当の動作になる。
> もし 1:1 で 1 回転させたければ `setSpeed(15)` 程度で 4096 step を回す。

### `MotorKnob`

A0 の可変抵抗でモーターを追従制御:

```cpp
#include <Stepper.h>
#define STEPS 100  // ← 28BYJ-48 なら 4096

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

### `stepper_oneStepAtATime`

配線の確認用 (1 ステップずつ):

```cpp
#include <Stepper.h>
const int stepsPerRevolution = 200;  // ← 28BYJ-48 なら 4096
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

### `stepper_speedControl`

potentiometer で速度を変える:

```cpp
#include <Stepper.h>
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);
int stepCount = 0;

void setup() {}

void loop() {
  int sensorReading = analogRead(A0);
  int motorSpeed = map(sensorReading, 0, 1023, 0, 100);
  if (motorSpeed > 0) {
    myStepper.setSpeed(motorSpeed);
    myStepper.step(stepsPerRevolution / 100);
  }
}
```

## ライセンス

> Copyright (c) Arduino LLC. All right reserved.
> Copyright (c) Sebastian Gassner. All right reserved.
> Copyright (c) Noah Shibley. All right reserved.

LGPL 2.1 (詳細は `LICENSE.txt`)。

## 関連ページ

- [[tutorials/stepper-library-examples]] — サンプル集
- [[tutorials/unipolar-stepper-motor]] — ライブラリを使わない例
- [[28byj-48]] — 本プロジェクトのモーター
- [[uln2003]] — 駆動 IC
- [[pinMode]] — 内部で呼ばれる
- [[digitalWrite]] — 内部で呼ばれる
