# Log — 時系列ログ

wiki の **append-only** な活動履歴。
各エントリは `## [YYYY-MM-DD] <種別> | <タイトル>` で始める (grep で抽出可能)。
種別: `ingest` / `query` / `lint` / `update` / `note` 等。

最終更新: 2026-08-12

## [2026-08-10] note | 通信方式・最終用途の設計議論

ユーザーとの設計議論。Modbus TCP / 試験機用途の通信方式 / 最終用途の計測要件を整理。

- **Modbus TCP vs JSON HTTP**: 技術的には可能 (自前実装前提。公式 ArduinoModbus は
  renesas 未対応 — open issue #119)。この用途ならどちらでも成立、決定要因は
  「クライアントが産業ソフトかどうか」→ [[tutorials/wifi-api-design-notes]]
- **WiFi vs USB-serial vs Bluetooth**: 単機・PC 隣接なら USB が最善 (遅延 ~1 ms で決定的)。
  **今回の結論** = 試作用に WiFi HTTP を継続、最終形態は PC 制御 + USB-serial が有力
- **最終用途の確定**: 三軸試験機 (ひずみゲージ + 80 Hz サンプリング, PC 制御メイン)。
  2.4 GHz RF は測定に実質無関係、実ノイズ源はエイリアシング / GND ループ / 伝導ノイズ
  → [[triaxial-test]] (新規作成)
- **用語集の方針**: 一般用語 (MCU, OTA, インピーダンス等) の個別ページは作成しない
  (LLM のモデル知識に含まれるため情報量ゼロ)。用語定義は [[triaxial-test]] に
  inline で収録

### 作成・更新

- [[tutorials/wifi-api-design-notes]] — Modbus 比較 + 通信方式選択を追記
- [[triaxial-test]] — 新規作成 (concepts/)
- [[index]] — 新ページ登録

## [2026-08-10] update | 方針変更: UNO R4 WiFi + MB102 + API server 化

ユーザー指示により本プロジェクトの方針を全面改訂。
旧 [[arduino-uno-r4-minima]] ベースの有線制御 →
**[[arduino-uno-r4-wifi|UNO R4 WiFi]] + [[mb102|MB102]] + 9V 1.3A アダプタ +
Wi-Fi HTTP API サーバ** 構成に移行。

### 設計判断 (ユーザー確認済み)

1. **電源トポロジ**: Arduino は USB 給電 (開発時) / 別 USB 充電器 (本番)。
   MB102 はモーター 5V 専用。
   (理由: [[ams1117|AMS1117]] 700 mA 枠の電流バジェット → 詳細は [[arduino-uno-r4-wifi#電源トポロジ]])
2. **Wi-Fi モード**: STA 実装 (MVP)。AP フォールバックは将来用に
   [[tutorials/wifi-api-design-notes]] に設計を残す。
3. **API スタイル**: MVP はシンプル (POST /step + GET /status)。
   他の設計案 (高機能 REST / ジョブキュー) も設計ノートに残す。

### ingest 一覧

**ボード / コプロセッサ**
- ABX00087 datasheet (markdown 完読 + PDF) → [[sources/arduino-uno-r4-wifi-datasheet]]
- ABX00087 full-pinout (PDF) → [[arduino-uno-r4-wifi#ピン配置 digital/analog headers]]
- ABX00087 schematics (PDF, 3 ページ) → 参考
- user-manual.md (markdown 完読) → [[arduino-uno-r4-wifi]]
- ESP32-S3-MINI-1/1U datasheet (PDF, 53 ページ, 概念レベル参照) → [[sources/esp32-s3-mini-1-datasheet]]
- RA4M1 datasheet (PDF, 130 ページ) → [[ra4m1]] (D10/D11 端子差分を確認)

**電源モジュール**
- MB102 AZ-Delivery datasheet (PDF, 4 ページ) → [[sources/mb102-datasheet]]
- MB102 AZ-Delivery schematic (PDF, 1 ページ, 画像化) → [[mb102#回路構成]]
- MB102 Handson Technology datasheet (PDF, 4 ページ) → [[sources/mb102-ps-datasheet]]
- AMS1117 datasheet (UMW 版, テキスト抽出のみ) → [[sources/ams1117-datasheet]]

**PDF 二段階読解ルール適用結果**
- 全文 pdftotext でテキスト抽出 → 4 ファイル (/tmp/pdftext/)
- 画像化: MB102 schematic 1 ページのみ

### 新規作成 wiki ページ

- **concepts**: [[arduino-uno-r4-wifi]], [[esp32-s3-mini-1]], [[mb102]], [[ams1117]]
- **sources**: [[sources/arduino-uno-r4-wifi-datasheet]],
  [[sources/esp32-s3-mini-1-datasheet]],
  [[sources/mb102-datasheet]], [[sources/mb102-ps-datasheet]],
  [[sources/ams1117-datasheet]]
- **tutorials**: [[tutorials/wifi-api-server]] (MVP),
  [[tutorials/wifi-api-design-notes]] (将来設計),
  [[tutorials/mb102-power-wiring]]
- **api**: [[api/wifis3-library]], [[api/webserver-library]]

### 既存ページ更新

- [[overview]] — 全面改訂 (新ハードウェア + 電源トポロジ + API サーバ計画)
- [[AGENTS]] — 目的文更新、命名表更新、Wi-Fi API サーバ用ルール追加
- [[index]] — 新ページ登録、タグ別クロスリファレンス追加
- [[arduino-uno-r4-minima]] — `superseded` 注記追加 (コンテンツは温存)
- [[ra4m1]] — Minima/WiFi で D10/D11 対応端子が異なる旨を追記
  (**P112→P103, P109→P411** の差分を発見・記録)
- [[tutorials/platformio-setup]] — env を `uno_r4_wifi` に変更

### コード

- `platformio.ini` — `env:uno_r4_wifi` に変更予定 (要 update)
- `src/main.cpp` — Wi-Fi API サーバスケッチに差し替え予定 (要 update)

### 未解決事項 (引き継ぎ)

- Wi-Fi 認証情報 (SSID/PASS) のハードコード方針 → `src/secrets.h` 分離
- MB102 と Arduino の GND 共通化を配線チュートリアルに記載済み
- `/stop` のノンブロッキング化は将来課題 ([[tutorials/wifi-api-design-notes]])
- WiFiS3 + WebServer + Stepper 同時の実機 RAM / Flash 計測は未実施

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

## [2026-08-10] note | 配線確定 (旧 Minima 方針)

28BYJ-48 + ULN2003 driver board を Arduino UNO R4 Minima の D8–D11 で制御する
配線を旧 [[overview]] に記載。**新方針 ([[arduino-uno-r4-wifi]]) でも D8–D11 の
Arduino ピン番号は共通**なため流用可能 (ただし D10/D11 が指す RA4M1 端子は
Minima と WiFi で異なるため、ボードレジスタを直接叩く場合は注意)。

- IN1 = D8 → Blue (coil A)
- IN2 = D9 → Pink (coil B)
- IN3 = D10 → Yellow (coil C)
- IN4 = D11 → Orange (coil D)
- 5V (Arduino 5V or 外部 5V) → Red (COM) ＋ ULN2003 COM
- GND 共通

> ⚠️ **訂正 (2026-08-12)**: 実機配線は **IN1=D7, IN2=D6, IN3=D5, IN4=D4** で確定。
> 旧記述の D8–D11 は不採用。コード `src/main.cpp` も 7, 6, 5, 4 に変更済み。
> 詳細は [[log#2026-08-12 中座]] のエントリ参照。

## [2026-08-10] note | 未解決事項 (旧方針)

- モーター用 5V 電源の選択 (Arduino 5V から取るか、外部電源を使うか) —
  **新方針で解決**: MB102 から 5V 給電、Arduino は USB 給電 ([[arduino-uno-r4-wifi#電源トポロジ]])
- 完成版スケッチの配置先 (wiki に置くか、別途 `src/` を設けるか) —
  ユーザーと相談 (旧方針で src/ 採用と決着済み)
- [[28byj-48]] の `stepsPerRevolution` の正確な値 (256 vs 4096) —
  実機検証が必要 (新方針でも要検証)

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

## [2026-08-12] update | ESP32-S3 Wi-Fi ファームウェア更新手順の新規ページ化

実機で `Connected. IP: 0.0.0.0` (ファームウェア 0.4.1 の localIP() バグ) が発覚。
[[tutorials/wifi-firmware-update]] を新規作成 (0.4.1 → 0.6.0, updater スクリプト手順)。

- 症状: `WiFi.status() == WL_CONNECTED` なのに `localIP() == 0.0.0.0`
- 原因: ESP32-S3 USB bridge ファームウェア 0.4.1 の既知バグ
- 対処: 公式 `unor4wifi-update-windows.zip` (unor4wifi-reboot + espflash) で 0.6.0 へ更新
- WSL 固有の注意: 更新前に `usbipd detach`、更新後に再アタッチが必要

### 作成・更新

- [[tutorials/wifi-firmware-update]] — 新規作成 (tutorials/)
- [[index]] — Tutorials 表に登録

## [2026-08-12] update | WSL2 USB パススルー対応 + 実機接続確認

[[tutorials/platformio-setup]] に WSL2 での USB デバイス接続手順 (usbipd-win) を追記。

- Windows 側: `usbipd bind` → `usbipd attach --wsl` (`--auto-attach` で常時化)
- WSL 側: `/dev/ttyACM0` 確認 → `pio device list` で `UNO WiFi R4 CMSIS-DAP - TinyUSB CDC`
  (VID:PID 2341:1002, SER 64E83361C5C0) を確認
- パーミッション: `crw-rw-rw-` (666) で追加設定不要だった (Permission denied 時の対処も追記)

### 作成・更新

- [[tutorials/platformio-setup]] — WSL2 troubleshoot セクション追記 (updated: 2026-08-12)
- [[log]] — 本エントリ追記

### 次のアクション

`pio run -t upload` で実機へ書き込み → 動作確認 → 結果を本 [[log]] へ。

## [2026-08-12] note | 配線完了 (実機)

[[arduino-uno-r4-wifi|UNO R4 WiFi]] + [[28byj-48]] + [[uln2003|ULN2003]] +
電源モジュール (ELEGOO 互換品) の配線を実機で完了。
写真: https://gyazo.com/27fb73f6bdb5df4617a67237fffbccc0

- 信号線: D8–D11 → ULN2003 IN1–IN4
- 電源: 9V アダプタ → ELEGOO 電源モジュール → ブレッドボード 5V/GND
- GND 共通化済: Arduino (USB 給電) / 電源モジュール / ULN2003 / 28BYJ-48

### 次のアクション

[[overview#次のアクション]] 参照。

1. `src/secrets.h` (未作成) を新設し SSID / PASS を分離 → `.gitignore` へ追加
2. USB-C で Arduino を PC に接続し `pio run -t upload`
3. `pio device monitor` で IP を確認
4. `curl http://<IP>/status` で疎通確認
5. `POST /step?steps=2048&dir=cw` でモーター回転確認
6. 結果を本 [[log]] にフィードバック

### 作成・更新

- [[log]] — 本エントリ追記

## [2026-08-12] note | 中座 — 実機デバッグ途中 (Wi-Fi OK / モーター通電なし)

Wi-Fi HTTP API サーバは実機疎通まで完了。モーター駆動は通電確認中で一時中座。
**次回は「残課題」から再開する。**

### 進捗

- **ファームウェア**: ESP32-S3 を 0.4.1 → **0.6.0** に更新完了
  (espflash の途中停止をリトライで回避。手順: [[tutorials/wifi-firmware-update]])
- **IP 0.0.0.0 問題を解決**: DHCP 割当が `WiFi.status() == WL_CONNECTED` より遅れる
  タイミング問題だった。`src/main.cpp` に localIP が有効になるまで最大 10 秒待つ
  処理 + デバッグ出力 (status/localIP/gateway/subnet/RSSI) を追加
  → **192.168.11.3** で接続確認 (status=3, RSSI ≈ -67 dBm)
- **curl 疎通 OK**: `GET /` / `GET /status` / `POST /step` すべて 200 応答
- **実配線が D7–D4 と判明** → `src/main.cpp` のピン定数を 7,6,5,4 に変更、
  wiki 全体の配線対応表を訂正 ([[overview]], [[28byj-48]], [[uln2003]],
  [[tutorials/mb102-power-wiring]], [[tutorials/wifi-api-server]], 旧ログエントリ)

### 残課題: モーターが動かない (次回の開始点)

- 症状: ステップ実行中 4 LED が高速点滅 (全点灯に見える) + 高周波唸り音 (1 kHz)
  = シーケンスとドライバ入力は正常
- **シャフトの抵抗が電源 ON/OFF で同程度 → 保持トルクゼロ = コイルに無通電**
  (ギアボックスは正常: 手回しのギア音・抵抗は 28BYJ-48 として普通)
- 未検証のチェック項目 (優先順):
  1. ULN2003 基板 **+V と GND 間の電圧** (~5V 必須)
  2. 28BYJ-48 **赤線 (COM/中心タップ) → 5V** の接続 (基板 +V 経由 or 直接)
  3. 赤 ↔ 青/ピンク/黄/オレンジ のコイル抵抗 (~50Ω)
  4. モーター 5 ピンコネクタの接触 (挿し直し)
- 最有力仮説: **赤線 (中心タップ) への 5V 供給が切れている**
- 通電確認でき次第: `POST /step?steps=2048&dir=cw` で回転確認 → 本 [[log]] に記録

### 変更ファイル

- `src/main.cpp` — ピン D7–D4 化 + IP 待ちデバッグ出力
- [[overview]], [[28byj-48]], [[uln2003]], [[tutorials/mb102-power-wiring]],
  [[tutorials/wifi-api-server]], [[log]] — 配線対応表を実機 (D7–D4) に訂正
