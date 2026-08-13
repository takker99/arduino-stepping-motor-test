# Log — 時系列ログ

wiki の **append-only** な活動履歴。
各エントリは `## [YYYY-MM-DD] <種別> | <タイトル>` で始める (grep で抽出可能)。
種別: `ingest` / `query` / `lint` / `update` / `note` 等。

最終更新: 2026-08-13

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
- **LED の観測 (2026-08-12 訂正版)**:
  - **A・C は MB102 電源 ON で常時点灯** = 入力 HIGH (Stepper ライブラリの
    最終相ホールド, 正常な状態)。2048 step ≡ 0 (mod 4) → 相 0 = A+C
  - **手回し時のみ B・D も点灯** (光度は回転速度依存) = 回転発電の EMF が
    入力に逆流して LED が光る = **B・D コイルの閉回路を実証** (断線・未挿入なら光らない)
- **重要: この観測は MB102 の 5V 出力を証明しない**
  - A・C の LED は Arduino GPIO (5V) が駆動 → MB102 は無関係
  - B・D の EMF ループは ON 中の A/C トランジスタ経由でも閉じる → 赤線が
    どの電源にも繋がっていなくても成立するため、横断的な観測では
    赤線の 5V 供給・MB102 の出力を確定できない
- **仮説 (優先順, 2026-08-12 更新)**:
  1. **MB102 の 5V 出力が出ていない** (ジャンパ OFF / 9V アダプタ未接続・弱い)
  2. **A か C コイルの断線** (青 or 黄)
  3. 赤線 (COM) の 5V 未接続
- **次回の最初のアクション**: MB102 電源 ON で **ULN2003 基板の +V ↔ GND 間の
  電圧を測定** → 0V なら原因確定 (MB102 側を追う) / ~5V なら次のチェック
  (赤 ↔ 青, 赤 ↔ 黄 のコイル抵抗 ~50Ω)
- 通電確認でき次第: `POST /step?steps=2048&dir=cw` で回転確認 → 本 [[log]] に記録

### 変更ファイル

- `src/main.cpp` — ピン D7–D4 化 + IP 待ちデバッグ出力
- [[overview]], [[28byj-48]], [[uln2003]], [[tutorials/mb102-power-wiring]],
  [[tutorials/wifi-api-server]], [[log]] — 配線対応表を実機 (D7–D4) に訂正

## [2026-08-13] note | ULN2003 は DIP 化基板 + 5V 供給パス確認完了

実機の ULN2003 は標準の「ULN2003 driver board (端子台 + LED 付き)」ではなく、
**ULN2003AN (PDIP-16) を汎用ブレッドボード基板に直付けした DIP 化基板** であることを確認。
wiki のドライバーボード前提の記述を DIP 基板対応に修正。

### 進捗 (仮説の検証)

- **測定**: テスター (DC 電圧) で **E (pin 8) ↔ モーター赤線 (COM) 間 = ~5V 確認**
- 意味: **MB102 → 赤線 (COM) への 5V 供給パスは正常**
- **仮説 1「MB102 の 5V 出力が出ていない」は否定**
  (MB102 の出力自体はユーザー確認済み、かつ赤線までの配線も通電確認済み)
- 残る仮説 (優先順): コイル断線 (A or C) > その他

### 次のアクション (電源 OFF で実施、Ω レンジ)

1. **コイル抵抗チェック** (28BYJ-48 データシート: 1 相 50 Ω ±7%):
   - 赤 ↔ 青 ≈ 50Ω / 赤 ↔ 黄 ≈ 50Ω / 赤 ↔ ピンク ≈ 50Ω / 赤 ↔ オレンジ ≈ 50Ω
     (全部 ∞ なら断線 = 仮説 2 確定)
   - ついでに: 青 ↔ オレンジ ≈ 100Ω (コイル 1 全長)、ピンク ↔ 黄 ≈ 100Ω
     (コイル 2 全長)、青 ↔ ピンク = ∞ (別コイル、通導しないのが正常)
2. **pin 9 (COM) の +5V 接続確認**: DIP 手配線はドライバーボードと違って基板
   パターンが無いので、pin 9 が +5V に繋がっているか目視 + 導通チェック
   (未接続でも回転の直接原因にはならないが、データシート上は必須接続)
