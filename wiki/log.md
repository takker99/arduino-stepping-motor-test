# Log — 時系列ログ

wiki の **append-only** な活動履歴。
各エントリは `## [YYYY-MM-DD] <種別> | <タイトル>` で始める (grep で抽出可能)。
種別: `ingest` / `query` / `lint` / `update` / `note` 等。

最終更新: 2026-08-10

## [2026-08-10] ingest | 初回一括 ingest

raw/ 配下の全資料を一括 ingest して wiki 基盤を構築。

**ボード / マイコン**
- ABX00080 datasheet (PDF + markdown) → [[sources/arduino-uno-r4-minima-datasheet]]
- user-manual.md → [[arduino-uno-r4-minima]]
- RA4M1 datasheet (PDF) → [[ra4m1]]

**ステッパー / ドライバ**
- 28BYJ-48 datasheet → [[28byj-48]], [[sources/28byj-48-datasheet]]
- ULN2003A SLRS027 datasheet → [[uln2003]], [[sources/uln2003a-datasheet]]

**チュートリアル / ライブラリ**
- stepper-library-examples.md → [[tutorials/stepper-library-examples]]
- unipolar-stepper-motor-tutorial.md → [[tutorials/unipolar-stepper-motor]]
- Stepper library src/ + docs/ + examples/ → [[api/stepper-library]]

**言語リファレンス**
- pinMode.adoc → [[api/pinMode]]
- digitalWrite.adoc → [[api/digitalWrite]]
- delay.adoc → [[api/delay]]

**スキーマ / システム**
- [[AGENTS]] (新規作成)
- [[overview]], [[index]] (新規作成)
- 本 log.md (新規作成)

**PDF 二段階読解ルール適用結果**
- 全文 pdftotext でテキスト抽出 → 6 ファイル (/tmp/pdftext/)
- 図表必要ページのみ pdftoppm で画像化:
  - 28BYJ-48 datasheet p.1 (ピンマップ + 外形図)
  - ABX00080-full-pinout p.1–4 (フルピンアウト 4 ページ)
  - ABX00080-schematics p.1 (回路図本体)
  - ULN2003A p.3 (ピン配置), p.14 (等価回路), p.17 (駆動設計), p.19 (System Examples)

## [2026-08-10] note | 配線確定

28BYJ-48 + ULN2003 driver board を Arduino UNO R4 Minima の D8–D11 で制御する
配線を [[overview]] に記載。

- IN1 = D8 → Blue (coil A)
- IN2 = D9 → Pink (coil B)
- IN3 = D10 → Yellow (coil C)
- IN4 = D11 → Orange (coil D)
- 5V (Arduino 5V or 外部 5V) → Red (COM) ＋ ULN2003 COM
- GND 共通

サンプルスケッチ (Stepper ライブラリ使用) も [[overview]] に雛形を記載。

## [2026-08-10] note | 未解決事項

- モーター用 5V 電源の選択 (Arduino 5V から取るか、外部電源を使うか) —
  実機テストで決定
- 完成版スケッチの配置先 (wiki に置くか、別途 `src/` を設けるか) —
  ユーザーと相談
- [[28byj-48]] の `stepsPerRevolution` の正確な値 (256 vs 4096) —
  実機検証が必要

## [2026-08-10] update | PlatformIO 環境構築

スケッチ開発環境として PlatformIO を導入。ビルド成功まで確認済み。

- `platformio.ini` 作成 (env: uno_r4_minima, renesas-ra @ 1.9.0, Arduino framework)
- `src/main.cpp` 作成 — [[tutorials/stepper-library-examples]] の
  `stepper_oneRevolution` ベース + 28BYJ-48 用設定 (4096 step/rev, D8–D11)
- Stepper ライブラリを `lib_deps = arduino-libraries/Stepper` で追加
  (framework に同梱されないため)
- **aarch64 の罠**: renesas-ra の要求ツールチェーン ~1.70201.0 は
  linux_aarch64 ビルドが存在しない → `platform_packages` で
  ~1.120301.0 (GCC 12.3.1) に上書きして解決
  (詳細: [[tutorials/platformio-setup]])
- ビルド成功: RAM 8.6% / Flash 13.1%
- `.gitignore` に `.pio/`, `.vscode/` を追加
- **未解決事項の決着**: 完成版スケッチの配置先 → プロジェクトルート `src/` に決定
  ([[tutorials/platformio-setup]] 参照)
