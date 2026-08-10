# AGENTS.md — Wiki Schema

このドキュメントは LLM が wiki を保守・運営するときの **スキーマ（規約）** である。
`llm-wiki.md` のパターン（三層: raw / wiki / schema）をこのプロジェクト向けに具体化する。

> **目的（再掲）**: Arduino UNO R4 Minima でステッピングモーター（28BYJ-48 + ULN2003）を
> 動かすスケッチを書くための知識を蓄積する。wiki は raw のラッパーであり、
> 「スケッチを書く」行為の判断材料を即座に引き出せる形に保つ。

---

## 三層の責務

| 層 | パス | 誰が触る | 役割 |
| --- | --- | --- | --- |
| **Raw** | `raw/` | 人間のみ（LLM は読み取り専用） | 一次資料の原典。不変 |
| **Wiki** | `wiki/` | LLM が作成・更新 | 構造化されたMarkdown群。cross-reference, 矛盾フラグ, 統合済みknowledge |
| **Schema** | `wiki/AGENTS.md` | 人間とLLMが共進化 | このファイル。wikiの運用ルール |

`.gitignore` で `raw/*` を除外しているので raw は repo に commit されない。
wiki のみが git の管理対象。

---

## ディレクトリ構造

```
wiki/
├── AGENTS.md             # このファイル (schema)
├── overview.md           # プロジェクト俯瞰・current state
├── index.md              # 全ページのカタログ（content-oriented）
├── log.md                # 時系列ログ（chronological, append-only）
│
├── concepts/             # 概念・部品ページ（エンティティ単位）
│   ├── arduino-uno-r4-minima.md
│   ├── ra4m1.md
│   ├── stepper-motor.md
│   ├── unipolar-vs-bipolar.md
│   ├── 28byj-48.md
│   ├── uln2003.md
│   └── ...
│
├── sources/              # 一次資料ごとのサマリ
│   ├── arduino-uno-r4-minima-datasheet.md
│   ├── ra4m1-datasheet.md
│   ├── 28byj-48-datasheet.md
│   ├── uln2003a-datasheet.md
│   └── ...
│
├── tutorials/            # Arduino 公式チュートリアルの要約
│   ├── unipolar-stepper-motor.md
│   └── stepper-library-examples.md
│
└── api/                  # Arduino 言語・ライブラリ API リファレンス
    ├── pinMode.md
    ├── digitalWrite.md
    ├── delay.md
    └── stepper-library.md
```

### 使い分けの基準