3. コイル正常なら通電確認 (`POST /step?steps=2048&dir=cw`) → 結果を本 [[log]] へ

### 作成・更新

- [[uln2003]] — 「ドライバーボードについて」節に DIP 化基板の追記
- [[28byj-48]] — 配線表に DIP 基板ピン対応を追記
- [[overview]] — 次のアクションを現状に更新
- [[log]] — 本エントリ追記

## [2026-08-13] note | コイル抵抗チェック: 23Ω = 正常 (仮説 2 棄却)

電源 OFF、Ω レンジでモーター線のコイル抵抗を測定。

- **測定結果**: 赤 (COM) ↔ 青 ≈ 23Ω / 赤 ↔ 黄 ≈ 23Ω
- **判定: 正常**。28BYJ-48 の実測値は赤↔各相 22–24Ω が典型
  (NYU ITP PhysComp の測定ガイド。データシートの 50Ω は公称/フルコイル相当で、
  実機は半分程度の値になるのが普通。12V 版は 200Ω)
- 赤↔青、赤↔黄は**別コイル** (コイル 1 とコイル 2) → **両コイルとも通導・対称**
  = **コイル断線 (仮説 2) は否定**
- 5V 供給パス確認済み + コイル正常 → 残る容疑は**ドライバ側**:
  モーター線の接続先ミス / E (pin 8) の GND 浮き / チップ不良

### 次のアクション

1. (任意, コイルペアの確定) 赤↔ピンク・赤↔オレンジ ≈ 23Ω、フルコイル
   (青↔オレンジ or 青↔黄 ≈ 46Ω) でこのモーターのコイルペアを確認
2. **通電判別テスト** (MB102 ON → `POST /step` 実行後のホールド状態で
   各相ワイヤ ↔ GND を DC 電圧測定):
   - **正配線** (出力 16–13 に接続): アクティブ 2 相 ≈ **1V** (V_CE(sat)),
     残り 2 相 ≈ **5V**
   - **誤配線** (入力 1–4 に接続): 2 本 ≈ 5V (HIGH), 2 本 ≈ 0V (LOW)
   - 全部 ≈ 5V で変化なし: E (pin 8) の GND 接続不良 or チップ故障
3. 判定に応じて修正 → `POST /step?steps=2048&dir=cw` で回転確認 → 本 [[log]] へ

### 作成・更新

- [[28byj-48]] — 実測コイル抵抗 (23Ω) とデータシート 50Ω の解釈を追記
- [[log]] — 本エントリ追記

## [2026-08-13] note | 診断: 電気系は完全に正常 — 原因は起動周波数超過 (失歩スタール)

通電判別テストの結果、電気系の正常性が確定。回らない原因は速度設定と判明。

### 通電判別テスト (POST /step 後ホールド状態)

- **青・黄 ≈ 1V** (E 基準 0.9V / レール基準 1.2V) = アクティブ相。
  V_CE(sat) ≈ 0.9–1.1V と一致 → トランジスタ導通 + コイル励磁 (~170 mA)
- **ピンク・オレンジ ≈ 3.7V / 4V** = 非励磁相 (コイル経由で 5V に引かれる)
- 判定: **正配線 (出力 16–13) + A+C ホールド**。電源経路 / コイル / ドライバ /
  シーケンスは全部正常
- GND 基準差 0.3V: E がレールより浮くのはモーター電流 ~350 mA による共通 GND
  経路の電圧降下 (ground bounce)。正常範囲

### 原因の再特定

- **8/12 の「保持トルクゼロ」は誤診の可能性大**: ギアボックス摩擦
  (600–1200 gf·cm) が保持トルク (~350 gf·cm) を上回るため手回しでは区別不能
  (または当時 COM 未接続)
- **真因: 起動周波数超過**:
  - `setSpeed(15)` → 4096 steps/rev → **1024 steps/s ≈ 1kHz** ← 唸り音の正体
  - 28BYJ-48 の pull-in (起動可能) 周波数は **>600Hz** → 1024 steps/s では
    起動時に失歩してその場で振動するだけ (失歩スタール)
- 対処: `setSpeed(15)` → **`setSpeed(5)`** (341 steps/s < 600Hz) に変更
  (`src/main.cpp`, 2026-08-13)
