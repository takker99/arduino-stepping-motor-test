---
title: digitalWrite()
type: api
tags: [api, arduino, digital-io]
sources:
  - raw/arduino/language-reference/digitalWrite.adoc
created: 2026-08-10
updated: 2026-08-10
---

# `digitalWrite()`

デジタルピンに `HIGH` または `LOW` を書き込む。
ステッピングモーター制御では **励磁パターンを出力する基本 API**。

出典: <https://docs.arduino.cc/language-reference/en/functions/digital-io/digital-io/digitalwrite/>

## シグネチャ

```cpp
void digitalWrite(uint8_t pin, uint8_t value);
```

## 引数

| 引数 | 型 | 値 |
| --- | --- | --- |
| `pin` | `uint8_t` | 対象のピン番号 |
| `value` | `uint8_t` | `HIGH` または `LOW` |

## 戻り値

なし。

## 動作 (mode 別)

| pinMode | HIGH 時の動作 | LOW 時の動作 |
| --- | --- | --- |
| `OUTPUT` | 5 V (3.3 V) を出力 | 0 V (GND) を出力 |
| `INPUT` | 内蔵プルアップ **有効化** | 内蔵プルアップ **無効化** |
| `INPUT_PULLUP` | (INPUT と同じ) | (INPUT と同じ) |

## 例 (ステッピングモーター制御)

[[28byj-48]] + [[uln2003]] で 1 相励磁シーケンスを回す:

```cpp
int motorPin1 = 8;
int motorPin2 = 9;
int motorPin3 = 10;
int motorPin4 = 11;

void setup() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
}

void loop() {
  // Step 1: Phase A のみ励磁
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(10);

  // Step 2: Phase B のみ励磁
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(10);

  // Step 3
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
  delay(10);

  // Step 4
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
  delay(10);
}
```

## 注意点

### 1. `pinMode(OUTPUT)` を必ず先に呼ぶ

`pinMode()` を呼ばずに `digitalWrite(HIGH)` すると、内蔵プルアップが
電流制限抵抗として働くため LED が暗く点灯するだけになる。
**ステッパー制御では必ず `OUTPUT` に設定**する。

### 2. analog ピンも使える

`A0` 〜 `A5` も `digitalWrite(A0, HIGH)` でデジタル出力に使える
(Nano, Pro Mini の A6/A7 を除く)。

### 3. HIGH = 5V, LOW = 0V (UNO R4 Minima)

[[arduino-uno-r4-minima]] は 5V 動作なので、HIGH は 5V 出力。
3.3V デバイス (esp. DAC, ADC 等) を駆動する場合はレベルシフタを考慮。

## 関連ページ

- [[pinMode]] — 出力モード設定
- [[delay]] — 時間待ち
- [[stepper-library]] — `digitalWrite` を抽象化するライブラリ
- [[tutorials/unipolar-stepper-motor]] — サンプルでの使用例
- [[tutorials/stepper-library-examples]] — サンプル集