- **concepts/** — 「部品/物理現象/分類軸」など、ページがそれ自体で独立して読める単位。
  モーターの構造、ユニポーラ/バイポーラの違い、駆動方式の選択基準など。
- **sources/** — ある1つの一次資料を要約したページ。出典・参照ファイル名を必ず残す。
- **tutorials/** — 公式チュートリアルの統合版。複数ページにまたがるrawを統合して1つの
  ハウツーに再構成したもの。
- **api/** — `pinMode()`, `digitalWrite()`, `delay()`, `Stepper.step()` など、
  スケッチを書く際にシグネチャ・戻り値・注意点を即座に引き出したいもの。

---

## ページ命名規則

- ファイル名は小文字 + ハイフン区切り（kebab-case）
- 固有名詞はそのまま使用可: `28byj-48.md`, `ra4m1.md`, `uln2003.md`（数字を含めて小文字化）
- スペース・アンダースコアは使わない
- 拡張子は `.md`

## ページ内フォーマット

各ページは frontmatter（YAML）を持つ:

```markdown
---
title: 28BYJ-48 ステッピングモーター
type: concept   # concept | source | tutorial | api
tags: [stepper, hardware, 28byj-48]
sources:
  - raw/28byj-48/step-motor-5v-28byj48-datasheet.pdf
created: 2026-08-10
updated: 2026-08-10
---

# 28BYJ-48

本文...
```

最低限 `title` と `type` は必須。`sources` は raw を参照するときに書く。

本文は **Obsidian リンク**（`[[...]]`）で他のページにクロスリファレンスする。
wiki内リンクは絶対パスではなくファイル名のみ: `[[28byj-48]]`, `[[uln2003]]` のように書く。

---

## Ingest ワークフロー

新しい raw を追加したとき（または最初から backlog を処理するとき）:

1. **まず raw を読む**。種類によって読み方が異なる:
   - **Markdown / AsciiDoc**: そのまま全文読む。
   - **PDF**: 二段階で読む（後述）。
2. **内容を議論する箇所があれば人間に確認**。不明点・複数の解釈がある場合は
   勝手に決め打ちせず質問する。
3. **既存 wiki を確認する**。`index.md` を見て、関連ページが既にあるか確認。
4. **ページを書く**:
   - 新しい概念・部品 → `concepts/` に新規ページ
   - 新しい一次資料 → `sources/` にサマリページ（concepts へリンク）
   - 既存概念の追記・修正 → 既存ページを edit
5. **cross-reference を貼る**: 関連ページへ `[[...]]` リンクを追加。
6. **index.md を更新**: 新しいページ・更新を反映。
7. **log.md に追記**: `## [YYYY-MM-DD] ingest | <タイトル>` の形式で1行目を始める。

### 1 ingest = 1 ログエントリ（原則）

ただし複数の raw をまとめて 1 セッションで処理する場合は 1 エントリにまとめてよい。
その場合はログの本文にサブ項目（"- ABX00080 datasheet", "- 28BYJ-48 datasheet"）を
箇条書きで残す。

---

## PDF 二段階読解ルール

PDF は **必ず二段階** で読む:

### 段階1: pdftotext でテキスト抽出

```bash
pdftotext -layout raw/<path>/foo.pdf /tmp/foo.txt
```

抽出したテキストを Read ツールで全文読む。図表参照部分・座標指定が必要な
箇所（ピン配置、特性グラフ、タイミングチャートなど）が含まれている箇所を
リストアップする。

### 段階2: 必要なページだけ pdftoppm で画像化

```bash
pdftoppm -r 150 -f <N> -l <N> raw/<path>/foo.pdf /tmp/foo-page -png
```

`-r 150` 程度（dpi）で十分読める。段階1で「これは図を見ないと判断できない」
と判断したページだけ `-f` / `-l` で範囲指定して画像化し、Read ツールで読む。

### 全ページ画像化は禁止

データシートは数十〜数百ページのことがあるので、闇雲に全ページ画像化
するとコンテキストを浪費する。段階1のテキストを読んでから、本当に図が
必要なページだけを抜粋する。

### どの図が必要になりやすいか

このプロジェクトの文脈で図が必要になる典型例:

- ピン配置図 / ピン番号 → ABX00080-full-pinout.pdf, uln2003a pinout, 28byj-48 wiring
- 回路図 → ABX00080-schematics.pdf（ブロック図レベルで十分）
- タイミングチャート → uln2003a（相励磁シーケンスの解釈）
- 等価回路 → uln2003a（ダーリントン段の動作）

---

## Query ワークフロー

ユーザーから質問が来たとき:

1. **index.md を読む** → 関連ページを見つける
2. 関連ページを読む
3. 必要なら raw を読み直す（矛盾解決・根拠確認）
4. 回答する。回答はwiki からも参照できるよう `[[...]]` リンク付きで出力する
5. 回答自体がwikiに追加する価値を持つ（比較表、解析、コード片）場合は
   該当ページを作る/更新する

---

## Lint ワークフロー（任意のタイミング）

ユーザーから指示があったとき、またはLLMが自発的に提案してよい:

- 矛盾チェック: 同じ事実について複数ページで食い違いがないか
- 孤児ページ: index.md に登録されているがどこからもリンクされていない
- 古くなった記述: より新しい raw で上書きされた事実
- 不足概念: index には載っていないが raw で頻出する語
- 引用リンク切れ: `raw/...` へのパスが実在するか

問題を見つけたら、修正提案してから適用するか、ユーザーに確認する。

---

## 「スケッチを書く」目的のための追加ルール

この wiki の最終目的は Arduino スケッチを書くこと。よって:

- **配線情報** は `concepts/28byj-48.md` と `concepts/uln2003.md` に必ず
  「Arduino UNO R4 → ULN2003 driver board → 28BYJ-48」の対応表として残す。
- **使用上の注意**（電源、電流、GPIO電流容量、GND共通）は余白でも残す。
- **スケッチの雛形**（コード片）は tutorials/ に置く。完成版スケッチを
 ここに保存するかどうかは別途相談する（プロジェクトルートに `src/` を
 設けるか wiki に閉じるかは未決定 — ユーザーと相談）。

---

## 命名・用語の固定

混乱を避けるため、wiki内では以下の表記を統一する:

| 用語 | wikiでの表記 |
| --- | --- |
| Arduino UNO R4 Minima | "UNO R4 Minima" または "R4 Minima"（文脈で判断） |
| 28BYJ-48 | "28BYJ-48"（ハイフン含めて固有名詞） |
| ULN2003AN / ULN2003A | "ULN2003"（チップそのもの）または "ULN2003AN"（TI品番） |
| RA4M1 (R7FA4M1AB3CFM) | "RA4M1"（シリーズ名）または "R7FA4M1AB3CFM"（品番） |
| ユニポーラ | "unipolar" または "ユニポーラ"（ページごとに初出時に統一） |

---

## このファイル自体の更新

AGENTS.md は固定ではない。運用しながら改善する。
**変更前にユーザーに確認する**こと（schema の合意なき改変は混乱の元）。
