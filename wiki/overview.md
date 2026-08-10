# Overview — arduino-stepping-motor-test

この wiki は **Arduino UNO R4 Minima + 28BYJ-48 + ULN2003A** でステッピング
モーターを制御するためのナレッジベースである。

## プロジェクトの状態 (2026-08-10)

**フェーズ**: 開発環境構築完了 (PlatformIO)。

- PlatformIO 環境セットアップ完了 → [[tutorials/platformio-setup]]
- スケッチ雛形 `src/main.cpp` 作成・ビルド成功 (RAM 8.6% / Flash 13.1%)
- スケッチの配置先はプロジェクトルート `src/` に決定

次のアクションは **実機への書き込み** (`pio run -t upload`) と動作確認。

## 主要トピック

### ハードウェア

- [[arduino-uno-r4-minima]] — コントローラ (RA4M1, 5 V, 14 dig + 6 ana)
- [[28byj-48]] — 駆動対象モーター (5 V, 4 相, 1/64, 4096 step/rev)
- [[uln2003]] — 駆動用ダーリントンアレイ (Arduino GPIO 8 mA 制約 → 増幅)

### 概念

- [[stepper-motor]] — ステッピングモーター一般
- [[unipolar-vs-bipolar]] — 駆動方式による分類
- [[ra4m1]] — UNO R4 Minima 搭載 MCU

### 配線 / API

- [[tutorials/stepper-library-examples]] — Stepper ライブラリのサンプル集
- [[tutorials/unipolar-stepper-motor]] — ライブラリを使わない最小例
- [[tutorials/platformio-setup]] — PlatformIO 環境構築・ビルド手順
- [[api/stepper-library]] — `Stepper.h` API リファレンス
- [[api/pinMode]] / [[api/digitalWrite]] / [[api/delay]]

### 一次資料

- [[sources/arduino-uno-r4-minima-datasheet]]
- [[sources/28byj-48-datasheet]]
- [[sources/uln2003a-datasheet]]

## 配線 (最終形)

```
                ┌─────────────────────────────┐
                │  Arduino UNO R4 Minima       │
                │                              │
                │  D8 ──────→ IN1 ┐            │
                │  D9 ──────→ IN2 ├──┐         │
                │  D10 ─────→ IN3 ├──┤         │
                │  D11 ─────→ IN4 ├──┤         │
                │                  │  │         │
                │  5V ─────────────┼──┼─→ COM   │
                │  GND ──────────┐ │  │  │      │
                └────────────────┼─┼──┼──┼──────┘
                                 │ │  │  │
                                 │ │  │  │
                            ┌────┴─┴──┴──┴─────┐
                            │   ULN2003 driver  │
                            │      board         │
                            │                    │
                            │  OUT1 ──→ Blue     │
                            │  OUT2 ──→ Pink     │
                            │  OUT3 ──→ Yellow   │
                            │  OUT4 ──→ Orange   │
                            │  COM  ←── +5V      │
                            │  GND  ←── GND      │
                            └────┬────┬────┬─────┘
                                 │    │    │   │
                                 ▼    ▼    ▼   ▼
                              ┌──────────────────┐
                              │   28BYJ-48       │
                              │   (5V stepper)   │
                              │                  │
                              │  1=Blue, 2=Pink, │
                              │  3=Yellow, 4=Org,│
                              │  5=Red = +5V     │
                              └──────────────────┘
```

GND は共通。モーター用 5V は Arduino 5V ピンとは別の外部電源を推奨。

## スケッチ雛形

[[api/stepper-library]] ベースの最小スケッチ:

```cpp
#include <Stepper.h>

const int stepsPerRevolution = 4096;  // 28BYJ-48 減速後
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  myStepper.setSpeed(15);
  Serial.begin(9600);
}

void loop() {
  myStepper.step(2048);   // 半回転
  delay(500);
  myStepper.step(-2048);  // 逆方向に半回転
  delay(500);
}
```

詳細は [[tutorials/stepper-library-examples]] 参照。

## 次のアクション

1. 実機を USB 接続して `pio run -t upload` で書き込み (要: 実機)
2. `pio device monitor` でシリアル確認、回転方向・ステップ数 (256 vs 4096) を検証
3. 実機テスト結果・観察を wiki にフィードバック
4. 必要に応じて電源回路 (5V 別電源) の検討

## 関連ページ

- [[AGENTS]] — wiki 運用スキーマ
- [[index]] — 全ページカタログ
- [[log]] — 時系列ログ