- 将来課題 (継続): `/step` に speed パラメータ追加 or 加速度ランプ
  ([[tutorials/wifi-api-design-notes]] の将来課題)

### 次のアクション

1. `pio run -t upload` + `pio device monitor` (ユーザー操作)
2. `POST /step?steps=2048&dir=cw` で回転確認 (5 rpm = 約 24 秒)
3. 結果を本 [[log]] へ

### 作成・更新

- `src/main.cpp` — `setSpeed(5)` に変更
- [[log]] — 本エントリ追記

## [2026-08-13] note | setSpeed(5) でも回転せず → /step に speed パラメータ追加

`setSpeed(5)` (341 steps/s) でもモーターは回転しなかった。
音は低くなり (ステップ周波数に一致) 音量も減った = ステップは低速で打たれているが
依然失歩スタール。電気系は正常確認済みなので、速度をさらに下げて検証するため
**`/step` に `speed` パラメータ (rpm) を追加** した (再フラッシュなしで速度変更可)。

### 変更 (`src/main.cpp`)

- `POST /step?steps=N&dir=cw|ccw&speed=RPM` — speed 1–60 rpm (省略時は現在値維持、
  デフォルト 5)。`setSpeed()` を実行してからステップ実行
- `/status` と `/step` レスポンスに `"speed": N` を追加
- ビルド成功 (RAM 16.0% / Flash 22.3%)

### 検証の背景

- 341 steps/s は pull-in (>600Hz) 未満のはずだが回らない → 実機の起動可能周波数は
  データシートより低い (ギアトレイン負荷 + 摩擦 600–1200 gf·cm の影響)
- **`STEPS_PER_REV=4096` のキャリブレーション問題**: Arduino Stepper ライブラリの
  4 相 2 相励磁シーケンスは 1 回転 = **2048 step** (内部 32 step × 減速 64)。
  4096 設定だと実速度は rpm 表示の **2 倍** になっている (起動がより困難になる)。
  正しい値は 2048 ([[28byj-48]] の未解決事項を実機観測で確定)

### 次のアクション

1. `pio run -t upload` (ユーザー操作)
2. `POST /step?steps=512&dir=cw&speed=1` (68 steps/s, 実 2 rpm) で起動確認。
   回らないなら `speed=2` で 136 steps/s → それでも駄目ならギアボックス側の
   機械的故障を疑う (シャフトの手回しでギア欠け・固着の再確認)
3. 回ったら 2048 steps で通し確認 → 結果を本 [[log]] へ

### 作成・更新

- `src/main.cpp` — speed パラメータ追加 (1–60 rpm, デフォルト 5)
- [[tutorials/wifi-api-server]] — エンドポイント表・レスポンス例・curl 例を更新
- [[log]] — 本エントリ追記

## [2026-08-13] ingest | StupidDog 記事 (相順問題) — 真因が判明

ユーザー提供の記事 <https://stupiddog.jp/note/archives/1235> を ingest。
**これまでの「速度が速すぎる (失歩スタール)」診断は誤り。真因は相順 (phase order)。**

### 記事の要点 (実機検証済み)

- Stepper ライブラリの 4 ピン構成シーケンス (Stepper.cpp):
  `1010 → 0110 → 0101 → 1001` は **バイファイラ巻きユニポーラ駆動** を想定
- 28BYJ-48 の 4 相を**色順のまま** (Blue, Pink, Yellow, Orange) C0–C3 に渡すと
  **回転磁界にならず振動するだけ** ← 本プロジェクトの実機症状と完全一致
- 対処: **コンストラクタの第 2・3 引数 (IN2/IN3) を入れ替える**
  - 記事: `Stepper(2048, Blue, Yellow, Pink, Orange)`
  - 定番例 `Stepper(2048, 8, 10, 9, 11)` も同じ入れ替え (IN1, IN3, IN2, IN4)
- steps/rev の正しい値は **2048** (記事のスケッチも 2048)

### 診断の訂正

- 8/13 の「setSpeed(5) でも回らない = 起動周波数超過」は**原因を取り違えていた**
- 実際は: 5 rpm で回らない = 振動周波数がステップ周波数に一致していたのは
  相順不一致の振動。速度は無関係だった (速度を下げても相順は直らない)
