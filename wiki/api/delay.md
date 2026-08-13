---
title: delay()
type: api
tags: [api, arduino, time]
sources:
  - raw/arduino/language-reference/delay.adoc
created: 2026-08-10
updated: 2026-08-10
---

# `delay()`

指定したミリ秒数だけプログラムを一時停止する。
ステッピングモーター制御では **ステップ間隔** を作るのに使う。

出典: <https://docs.arduino.cc/language-reference/en/functions/time/delay/>

## シグネチャ

```cpp
void delay(unsigned long ms);
```

## 引数

| 引数 | 型 | 値 |
| --- | --- | --- |
| `ms` | `unsigned long` | 一時停止する時間 (ミリ秒)。最大 4,294,967,295 ms ≈ 49.7 日 |

## 戻り値

なし。

## 動作

- 指定した ms だけプログラムの実行を停止する
- 1000 ms = 1 秒
- `delay()` 中も割り込みは有効 (シリアル受信、PWM、attachInterrupt は動く)

## 例 (LED blink)

```cpp
int ledPin = 13;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(1000);    // 1 秒待つ
  digitalWrite(ledPin, LOW);
  delay(1000);    // 1 秒待つ
}
```

## ステッピングモーター制御での典型

1 相励磁の各ステップ後に delay を入れる:

```cpp
void stepOnce() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(2);  // 2 ms 待つ → 1 ステップ
}

void loop() {
  for (int i = 0; i < 4; i++) {
    stepOnce();
  }
}
```

`delay()` の値でステップ速度が決まる:
- 2 ms → 500 step/s = 125 cycle/s
- 10 ms → 100 step/s = 25 cycle/s

[[28byj-48]] の場合は 2048 step/rev なので、
- 2 ms/step → 4.1 s/rev (≈ 14.6 RPM)
- 10 ms/step → 20.5 s/rev (≈ 2.9 RPM)

[[stepper-library]] を使うと `setSpeed(rpm)` で RPM 単位で指定でき、
内部で `micros()` ベースのタイミング管理をする (ブロッキング delay ではない)。

## 注意点 (公式リファレンスより)

> "No other reading of sensors, mathematical calculations, or pin manipulation can
> go on during the delay function, so in effect, it brings most other activity
> to a halt."

- `delay()` 中は他の処理が止まる
- 数十 ms 以上の delay は `millis()` + `Blink Without Delay` パターンが推奨
- ステッパー制御では delay 自体は問題ない (モーター駆動中は他に何もする
  必要がないことが多いため)

## ブロッキングの代用

[[stepper-library]] の `step()` は **内部で `micros()` + `yield()` を使う
ノンブロッキング設計** になっている ([Stepper.cpp 実装]). 長時間 step 中でも
他の処理 (シリアル入力等) が可能。

[Stepper.cpp 実装]: ../stepper-library#内部実装

## 関連ページ

- [[pinMode]]
- [[digitalWrite]]
- [[stepper-library]] — ノンブロッキングな step
- [[tutorials/unipolar-stepper-motor]] — delay ベースのサンプル
- [[tutorials/stepper-library-examples]] — ライブラリ使用例
