---
title: pinMode()
type: api
tags: [api, arduino, digital-io]
sources:
  - raw/arduino/language-reference/pinMode.adoc
created: 2026-08-10
updated: 2026-08-10
---

# `pinMode()`

デジタルピンのモード (入力 / 出力) を設定する。
ステッピングモーター制御では `pinMode(pin, OUTPUT)` を必ず呼ぶ。

出典: <https://docs.arduino.cc/language-reference/en/functions/digital-io/digital-io/pinmode/>

## シグネチャ

```cpp
void pinMode(uint8_t pin, uint8_t mode);
```

## 引数

| 引数 | 型 | 値 |
| --- | --- | --- |
| `pin` | `uint8_t` | 設定対象のピン番号 (D0–D13, A0–A5 など) |
| `mode` | `uint8_t` | `INPUT`, `OUTPUT`, `INPUT_PULLUP` のいずれか |

## 戻り値

なし。

## 各モードの意味

- `OUTPUT`: ピンを出力モードに。HIGH (5 V) / LOW (0 V) を `digitalWrite()` で
  出力できる。**ステッピングモーター制御の GPIO はすべてこれ**。
- `INPUT`: 入力モード (HIGH-IMPEDANCE)。`digitalRead()` で外部信号を読み取る。
- `INPUT_PULLUP`: 入力モード + 内蔵プルアップ抵抗を有効化 (約 20–50 kΩ)。
  スイッチのチャタリング防止などに使う。

## 例

```cpp
void setup() {
  pinMode(13, OUTPUT);    // D13 を出力に (LED_BUILTIN)
  pinMode(2, INPUT_PULLUP); // D2 を入力+プルアップに
}

void loop() {
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
}
```

## ステッピングモーター制御での典型

[[28byj-48]] + [[uln2003]] driver board を Arduino UNO R4 の D8–D11 に繋ぐ場合:

```cpp
const int motorPin1 = 8;
const int motorPin2 = 9;
const int motorPin3 = 10;
const int motorPin4 = 11;

void setup() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
}
```

> **重要**: `pinMode()` を `OUTPUT` に設定しないと `digitalWrite()` しても
> LED が暗く点灯するだけになる ([公式リファレンス digitalWrite の警告])。
> 内部プルアップ抵抗が電流制限抵抗として働くため。

[公式リファレンス digitalWrite の警告]: ../digitalWrite#notes-and-warnings

## 注意点

- **analog ピン (A0–A5) もデジタルとして使える**。
  ピン番号は A0 (= 14) などの数字でも、`A0` 定数でも指定できる。
- 一度 `pinMode()` を設定すれば、`digitalWrite()` を何度呼んでもモードは維持される。
- `pinMode()` の設定はマイコンの再起動や `RESET` まで持続する。

## 関連ページ

- [[digitalWrite]] — 出力値の書き込み
- [[delay]] — 時間待ち
- [[stepper-library]] — ピンモードを自動設定するライブラリ
- [[tutorials/unipolar-stepper-motor]] — サンプルでの使用例
