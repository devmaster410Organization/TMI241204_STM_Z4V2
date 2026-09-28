# CLAUDE.md

このファイルは、本リポジトリで作業する Claude Code 向けのガイドです。

## プロジェクト概要

Z4V2 漏電計（Leakage meter）の STM32 ファームウェア。STM32CubeIDE プロジェクト。

- 電圧（最大3系統）と漏電電流 I0（4ch）をサンプリングし、実効値・Ior（抵抗分漏電）・位相・統計（平均/最大/最小）を計算する
- 外部 I/F は RS-485 MODBUS RTU スレーブ（USART3）と USB CDC（仮想COMポート、保守用コマンド）
- キャラクタ LCD とキー / DIP スイッチによる UI

## マイコン

- 現行: **STM32G431CBT**（Flash 128K / RAM 32K）
- 次期: **STM32G473CCT3**（Flash 256K / RAM 128K）へ移行予定
- RAM 32K と余裕が少ないため、スタック・静的バッファの追加には注意すること

## ビルド

- STM32CubeIDE でビルド（`Debug/` 構成、`Debug/makefile` は自動生成）
- コマンドラインからは `Debug/` で `make -j` （`arm-none-eabi-gcc` が PATH にある場合）
- コンパイラオプション: `-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Og -std=gnu11`、`-u _printf_float`（printf で float 使用）
- 書込み/デバッグ: `Z4V2_Leakage_meter Debug.launch`（ST-LINK）
- 自動テストはない。実機で確認する

## CubeMX 生成コードの扱い

- `Z4V2_Leakage_meter.ioc` から `main.c`、`stm32g4xx_hal_msp.c`、`stm32g4xx_it.c`、`app_freertos.c`、`USB_Device/`、`Drivers/`、`Middlewares/` が生成される
- 生成ファイルを編集する場合は必ず `/* USER CODE BEGIN ... */` 〜 `/* USER CODE END ... */` の中に書くこと（それ以外は再生成で消える）
- `Drivers/`、`Middlewares/` は原則編集しない
- リンカスクリプト `STM32G431CBTX_FLASH.ld` はカスタマイズ済み（末尾 4K を `DATA_FLASH0` / `DATA_FLASH1` として設定値保存用に確保）

## アーキテクチャ

FreeRTOS（CMSIS-RTOS v2）上で以下のタスクが動く（タスクは `main.c` で静的生成）。

| タスク | 実体 | 役割 |
|---|---|---|
| tsk_Calc | `tsk_calc.c` | ADC DMA 受信 → 電圧/漏電計算、モード遷移処理 |
| tsk_Ul | `tsk_ui.c` | LCD 表示・キー / DIPSW 処理 |
| tsk_USB | `tsk_usb.c` | USB CDC コマンドシェル |
| tsk_MODBUS | `tsk_modbus.c` | MODBUS RTU スレーブ（USART3 / RS-485） |

### 計測データの流れ

1. ADC2（IN1〜IN4, IN12, IN13 の6ch）を TIM トリガで **3600 SPS** 変換、DMA 完了割込み（`HAL_ADC_ConvCpltCallback`）でサンプルを `sampling_t.sample_buf_t[]`（リングバッファ）に格納し、インデックスを `queue_ADCHandle` に送る
2. ADC1 は温度・VBAT・VREFINT（VDDA 補正に使用）
3. `tsk_calc` がキューから取り出し、
   - `calc_volt.c` : 電圧計算（`Culc_vol`）→ `PushVoltageStat`
   - `calc_leak.c` : I0 計算。HPF + Goertzel による 50/60Hz 自動判別の 100ms 実効値（`Leak100ms_5060_*`）
   - `calc_ior.c` : I0r 計算。100ms（360点 = 50Hz 5周期 / 60Hz 6周期）の DFT で VAC1/VAC2/LAC1-4 のフェーザを求め、基準電圧に対する I0 の位相を出す。振幅は `GetLInstValue()`（校正済み I0）を使い、角度だけ DSP で求める。ハード遅延は `IOR_V_DELAY_US` / `ior_i0_delay_us[]` で補正（USB `ior` コマンドでキャリブレーション）
     - 1P2W/1P3W: Vref = VAC1、3P3W(S接地): Vref = VAC1+VAC2（V_RS+V_TS）で I0r = I0·|cosθ|/cos30°
   - `calc_stat.c` : 平均・最大・最小などの統計（`PushLeakageStat` など）
4. 電圧ゼロクロスは TIM インプットキャプチャ（`HAL_TIM_IC_CaptureCallback`）で周期・周波数を取得（TIM15 = 10µs カウンタ）。インプットキャプチャによる I0r 算出（`get_all_phase`）は 1P3W/3P3W で使えず、現在は表示に使っていない
5. ADC2 の buf 並びは VAC1, VAC2, LAC1(IN13), LAC2(IN3), LAC3(IN4), LAC4(IN12)。`QSEL_IN3_CHANNEL` 等の名前はピン名と一致しないので注意

### 主要なグローバル

- `g_sys`（`prj.h`）: DIPSW 状態、動作モード。モード変更は `g_sys.mode_next` に書き、`tsk_calc` が遷移処理して `g_sys.mode` を更新する（`MODE_MEASURE` / `MODE_SETUP`）
- `g_setup`（`setup.h`）: 設定値（MODBUS 通信設定、漏電ローカット、相線式、CT 種別、校正値 gain/offset 等）
  - `setup_default` / `setup_min` / `setup_max` で既定値と範囲を定義。USB・MODBUS から設定する際は範囲チェックすること
  - `SETUP_write()` / `SETUP_read()` でチェックサム付きで Flash に保存。メイン/サブの2面に二重化
  - `setup_t` にメンバを追加すると Flash 上のレイアウトが変わる（既存保存値はチェックサム不一致 → 既定値になる）
- `sampling_t`（`tsk_calc.c`）: サンプリング/計算の作業領域とデバッグ用カウンタ
- `queue_USBHandle`: USB 送信キュー。16bit メッセージ `[15:8]=送信元, [7:0]=文字`（`USBMSG_PACK` 等のマクロを使う）

### その他

- `Core/ReiwaStd/` : 社内共通ライブラリ（FIFO、CRC16、キー入力、文字列変換、簡易UARTドライバ）
- `prj.h` : プロジェクト共通ヘッダ。アプリのソースはこれを include する
- `ver.c` / `ver.h` : バージョン文字列・ビルド日時

## ドキュメント

- [MODBUS_register_table.md](MODBUS_register_table.md) : MODBUS レジスタ表（`modbus_reg.c` / `modbus_reg.h` が根拠）
- [USB_command_table.md](USB_command_table.md) : USB コマンド表（`tsk_usb.c` が根拠）
- MODBUS レジスタや USB コマンドを変更したら、上記の表も合わせて更新すること
- `memo.txt` : TODO・備忘録

## コーディング規約

- C (gnu11)。コメントは日本語
- ファイル先頭に Doxygen 形式のヘッダ（`/// @file`, `@brief`, `@author`, `@date`, `@version`）
- 割込みコンテキストからは重い処理をせず、キュー経由でタスクに渡す
- float は単精度（`f` サフィックス）を使う（FPU は単精度のみ）
