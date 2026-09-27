# USB コマンド表

本書は以下の実装を根拠にしています。
- Core/Src/tsk_usb.c
- USB_Device/App/usbd_cdc_if.c

## 1. インターフェース仕様

- 方式: USB CDC (Virtual COM Port)
- 受信改行: CR+LF / CR / LF
- 送信改行: CR+LF
- エコーバック: 既定で ON (`usbcb.echo_flg = true`)
- BS (0x08): サポート
- 矢印キー: 非サポート
- 1行最大: `CDC_DATA_FS_MAX_PACKET_SIZE` 相当

## 2. コマンド一覧

| コマンド | 書式 | 機能 | 応答/動作 |
|---|---|---|---|
| `version` | `version` | バージョン表示 | `Version: 1.0.0` |
| `help` | `help` | コマンド一覧表示 | 登録コマンド名を列挙 |
| `?` | `?` | `help` と同義 | 同上 |
| `reset` | `reset` | リセット要求 | `System Reset Command Received.` を返す（本関数内で実リセット実行はしない） |
| `set` | `set <param> [value]` | 設定値変更/現在値表示 | 引数1個なら現在値表示、2個なら値を変更 |
| `get` | `get <param>` / `get all` | 設定値取得 | 単体または全項目表示 |
| `mode` | `mode` / `mode <n>` | モード表示/変更 | `mode` は現在モード表示、`0`=RUN, `1`=SETUP |
| `status` | `status` | 状態表示 | Ver, DSW, VDDA を表示 |
| `mon` | `mon` | モニタ要求 | `g_sys.monz0_count = 360` を設定 |
| `power` | `power` | 予約 | 現状は無応答（何も出力しない） |

- コマンド名は大文字小文字を区別（`strncmp` による完全一致）。
- 不明コマンドは `Unknown Command.` の後に入力行そのものをエコーします。
- 入力は空白区切りで最大10ワードまで解析され、超過分は無視されます。

## 3. コマンド別 実行例

以降の `>` は端末（TeraTerm 等）からの入力行、それ以外は本機からの応答です。

### 3.1 `version`

```
> version
Version: 1.0.0
```

### 3.2 `help` / `?`

```
> help
Available Commands:
version
help
?
reset
set
get
mode
power
status
mon
```

### 3.3 `reset`

```
> reset
System Reset Command Received.
```

※ 本コマンドは応答のみで、実際のリセット処理（`NVIC_SystemReset()`等）はこの関数内では呼ばれません。

### 3.4 `status`

```
> status
Status:
Ver:1.0.0
dsw:0x0A
vdda:3.30 V
```

- `Ver` は `pVersionString`
- `dsw` は `GetDsw()` の値を16進2桁表示
- `vdda` は `sysvdda` を小数第2位まで表示

### 3.5 `mon`

```
> mon
```

- 応答メッセージは出力されません（`g_sys.monz0_count = 360` を内部設定するのみ）。

### 3.6 `power`

```
> power
```

- 現状は完全に無処理（応答なし）。将来リレー駆動用に予約されています。

### 3.7 `mode`

現在モードを表示:
```
> mode
Current Mode: RUN
```

計測モードへ変更:
```
> mode 0
** RUN **
```

設定モードへ変更:
```
> mode 1
## SETUP MODE ##
```

- 数値以外や `0`/`1` 以外を指定した場合は無応答（該当 `case` がないため）。

### 3.8 `set`

**引数なし（使い方表示）:**
```
> set
Usage: set <param> <value>
Usage: get <param> | get all
params:
modbus_slave_address rs485_baudrate rs485_stop_bit rs485_parity rs485_bit_length response_delay_ms
leakage_low_cut ac_phase_wire ct_type1 ct_type2 ct_type3 ct_type4 avarage_count
volt_calib1_gain volt_calib1_offset volt_calib2_gain volt_calib2_offset
leakage_calib1_gain leakage_calib1_offset ... leakage_calib4_gain leakage_calib4_offset
```

**引数1個（現在値のみ表示、値は変更されない）:**
```
> set rs485_baudrate
rs485_baudrate:2
```

**引数2個（値変更、SETUPモード時）:**
```
> set rs485_baudrate 2
set ok: rs485_baudrate
rs485_baudrate:2
```

**範囲外の値を指定した場合:**
```
> set modbus_slave_address 300
Invalid value for modbus_slave_address: 300
```

**存在しないパラメータ名を指定した場合:**
```
> set foo_bar 1
Set failed. Check parameter name and value format.
Usage: set <param> <value>
Usage: get <param> | get all
params:
...(以下 help と同じ一覧)
```

**計測モード中に `set` した場合:**
```
> set rs485_baudrate 2
Cannot set parameter in MEAS mode
```

### 3.9 `get`

**引数なし（使い方表示、内容は `set` と同じ）:**
```
> get
Usage: set <param> <value>
...
```

**単一パラメータ取得:**
```
> get leakage_low_cut
leakage_low_cut:0.1
```

**未知のパラメータ名を指定した場合:**
```
> get foo_bar
Unknown parameter:foo_bar
```

