# MODBUS レジスタ表

本表は、現行ファームウェア実装 (`Core/Src/modbus_reg.c`) で実際にアクセスされているレジスタを中心に整理したものです。

## 1. 通信仕様（実装ベース）

- Read Holding Registers: `0x03`
- Write Single Register: `0x06`
- Write Multiple Registers: `0x10`
- 32bit 値は **2レジスタ構成（上位16bit -> 下位16bit）**
  - 例: `0x0900` が上位、`0x0901` が下位

## 2. 動作コマンド

| アドレス | 名前 | R/W | 内容 | 備考 |
|---|---|---|---|---|
| `0x0000` | `CMD_ADDR_OPERATION` | W | 動作指令 | `0x06` で書込 |

### コマンドデータ一覧（`0x0000` へ書込）

| データ | 意味 |
|---|---|
| `0x0300` | 積算電力量ゼロリセット |
| `0x0400` | 計測モードへ移行 |
| `0x0700` | 設定モードへ移行 |
| `0x0800` | 計測履歴初期化 |
| `0x0901` | 設定値初期化 |
| `0x0903` | 全初期化 |
| `0x0904` | 警報履歴初期化 |
| `0x1000` | 瞬低ログ読出し（先頭へ） |
| `0x1001` | 瞬低ログ読出し（進める） |
| `0x1002` | 瞬低ログ読出し（消去して進める） |
| `0x1200` | 最大値リセット |
| `0x1300` | 最小値リセット |
| `0x9900` | ソフトリセット |

## 3. 瞬時値（Read Only）

| 先頭アドレス | 名前 | R/W | スケーリング | 備考 |
|---|---|---|---|---|
| `0x0000` | 電圧1 (`REG_INST_VOLTAGE_1`) | R | 実測値 × 10 | 32bit（`0x0000/0x0001`） |
| `0x0002` | 電圧2 (`REG_INST_VOLTAGE_2`) | R | 実測値 × 10 | 32bit |
| `0x0004` | 電圧3 (`REG_INST_VOLTAGE_3`) | R | 実測値 × 10 | 32bit |
| `0x0034` | 周波数1 (`REG_INST_FREQUENCY_1`) | R | 実測値 × 10 | 32bit |
| `0x0058` | 温度1 (`REG_INST_TEMPERATURE_1`) | R | 実測値 × 10 | 32bit |
| `0x0068` | 漏電1 (`REG_INST_LEAKAGE_1`) | R | 整数値 | 32bit |
| `0x006A` | 漏電2 (`REG_INST_LEAKAGE_2`) | R | 整数値 | 32bit |
| `0x006C` | 漏電3 (`REG_INST_LEAKAGE_3`) | R | 整数値 | 32bit |
| `0x006E` | 漏電4 (`REG_INST_LEAKAGE_4`) | R | 整数値 | 32bit |

## 4. 最大値（Read Only）

| 先頭アドレス | 名前 | R/W | スケーリング | 備考 |
|---|---|---|---|---|
| `0x0300` | 電圧1 MAX (`REG_MAX_VOLTAGE_1`) | R | 実測値 × 10 | 32bit |
| `0x0302` | 電圧2 MAX (`REG_MAX_VOLTAGE_2`) | R | 実測値 × 10 | 32bit |
| `0x0304` | 電圧3 MAX (`REG_MAX_VOLTAGE_3`) | R | 実測値 × 10 | 32bit |
| `0x0364` | 漏電1 MAX (`REG_MAX_LEAKAGE_1`) | R | 整数値 | 32bit |
| `0x0366` | 漏電2 MAX (`REG_MAX_LEAKAGE_2`) | R | 整数値 | 32bit |
| `0x0368` | 漏電3 MAX (`REG_MAX_LEAKAGE_3`) | R | 整数値 | 32bit |
| `0x036A` | 漏電4 MAX (`REG_MAX_LEAKAGE_4`) | R | 整数値 | 32bit |

## 5. 最小値（Read Only）

| 先頭アドレス | 名前 | R/W | スケーリング | 備考 |
|---|---|---|---|---|
| `0x0400` | 電圧1 MIN (`REG_MIN_VOLTAGE_1`) | R | 実測値 × 10 | 32bit |
| `0x0402` | 電圧2 MIN (`REG_MIN_VOLTAGE_2`) | R | 実測値 × 10 | 32bit |
| `0x0404` | 電圧3 MIN (`REG_MIN_VOLTAGE_3`) | R | 実測値 × 10 | 32bit |
| `0x0464` | 漏電1 MIN (`REG_MIN_LEAKAGE_1`) | R | 整数値 | 32bit |
| `0x0466` | 漏電2 MIN (`REG_MIN_LEAKAGE_2`) | R | 整数値 | 32bit |
| `0x0468` | 漏電3 MIN (`REG_MIN_LEAKAGE_3`) | R | 整数値 | 32bit |
| `0x046A` | 漏電4 MIN (`REG_MIN_LEAKAGE_4`) | R | 整数値 | 32bit |

## 6. バージョン・状態

