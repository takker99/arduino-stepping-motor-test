---
title: ESP32-S3-MINI-1-N8
type: concept
tags: [esp32, wifi, bluetooth, mcu, co-processor]
sources:
  - raw/arduino-uno-r4-wifi/datasheet.md
  - raw/arduino-uno-r4-wifi/user-manual.md
  - raw/arduino-uno-r4-wifi/esp32-s3-mini-1_mini-1u_datasheet_en.pdf
created: 2026-08-10
updated: 2026-08-10
---

# ESP32-S3-MINI-1-N8

[[arduino-uno-r4-wifi|UNO R4 WiFi]] に搭載されている **Wi-Fi® / Bluetooth® LE コプロセッサ**。
Espressif Systems 製。UNO R4 WiFi では RA4M1 の "companion chip" として機能し、
通常はユーザが直接触らない (ファームウェア書き換えは上級者向け)。

## 主要スペック

| 項目 | 値 |
| --- | --- |
| コア | Xtensa® LX7 32-bit **デュアルコア** |
| ROM | 384 kB |
| SRAM | 512 kB |
| 動作電圧 | **3.3 V** |
| クロック | 40 MHz クリスタル |
| Wi-Fi | 802.11 b/g/n (Wi-Fi 4) 2.4 GHz, 最大 150 Mbps |
| Bluetooth | Bluetooth® 5 LE 最大 2 Mbps |
| アンテナ | 基板内蔵 trace-antenna (Wi-Fi と BT で共有、排他使用) |
| 内蔵フラッシュ (N8 区分) | 8 MB (Quad SPI) |

## 電圧系の注意

ESP32-S3 は **3.3 V 動作**。RA4M1 は 5 V 動作。
両者間の通信線は [[arduino-uno-r4-wifi|UNO R4 WiFi]] 基板上の **TXB0108DQSR**
(5 V ↔ 3.3 V 双方向レベル変換) を介して接続されている。

> ⚠️ ESP ヘッダ (`ESP_IO42`, `ESP_IO41`, `ESP_TXD0`, `ESP_RXD0`, `ESP_DOWNLOAD`)
> は 3.3 V 系。5 V 信号を接続しないこと。

## UNO R4 WiFi での役割

### 1. USB-Serial bridge (デフォルト)

UNO R4 WiFi の USB-C はデフォルトで ESP32-S3 を経由して RA4M1 に到達する。
ESP32-S3 には "Serial bridge" ファームウェアが書き込まれており、
パソコンから見ると RA4M1 が直接繋がっているように見える。
(PlatformIO から `pio run -t upload` する操作は透過的。)

### 2. Wi-Fi / Bluetooth 無線機能

`WiFiS3` ライブラリ ([[api/wifis3-library]]) 経由で RA4M1 から Wi-Fi 接続を制御する。
本プロジェクトの [[tutorials/wifi-api-server|API server]] は
RA4M1 上で動く HTTP サーバが WiFiS3 を呼び、ESP32-S3 が物理層を担う構成。

### 3. (上級者向け) 直接プログラミング

ESP ヘッダから ESP32-S3 に直接アクセスすることも可能だが、
**デフォルトファームウェアを上書きすると UNO R4 WiFi としての USB 書き込みや
Wi-Fi 機能に支障が出る**。Arduino の [Help Center 記事](https://support.arduino.cc/hc/en-us/articles/9670986058780-Update-the-connectivity-module-firmware-on-UNO-R4-WiFi) に
復旧手順あり。

## アンテナの共有

ESP32-S3 の trace-antenna は **Wi-Fi と Bluetooth で物理共有**。
両者は **同時に使用できない** (同時 ON すると一方を OFF する)。
本プロジェクトでは Wi-Fi のみ使用するため問題なし。

## 関連ページ

- [[arduino-uno-r4-wifi]] — 本モジュールを搭載するボード
- [[api/wifis3-library]] — WiFiS3 ライブラリ API
- [[sources/esp32-s3-mini-1-datasheet]] — 公式 datasheet サマリ
