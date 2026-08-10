# Index — 全ページカタログ

wiki の全ページを **カテゴリ別** に整理したカタログ。
新しいページを wiki に追加したら、ここにも追記する (AGENTS.md 参照)。

最終更新: 2026-08-10

## Concepts (部品・概念)

| ページ | 一行要約 | タグ |
| --- | --- | --- |
| [[arduino-uno-r4-minima]] | 32bit 化 UNO、RA4M1 搭載、5V 動作 | arduino, mcu, board |
| [[ra4m1]] | UNO R4 搭載の Renesas Cortex-M4 MCU | mcu, renesas |
| [[stepper-motor]] | ステッピングモーター一般 (unipolar/bipolar, 励磁シーケンス) | stepper, theory |
| [[unipolar-vs-bipolar]] | unipolar vs bipolar の構造・駆動比較 | stepper, theory |
| [[28byj-48]] | 5V, 4 相, 1/64 減速, 4096 step/rev unipolar モーター | stepper, unipolar |
| [[uln2003]] | 7ch NPN ダーリントンアレイ (500 mA/ch) | driver, darlington |

## Sources (一次資料のサマリ)

| ページ | 一行要約 | 元ファイル |
| --- | --- | --- |
| [[sources/arduino-uno-r4-minima-datasheet]] | UNO R4 Minima 公式データシート (ABX00080) | ABX00080-datasheet.pdf |
| [[sources/28byj-48-datasheet]] | 28BYJ-48 Kiatronics データシート | step-motor-5v-28byj48-datasheet.pdf |
| [[sources/uln2003a-datasheet]] | ULN2003A TI データシート (SLRS027) | uln2003a-slrs027o-ti.pdf |

## Tutorials (チュートリアル)

| ページ | 一行要約 | 元ファイル |
| --- | --- | --- |
| [[tutorials/stepper-library-examples]] | 公式 "Stepper Motors with Arduino" 4 サンプル | stepper-library-examples.md |
| [[tutorials/unipolar-stepper-motor]] | 公式 "Unipolar Stepper Motor" ライブラリ不使用例 | unipolar-stepper-motor-tutorial.md |
| [[tutorials/platformio-setup]] | PlatformIO 環境構築・ビルド手順 (aarch64 の罠含む) | — (実績メモ) |

## API (Arduino API リファレンス)

| ページ | 一行要約 | 元ファイル |
| --- | --- | --- |
| [[api/pinMode]] | デジタルピンモード設定 | pinMode.adoc |
| [[api/digitalWrite]] | デジタルピンに HIGH/LOW 出力 | digitalWrite.adoc |
| [[api/delay]] | 指定ミリ秒停止 | delay.adoc |
| [[api/stepper-library]] | `Stepper.h` ライブラリ API | Stepper.cpp, api.md |

## システムページ

| ページ | 一行要約 |
| --- | --- |
| [[overview]] | プロジェクト俯瞰・current state |
| [[AGENTS]] | wiki 運用スキーマ |
| [[log]] | 時系列ログ (append-only) |

## タグ別クロスリファレンス

### stepper

- [[stepper-motor]], [[unipolar-vs-bipolar]], [[28byj-48]]
- [[tutorials/stepper-library-examples]], [[tutorials/unipolar-stepper-motor]]
- [[api/stepper-library]]

### arduino / mcu

- [[arduino-uno-r4-minima]], [[ra4m1]]
- [[api/pinMode]], [[api/digitalWrite]], [[api/delay]]

### driver

- [[uln2003]]

### datasheet

- [[sources/arduino-uno-r4-minima-datasheet]]
- [[sources/28byj-48-datasheet]]
- [[sources/uln2003a-datasheet]]

## 更新方法

1. 新しいページを wiki に追加したら、上の対応するカテゴリに 1 行追記する
2. 既存ページのリンクを更新する場合もここを最新に保つ
3. frontmatter の `updated` フィールドも同時に更新する