| 先頭アドレス | 名前 | R/W | 内容 | 備考 |
|---|---|---|---|---|
| `0x0700` | `REG_VERSION` | R | 版数情報 | 実装値は `0x0000_0100`（1.00） |
| `0x0702` | `REG_STATUS` | R | 状態 | 現状は `0` 固定 |

## 7. パラメータ（実装アクセスあり）

| 先頭アドレス | 名前 | R/W | 値/意味 | 備考 |
|---|---|---|---|---|
| `0x0900` | 系統1 相線式 (`REG_PRM_SYS1_PHASE_WIRE`) | R/W | 0:単相2線, 1:単相3線, 2:三相3線, 3:三相4線 | 書込は設定モード時のみ、32bit |
| `0x0C00` | 漏電CT1種別 (`REG_PRM_LEAKAGE_CT1_TYPE`) | R/W | CT種別コード | 書込は設定モード時のみ、32bit |
| `0x0C02` | 漏電CT2種別 (`REG_PRM_LEAKAGE_CT2_TYPE`) | R/W | CT種別コード | 同上 |
| `0x0C04` | 漏電CT3種別 (`REG_PRM_LEAKAGE_CT3_TYPE`) | R/W | CT種別コード | 同上 |
| `0x0C06` | 漏電CT4種別 (`REG_PRM_LEAKAGE_CT4_TYPE`) | R/W | CT種別コード | 同上 |
| `0x0C08` | 漏電CT5種別 (`REG_PRM_LEAKAGE_CT5_TYPE`) | R | 現状 0 固定 | 読出しのみ実装 |
| `0x0C0A` | 漏電CT6種別 (`REG_PRM_LEAKAGE_CT6_TYPE`) | R | 現状 0 固定 | 読出しのみ実装 |
| `0x0C0C` | 漏電CT7種別 (`REG_PRM_LEAKAGE_CT7_TYPE`) | R | 現状 0 固定 | 読出しのみ実装 |
| `0x0C0E` | 漏電CT8種別 (`REG_PRM_LEAKAGE_CT8_TYPE`) | R | 現状 0 固定 | 読出しのみ実装 |
| `0x0918` | 漏電ローカット (`REG_PRM_LEAKAGE_LOW_CUT`) | R/W | 読出しは `値 × 10` | 書込は設定モード時のみ、32bit |
| `0x0924` | 平均回数 (`REG_PRM_AVG_COUNT`) | R/W | 平均回数コード | 書込は設定モード時のみ、実質下位側に値 |
| `0x0B00` | ユニットNo (`REG_PRM_UNIT_NO`) | R/W | スレーブアドレス | 書込は設定モード時のみ、32bit |
| `0x0B02` | 通信速度 (`REG_PRM_BAUDRATE`) | R/W | 0:9600,1:19200,2:38400,3:57600,4:115200 | 書込は設定モード時のみ、32bit |
| `0x0B04` | データビット (`REG_PRM_DATA_BIT`) | R/W | 0:7bit, 1:8bit | 書込は設定モード時のみ、32bit |
| `0x0B06` | ストップビット (`REG_PRM_STOP_BIT`) | R/W | 0:1bit, 1:2bit | 書込は設定モード時のみ、32bit |
| `0x0B08` | パリティ (`REG_PRM_PARITY`) | R/W | 0:None, 1:Even, 2:Odd | 書込は設定モード時のみ、32bit |
| `0x0B0A` | 送信待ち時間 (`REG_PRM_TX_WAIT_TIME`) | R/W | 応答遅延ms | 書込は設定モード時のみ、32bit |

## 8. パラメータ（読出しのみ実装/固定値）

| 先頭アドレス | 名前 | R/W | 実装状態 |
|---|---|---|---|
| `0x0902` | `REG_PRM_SYS2_PHASE_WIRE` | R | 0 固定 |
| `0x0904` | `REG_PRM_BLK1_SYNC_SEL` | R | 0 固定 |
| `0x0BF0` | `REG_PRM_LINK_CONFIG` | R | 0 固定 |
| `0x0F00` | `REG_PRM_ATTR_READ_1` | R | 0 固定 |
| `0x0F02` | `REG_PRM_ATTR_READ_2` | R | 0 固定 |
| `0x0F04` | `REG_PRM_ATTR_READ_3` | R | 0 固定 |
| `0x0F06` | `REG_PRM_ATTR_READ_4` | R | 0 固定 |
| `0x0F08` | `REG_PRM_TIME_INFO_MD` | R | 0 固定 |
| `0x0F0A` | `REG_PRM_TIME_INFO_HMS` | R | 0 固定 |

## 9. 注意事項

- ヘッダ (`Core/Inc/modbus_reg.h`) には、上記以外にも多数の KE1 互換定義があります。
- ただし、実際の応答可否は `MODBUS_get_reg()` / `MODBUS_set_reg()` の `switch` 実装が最終的な正です。
- `MODBUS_set_reg()` は設定モード以外で一部レジスタ書込み時に `EXCEPTION_CODE_ILLEGAL_FUNCTION (0x01)` を返します。
