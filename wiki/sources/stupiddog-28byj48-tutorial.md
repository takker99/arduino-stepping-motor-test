---
title: StupidDog's blog — ステッピングモーター(28BYJ-48)の動かし方 (ULN2003A使用)
type: source
tags: [28byj-48, stepper, tutorial, uln2003, arduino]
url: https://stupiddog.jp/note/archives/1235
ingested: 2026-08-13
---

# StupidDog's blog — 28BYJ-48 の動かし方 (ULN2003A 使用)

- URL: <https://stupiddog.jp/note/archives/1235>
- 投稿: 2019-05-07 (StupidDog 氏, 実機検証記事)
- 関連: 前編「28BYJ-48 を分解して仕組みを調べてみた」(archives/1209)

## 概要

28BYJ-48 + ULN2003 ドライバーボード + Arduino 標準 **Stepper ライブラリ** で
駆動する手順の実機検証記事。

- 電源: MB102 電源モジュール + 9V 2A アダプタ → 5V (本プロジェクトと同構成)
- 配線: Arduino D4–D7 → ドライバーボード IN1–IN4
  (Blue=D4, Pink=D5, Yellow=D6, Orange=D7 — 本プロジェクトと同色順)

## 重要: Stepper ライブラリの 4 線シーケンスと相順 (phase order) の問題

Stepper ライブラリの 4 ピン構成のシーケンスは (Stepper.cpp のコメントより):

```
Step C0 C1 C2 C3
   1  1  0  1  0
   2  0  1  1  0
   3  0  1  0  1
   4  1  0  0  1
```

これは **バイファイラ巻きユニポーラ駆動** を想定した 2 相励磁パターンで、
28BYJ-48 の 4 相を色順 (Blue, Pink, Yellow, Orange) のまま C0–C3 に
割り当てると **回転磁界にならず振動するだけ** になる。

> **対処 (実機で検証済み)**: 配線の 2 番目と 3 番目を入れ替えるか、
> **コンストラクタの第 2 引数と第 3 引数を入れ替える**。
>
> ```cpp
> // 正: (Blue, Yellow, Pink, Orange) の順で渡す
> Stepper myStepper(MOTOR_STEPS, MOTOR_1, MOTOR_3, MOTOR_2, MOTOR_4);
> ```
>
> 同様のパターンはコミュニティの定番例
> `Stepper(2048, 8, 10, 9, 11)` (IN1, IN3, IN2, IN4 順) にも見られる。

記事のスケッチでは `MOTOR_STEPS = 2048` を使用 (スイッチ押下で 45° 回転 = 256 step)。
記事の回路図では D4–D7 を IN1–IN4 に接続している (本プロジェクトの D7–D4 とは
Arduino ピン番号が逆だが、色→IN の対応は同じ)。

## 本プロジェクトとの関連

- 本プロジェクトの症状 (ステップ周波数の振動音 + 回転しない) はこの記事の
  「相順不一致」と同じ現象 (2026-08-13 診断確定)
- 実機配線: IN1=Blue(D7), IN2=Pink(D6), IN3=Yellow(D5), IN4=Orange(D4)
  → 修正: `Stepper(STEPS, D7, D5, D6, D4)` (第 2・3 引数入れ替え)
- steps/rev の正しい値: **2048** (4 相 2 相励磁シーケンス 1 回転)
- 関連: [[28byj-48]], [[api/stepper-library]], [[uln2003]]
