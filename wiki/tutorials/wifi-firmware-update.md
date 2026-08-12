---
title: ESP32-S3 Wi-Fi ファームウェア更新 (UNO R4 WiFi)
type: tutorial
tags: [wifi, firmware, esp32, uno-r4-wifi, troubleshooting]
sources:
  - https://github.com/arduino/uno-r4-wifi-usb-bridge
  - https://support.arduino.cc/hc/en-us/articles/16379769332892
created: 2026-08-12
updated: 2026-08-12
---

# ESP32-S3 Wi-Fi ファームウェア更新 (UNO R4 WiFi)

[[arduino-uno-r4-wifi|UNO R4 WiFi]] の ESP32-S3 (USB bridge + Wi-Fi コプロセッサ)
ファームウェアを更新する手順。2026-08-12 に実機 (0.4.1 → 0.6.0) で実施。

## なぜ更新が必要か

[[arduino-uno-r4-wifi]] の Wi-Fi は ESP32-S3 上のファームウェア
("UNO R4 WiFi USB bridge") が担っている。このファームウェアが古いと
既知バグに当たる。実例 (2026-08-12):

```
Booting UNO R4 WiFi Stepper Server...
Please upgrade the firmware (current: 0.4.1)   ← 更新警告 (src/main.cpp の起動ログ)
Connecting Wi-Fi
Connected. IP: 0.0.0.0                          ← 0.4.1 の localIP() バグ
HTTP server started on port 80.
```

- **症状**: `WiFi.status() == WL_CONNECTED` なのに `WiFi.localIP()` が `0.0.0.0`
  (Wi-Fi 自体は接続できていても IP が取れず HTTP API に到達不能)
- **原因**: 古い ESP32-S3 ファームウェア (0.4.1 以前) の DHCP/localIP 周りのバグ
- **判断の正本**: WiFiS3 ライブラリの `WIFI_FIRMWARE_LATEST_VERSION`
  (今回の framework 1.6.0 では `"0.6.0"`) と `WiFi.firmwareVersion()` の比較。
  スケッチの起動ログに `Please upgrade the firmware (current: X)` と出たら更新対象
- **更新元**: https://github.com/arduino/uno-r4-wifi-usb-bridge/releases
  (本ページ作成時点の最新: **0.6.0**)

> ⚠️ 既存の OTA サンプル `WiFiFirmwareOTA` は **テスト版 (98.98.98)** を焼くので
> 本番用途では使わない。本ページの手順で正規版を入れること。

## 更新手順 (Windows + updater スクリプト)

公式 updater は `unor4wifi-reboot` (HID 経由で ESP ダウンロードモードへ自動遷移)
→ `espflash` (0x0 に S3.bin 書き込み) を自動で行う。

### WSL を使っている場合の前準備

1. **WSL からデバイスを解放** (管理者 PowerShell):

   ```powershell
   usbipd detach -b 2-1
   ```

   (手順詳細: [[tutorials/platformio-setup#WSL2 で USB デバイスが見えない (UNO R4 WiFi)]])

### updater 実行

2. リリースページから **`unor4wifi-update-windows.zip`** をダウンロード・解凍:
   https://github.com/arduino/uno-r4-wifi-usb-bridge/releases/tag/0.6.0
3. `update.bat` を実行 (SmartScreen が出たら "More info → Run anyway")
4. **COM ポートの選択**を求められたら、UNO R4 WiFi の COM を選ぶ
   - ダウンロードモード中は Windows には "Espressif USB JTAG/serial debug unit" 扱い
     ではなく **generic な COMx** としか見えない (README 注記) ため、
     **Device Manager → Ports** で USB シリアルデバイスを確認して選択する
5. 完了したら **USB を抜き差し**して通常ファームウェアに戻す

### WSL に戻す

6. 再アタッチ (管理者 PowerShell):

   ```powershell
   usbipd attach --wsl -b 2-1
   ```

   > 更新中にデバイスが再列挙されるため、バス ID が変わった場合は
   > `usbipd list` で確認し `usbipd bind` → `attach` し直す。

7. スケッチを再アップロード (`pio run -t upload`) → RESET → 起動ログで確認:

   ```
   Connected. IP: 192.168.x.x   ← 0.0.0.0 でない実 IP
   ```
   (`Please upgrade the firmware` 警告が出なければ 0.6.0 適用済み)

## トラブルシュート

### `Cannot put the board in ESP mode. (via 'unor4wifi-reboot')`

- **Option 1**: USB を抜き差しして `update.bat` を再実行
- **Option 2**: espflash を直接使う手動方法 (公式サポート記事):
  https://support.arduino.cc/hc/en-us/articles/16379769332892
  (ESP ダウンロードモードは ESP_BOOT と GND を短絡して電源投入でも入れる)

### 書き込みが途中で止まる / `Timeout while running FlashDeflateData command`

**実績 (2026-08-12)**: チップ認識まで成功して書き込み中に timeout。
**単純なリトライの繰り返しで最終的に成功した** (3〜4 回程度失敗後に成功)。

- ケーブルを PC 直差しにする (ハブ経由は不安定)
- **リトライの要領**: 失敗後は USB を抜き差しせずに (ダウンロードモードのまま)
  `bin\espflash.exe write-bin -b 115200 0x0 firmware\UNOR4-WIFI-S3-0.6.0.bin`
  を直接再実行する方が早い (モードに入り直す手間が省ける)
- それでポートが消えたら抜き差しして通常モードに戻し、`update.bat` を最初から

### updater スクリプトが動作しない環境の場合

手動でも可能: `S3.bin` (リリース同梱の単一バイナリ) を
`espflash write-bin -b 115200 0x0 S3.bin` で 0x0 に書き込む。

## 関連ページ

- [[arduino-uno-r4-wifi]] — コントローラ (ESP32-S3 の役割)
- [[api/wifis3-library]] — `WiFi.firmwareVersion()`, `WIFI_FIRMWARE_LATEST_VERSION`
- [[tutorials/platformio-setup]] — WSL2 USB パススルー・ビルド環境
- [[tutorials/wifi-api-server]] — 本トラブルが発覚した MVP 実装