- 電気系・コイル・電源・配線の確認結果は有効 (診断過程として正しい)

### コード修正 (`src/main.cpp`)

```cpp
const int STEPS_PER_REV = 2048;   // 4096 → 2048 (1 回転の正しいステップ数)
Stepper myStepper(STEPS_PER_REV, PIN_IN1, PIN_IN3, PIN_IN2, PIN_IN4);  // 第2・3引数入れ替え
```

### 作成・更新

- [[sources/stupiddog-28byj48-tutorial]] — **新規** (Web 記事のサマリ)
- [[28byj-48]] — 2048 訂正 + 相順問題の追記
- [[api/stepper-library]] — シーケンス表に相順の罠の注記 + 2048 訂正
- [[tutorials/stepper-library-examples]] — 改造ポイント・トラブルシュートを訂正
- [[index]] — 新ページ登録 + 28byj-48 の要約訂正
- [[log]] — 本エントリ追記

### 次のアクション

1. `src/main.cpp` の修正を `pio run -t upload` で反映 (ユーザー操作)
2. `POST /step?steps=2048&dir=cw` (speed=5 のままでも回転するはず) で確認
3. 結果を本 [[log]] へ

## [2026-08-13] note | 回転成功 — 相順修正で解決

`pio run -t upload` で修正版を反映 → **`POST /step?steps=2048&dir=cw` で
1 回転を確認 (ユーザー実機)**。相順 (第 2・3 引数入れ替え) が真因と確定。

- 停止後の LED: **A+B (IN1+IN2 = Blue+Pink) 点灯のまま** = 正常な 2 相ホールド
  - 2048 step ≡ 0 (mod 4) → シーケンス step 0 = コンストラクタ 1・3 引数
    = D7+D6 (IN1+IN2) = Blue+Pink。相順修正後の正しい停止位置
  - 旧配線 (修正前) は A+C (IN1+IN3) が点灯していたが、これは相順不一致の
    シーケンス上の位置で、回転しない原因と対応していた
- 以降、一連のデバッグは完結: 電源経路 → コイル → ドライバ → シーケンス
  → **相順** まで全項目確認済み

### 注意 (ホールド中の消費電流)

Stepper ライブラリは `step()` 完了後も最後の 2 相を**励磁し続ける**
(停止位置の保持トルク維持)。実機では ~350 mA 流れっぱなしになり、
モーターがじわじわ発熱する。長期間の放置・連続運用では
**`/stop` の通電遮断化** (全ピン LOW) が望ましい
(現状 MVP の `/stop` は no-op。[[tutorials/wifi-api-design-notes]] の将来課題)。

### 作成・更新