**全パラメータ取得:**
```
> get all
modbus_slave_address:11
rs485_baudrate:2
rs485_stop_bit:1
rs485_parity:0
rs485_bit_length:1
response_delay_ms:0
leakage_low_cut:0.1
ac_phase_wire:1
ct_type1:0
ct_type2:1
ct_type3:1
ct_type4:1
avarage_count:0
volt_calib1_gain:1
volt_calib1_offset:0
volt_calib2_gain:1
volt_calib2_offset:0
volt_calib3_gain:1
volt_calib3_offset:0
leakage_calib1_gain:1
leakage_calib1_offset:0
leakage_calib2_gain:1
leakage_calib2_offset:0
leakage_calib3_gain:1
leakage_calib3_offset:0
leakage_calib4_gain:1
leakage_calib4_offset:0
```

（値は工場出荷時デフォルトの例。実機の応答は現在の設定値になります）

## 4. set/get パラメータ詳細（型・範囲・既定値）

| パラメータ名 | 型 | min | 既定値 | max | 単位/意味 |
|---|---|---|---|---|---|
| `modbus_slave_address` | u8 | 1 | 11 | 254 | MODBUSスレーブアドレス |
| `rs485_baudrate` | u8 (コード) | 0 | 2 | 4 | 0:9600, 1:19200, 2:38400, 3:57600, 4:115200 |
| `rs485_stop_bit` | u8 (コード) | 0 | 1 | 1 | 0:1bit, 1:2bit |
| `rs485_parity` | u8 (コード) | 0 | 0 | 2 | 0:None, 1:Even, 2:Odd |
| `rs485_bit_length` | u8 (コード) | 0 | 1 | 1 | 0:7bit, 1:8bit |
| `response_delay_ms` | u16 | 0 | 0 | 100 | 応答遅延 [ms] |
| `leakage_low_cut` | float | 0.0 | 0.1 | 100.0 | 漏電ローカット電流 [mA] |
| `ac_phase_wire` | u8 (コード) | 0 | 1 | 3 | 0:単相2線, 1:単相3線, 2:三相3線, 3:三相4線 |
| `ct_type1` | u8 (コード) | 0 | 0 | 1 | 0:OTG_LA21, 1:MZ1H |
| `ct_type2` | u8 (コード) | 0 | 1 | 1 | 同上 |
| `ct_type3` | u8 (コード) | 0 | 1 | 1 | 同上 |
| `ct_type4` | u8 (コード) | 0 | 1 | 1 | 同上 |
| `avarage_count` | u16 (コード) | 0 | 0 | 10 | 0:OFF ... 10:1024回平均 |
| `volt_calib1_gain` / `offset` | float | 1.0 / 0.0 | 1.0 / 0.0 | 1.0 / 0.0 | min=max=既定値のため実質固定 |
| `volt_calib2_gain` / `offset` | float | 1.0 / 0.0 | 1.0 / 0.0 | 1.0 / 0.0 | 同上 |
| `volt_calib3_gain` / `offset` | float | 1.0 / 0.0 | 1.0 / 0.0 | 1.0 / 0.0 | 同上 |
| `leakage_calib1〜4_gain` / `offset` | float | 1.0 / 0.0 | 1.0 / 0.0 | 1.0 / 0.0 | 同上 |

- `volt_calib*` / `leakage_calib*` は現行実装で min=max=既定値のため、`check_parameter()` により事実上変更不可（同値以外はエラー）です。
- 表示フォーマットは整数系が `%u`、float系が `%.6g`（例: `1.0` → `1`、`0.1` → `0.1`）。

## 5. エラー・異常系の挙動一覧

| 状況 | 応答 |
|---|---|
| 未知コマンド | `Unknown Command.` の後に入力行そのまま |
| `set`/`get` の未知パラメータ | `get`: `Unknown parameter:<param>` / `set`: `Set failed. Check parameter name and value format.` + 使い方表示 |
| 値の形式不正（数値でない等） | 無応答で終了（`return 0`） |
| 範囲外の値 | `Invalid value for <param>: <value>` |
| 計測モード中の `set` | `Cannot set parameter in MEAS mode` |
| パラメータ名/値のトークンが長すぎる/空 | `Invalid parameter token.` または `Invalid value token.` |

## 6. USB CDC 実装メモ

- 受信 (`CDC_Receive_FS`)
  - 受信バイトを1バイトずつキューへ投入
  - メッセージは `USBMSG_PACK(USBMSG_SRC_USB, Buf[i])`
- 送信 (`CDC_Transmit_FS`)
  - `TxState != 0` の場合 `USBD_BUSY`
- CDC 制御要求 (`CDC_Control_FS`)
  - `SET_LINE_CODING` などのケースは実装なし（`USBD_OK` を返すのみ）

## 7. 操作シナリオ例（設定変更フロー）

```
> mode
Current Mode: RUN
> mode 1
## SETUP MODE ##
> set rs485_baudrate 3
set ok: rs485_baudrate
rs485_baudrate:3
> get rs485_baudrate
rs485_baudrate:3
> mode 0
** RUN **
```

- 通信速度などのRS485パラメータはSETUPモード中に変更し、`mode 0` でRUNへ戻す運用を想定しています。

## 8. 運用上の注意

- コマンド入力は空白区切りで最大10ワードまで解析。
- `set` / `get` はヘルプ表示付きで運用することを推奨。
- `reset` と `power` は将来拡張余地あり（現行は限定動作）。