- [[log]] — 本エントリ追記
- (該当すれば [[overview#次のアクション]] も更新)

### 次のアクション

1. (任意) 連続動作確認: `dir=ccw` 逆回転 / 複数回転 / 角度指定 (steps=512 = 90°)
2. `/stop` の通電遮断化 (全ピン LOW) — 常時通電の回避 (要コード変更)
3. 完了したら本件のデバッグを [[overview]] に反映して一区切り

## [2026-08-13] update | `/stop` を通電遮断 (全ピン LOW) に実装

ホールド中の常時通電 (~350 mA, 発熱) を回避するため `/stop` を実装。

- **変更前**: MVP の no-op (200 を返すだけ)
- **変更後**: IDLE 状態なら 4 ピン (D7–D4) をすべて LOW にし、保持トルクを解除
  - RUNNING 中は 409 (busy) — `step()` がブロックするため実行中の中断は不可
    (ノンブロッキング化は [[tutorials/wifi-api-design-notes]] の将来課題)
  - 次の `/step` で Stepper ライブラリがシーケンスを書き直すので再励磁 OK
- ビルド成功 (RAM 16.0% / Flash 22.3%)

### 作成・更新

- `src/main.cpp` — `/stop` 実装 (全ピン LOW)
- [[tutorials/wifi-api-server]] — エンドポイント表・スケッチ・既知の制約を更新
- [[tutorials/wifi-api-design-notes]] — ノンブロッキング化の動機を訂正
- [[overview]] — API 表・次のアクションを更新
- [[log]] — 本エントリ追記

## [2026-08-13] update | `GET /` で OpenAPI JSON を返すように

API の全体像 (エンドポイント・パラメータ・レスポンス) を手元で確認できるよう、
**`GET /` が OpenAPI 3.0.3 の JSON を返す**ように変更 (旧: HTML ヘルプ)。

- 構成: `kOpenApiHead` + `WiFi.localIP()` + `kOpenApiTail` を連結 (Flash 常駐の
  静的文字列 + 動的 IP。RAM 消費は String 1 本分のみ)
- 収録内容: `paths` = `/`, `/status`, `/step` (steps/dir/speed パラメータ),
  `/stop`。`components/schemas` = `Status`, `StepResult`
- `servers[0].url` には現在の IP が自動で入る → Swagger UI / Redoc / Postman
  に貼れば API ブラウザとして使える
- HTML ヘルプは `/index.html` に移動
- JSON の妥当性は Python で検証済み (パース成功, 4 パス + 2 スキーマ)
- ビルド成功 (RAM 16.0% / Flash 23.6%)

### 作成・更新

- `src/main.cpp` — OpenAPI ドキュメント追加 (`GET /`), `/index.html` 分離
- [[tutorials/wifi-api-server]] — エンドポイント表・レスポンス例・curl 例を更新
- [[overview]] — API 表を更新
- [[log]] — 本エントリ追記

## [2026-08-13] note | 動作速度レンジ確認 (実機)

`speed` パラメータで各 rpm を実機確認した結果:

| rpm | steps/s | 結果 |
| --- | --- | --- |
| 1, 5, 10, 15 | 68 / 170 / 341 / 512 | **回転 OK** |
| 20, 30, 60 | 682 / 1024 / 2048 | 唸るだけで回らない (失歩スタール) |

- 起動限界は **15 rpm (512 steps/s) と 20 rpm (682 steps/s) の間**。
  データシートの pull-in ">600Hz" とほぼ一致 (実機はギアボックス負荷で
  少し低めに出る)
- 1 rpm は 1 回転 60 秒と非常に遅いが正常に回転する (最初の確認漏れは
  観察時間不足で、回らない原因は無し)
- 20 rpm 以上を回すには加速度ランプが必要 ([[tutorials/wifi-api-design-notes]]
  の将来課題)。実用上は **1–15 rpm が使用レンジ**
- ギアボックス損傷の疑いは無し (全速度で回転・音とも正常)

## [2026-08-13] docs | wiki 全体の整合性整理 (2048 step/rev / D7–D4 に統一)

実機検証の結果確定した値 (STEPS_PER_REV=2048, D7–D4, 相順入れ替え) を
wiki 全体に反映し、残存していた旧記述を整理した。

- **[[tutorials/wifi-api-server]]**: 埋め込みの完成版スケッチ (旧 D8–D11 /
  4096 / `openApiDoc()` 未定義参照など src/main.cpp とドリフト) を削除し、
  骨格 + `src/main.cpp` へのポインタに置換。`GET /` は OpenAPI JSON なので
  HTML ヘルプは `/index.html` と明記
- **[[api/stepper-library]]**: `stepper_oneRevolution` サンプルを 2048 +
  第 2・3 引数入れ替えに修正 (旧 4096 のままでは動作しない記述だった)。
  MotorKnob / oneStepAtATime のコメントも 2048 に
- **[[api/pinMode]]**: 例のピンを D8–D11 → D7–D4 (実機配線)
- **[[arduino-uno-r4-wifi]]**: 接続ピンの記述を D7–D4 に更新
- **[[overview]]**: モーター仕様の 4096 → 2048 step/rev
- **[[api/delay]]**: 2048 step/rev での s/rev・rpm 換算値に更新
- **[[wifi-api-design-notes]]**: degrees 換算式を /4096 → /2048 に修正
- **[[stepper-library-examples]]**: 改造ポイントのピンを D7–D4 に更新
- **[[sources/28byj-48-datasheet]]**: データシートの 4096 step は half-step
  換算である旨を明記 (ライブラリ用は 2048)
- **[[AGENTS]]**: 正本に OpenAPI JSON を追加、完成版スケッチの wiki 内
  完全コピー禁止 (ドリフト防止) を明文化
