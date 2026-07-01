/// @file    modbus_reg.h
/// @brief   Modbus register access function   header
/// @author  Y.Sugawara
/// @date    2026/4/28
/// @version 1.0


#ifndef INC_MODBUS_REG_H_
#define INC_MODBUS_REG_H_

/* =========================================================================
 * OMRON KE1-PGR1C Compatible Modbus Register Map
 * ========================================================================= */

 /* =========================================================================
 * OMRON KE1-PGR1C Modbus Register Map
 * ========================================================================= */



/* =========================================================================
 * OMRON KE1-PGR1C Compatible Modbus Register Map
 * ========================================================================= */

// =========================================================================
// 1. 変数エリア: 瞬時値 (Instantaneous Values)
// =========================================================================
#define REG_INST_VOLTAGE_1        0x0000  // 電圧1 (V)
#define REG_INST_VOLTAGE_2        0x0002  // 電圧2 (V)
#define REG_INST_VOLTAGE_3        0x0004  // 電圧3 (V)

#define REG_INST_CURRENT_1        0x000C  // 電流1 (A)
#define REG_INST_CURRENT_2        0x000E  // 電流2 (A)
#define REG_INST_CURRENT_3        0x0010  // 電流3 (A)
// ※ 電流4〜12は 0x0012 〜 0x0022 に順次割り当て

#define REG_INST_POWER_FACTOR_1   0x0024  // 力率1
#define REG_INST_POWER_FACTOR_2   0x0026  // 力率2
#define REG_INST_POWER_FACTOR_3   0x0028  // 力率3

#define REG_INST_FREQUENCY_1      0x0034  // 周波数1 (Hz)
#define REG_INST_FREQUENCY_2      0x0036  // 周波数2 (Hz)

#define REG_INST_ACTIVE_POWER_1   0x0038  // 有効電力1 (W)
#define REG_INST_ACTIVE_POWER_2   0x003A  // 有効電力2 (W)
#define REG_INST_ACTIVE_POWER_3   0x003C  // 有効電力3 (W)

#define REG_INST_REACTIVE_POWER_1 0x0048  // 無効電力1 (var)
#define REG_INST_REACTIVE_POWER_2 0x004A  // 無効電力2 (var)
#define REG_INST_REACTIVE_POWER_3 0x004C  // 無効電力3 (var)

#define REG_INST_TEMPERATURE_1    0x0058  // 温度1 (℃/F)
#define REG_INST_TEMPERATURE_2    0x005A  // 温度2 (℃/F)
#define REG_INST_TEMPERATURE_3    0x005C  // 温度3 (℃/F)

#define REG_INST_LEAKAGE_1        0x0068  // 漏電1 (mA)
#define REG_INST_LEAKAGE_2        0x006A  // 漏電2 (mA)
#define REG_INST_LEAKAGE_3        0x006C  // 漏電3 (mA)
#define REG_INST_LEAKAGE_4        0x006E  // 漏電4 (mA)
#define REG_INST_LEAKAGE_5        0x0070  // 漏電5 (mA)
#define REG_INST_LEAKAGE_6        0x0072  // 漏電6 (mA)
#define REG_INST_LEAKAGE_7        0x0074  // 漏電7 (mA)
#define REG_INST_LEAKAGE_8        0x0076  // 漏電8 (mA)


// =========================================================================
// 2. 変数エリア: 最大値 (Maximum Values)
// =========================================================================
#define REG_MAX_VOLTAGE_1         0x0300  // 電圧1 MAX
#define REG_MAX_VOLTAGE_2         0x0302  // 電圧2 MAX
#define REG_MAX_VOLTAGE_3         0x0304  // 電圧3 MAX

#define REG_MAX_CURRENT_1         0x030C  // 電流1 MAX
#define REG_MAX_CURRENT_2         0x030E  // 電流2 MAX
#define REG_MAX_CURRENT_3         0x0310  // 電流3 MAX

#define REG_MAX_POWER_FACTOR_1    0x0324  // 力率1 MAX
#define REG_MAX_POWER_FACTOR_2    0x0326  // 力率2 MAX
#define REG_MAX_POWER_FACTOR_3    0x0328  // 力率3 MAX

#define REG_MAX_ACTIVE_POWER_1    0x0334  // 有効電力1 MAX
#define REG_MAX_ACTIVE_POWER_2    0x0336  // 有効電力2 MAX
#define REG_MAX_ACTIVE_POWER_3    0x0338  // 有効電力3 MAX

#define REG_MAX_REACTIVE_POWER_1  0x0344  // 無効電力1 MAX
#define REG_MAX_REACTIVE_POWER_2  0x0346  // 無効電力2 MAX
#define REG_MAX_REACTIVE_POWER_3  0x0348  // 無効電力3 MAX

#define REG_MAX_TEMPERATURE_1     0x0354  // 温度1 MAX
#define REG_MAX_TEMPERATURE_2     0x0356  // 温度2 MAX
#define REG_MAX_TEMPERATURE_3     0x0358  // 温度3 MAX

#define REG_MAX_LEAKAGE_1         0x0364  // 漏電1 MAX
#define REG_MAX_LEAKAGE_2         0x0366  // 漏電2 MAX
#define REG_MAX_LEAKAGE_3         0x0368  // 漏電3 MAX
#define REG_MAX_LEAKAGE_4         0x036A  // 漏電4 MAX

// =========================================================================
// 3. 変数エリア: 最小値 (Minimum Values)
// =========================================================================
#define REG_MIN_VOLTAGE_1         0x0400  // 電圧1 MIN
#define REG_MIN_VOLTAGE_2         0x0402  // 電圧2 MIN
#define REG_MIN_VOLTAGE_3         0x0404  // 電圧3 MIN

#define REG_MIN_CURRENT_1         0x040C  // 電流1 MIN
#define REG_MIN_CURRENT_2         0x040E  // 電流2 MIN
#define REG_MIN_CURRENT_3         0x0410  // 電流3 MIN

#define REG_MIN_POWER_FACTOR_1    0x0424  // 力率1 MIN
#define REG_MIN_POWER_FACTOR_2    0x0426  // 力率2 MIN
#define REG_MIN_POWER_FACTOR_3    0x0428  // 力率3 MIN

#define REG_MIN_ACTIVE_POWER_1    0x0434  // 有効電力1 MIN
#define REG_MIN_ACTIVE_POWER_2    0x0436  // 有効電力2 MIN
#define REG_MIN_ACTIVE_POWER_3    0x0438  // 有効電力3 MIN

#define REG_MIN_REACTIVE_POWER_1  0x0444  // 無効電力1 MIN
#define REG_MIN_REACTIVE_POWER_2  0x0446  // 無効電力2 MIN
#define REG_MIN_REACTIVE_POWER_3  0x0448  // 無効電力3 MIN

#define REG_MIN_TEMPERATURE_1     0x0454  // 温度1 MIN
#define REG_MIN_TEMPERATURE_2     0x0456  // 温度2 MIN
#define REG_MIN_TEMPERATURE_3     0x0458  // 温度3 MIN

#define REG_MIN_LEAKAGE_1         0x0464  // 漏電1 MIN
#define REG_MIN_LEAKAGE_2         0x0466  // 漏電2 MIN
#define REG_MIN_LEAKAGE_3         0x0468  // 漏電3 MIN
#define REG_MIN_LEAKAGE_4         0x046A  // 漏電4 MIN



// --- バージョン・ステータス (Version & Status) ---
#define REG_VERSION                 0x0700  // バージョン [cite: 1467]
#define REG_STATUS                  0x0702  // ステータス [cite: 1467]

// --- 警報履歴 (Alarm History) ---
// ※ 履歴1 (最新) のみ抜粋。履歴2〜20は 0x071A〜0x07D6 に順次割り当てられています [cite: 1493, 1495, 1499, 1501]。
#define REG_ALARM_HIST1_CODE        0x0710  // 履歴1 発生警報 [cite: 1493]
#define REG_ALARM_HIST1_OCCUR_DATE  0x0712  // 履歴1 発生日時 (年月) [cite: 1493]
#define REG_ALARM_HIST1_OCCUR_TIME  0x0714  // 履歴1 発生日時 (日・時分秒) [cite: 1493]
#define REG_ALARM_HIST1_CLEAR_DATE  0x0716  // 履歴1 解除日時 (年月) [cite: 1493]
#define REG_ALARM_HIST1_CLEAR_TIME  0x0718  // 履歴1 解除日時 (日・時分秒) [cite: 1493]

#define REG_ALARM_INTERVAL 0x000A  // 警報発生間隔 [cite: 1494]


/* =========================================================================
 * OMRON KE1/KM1 Parameter Area (Modbus Address 0x0900~)
 * ========================================================================= */

// --- 基本設定 (相線式・同期) ---
#define REG_PRM_SYS1_PHASE_WIRE     0x0900  // 系統1 適用相線式 (0:単相2線, 1:単相3線, 2:三相3線, 3:三相4線)
#define PRM_PHASE_WIRE_1P2W     0x0000  // 系統1 適用相線式のビットマスク
#define PRM_PHASE_WIRE_1P3W     0x0001  // 系統1 適用相線式のビットマスク
#define PRM_PHASE_WIRE_3P3W     0x0002  // 系統1 適用相線式のビットマスク
#define PRM_PHASE_WIRE_3P4W     0x0003  // 系統1 適用相線式のビットマスク


#define REG_PRM_SYS2_PHASE_WIRE     0x0902  // 系統2 適用相線式
#define REG_PRM_BLK1_SYNC_SEL       0x0904  // 計測ブロック1 同期選択 (0:系統1, 1:系統2)
#define REG_PRM_BLK2_SYNC_SEL       0x0906  // 計測ブロック2 同期選択

// --- CT/VT設定 ---
#define REG_PRM_BLK1_CT_TYPE        0x0908  // 計測ブロック1 専用CT種別 (0:5A, 1:50A, 2:100A, 3:200A, 4:400A, 5:600A)
#define REG_PRM_BLK2_CT_TYPE        0x090A  // 計測ブロック2 専用CT種別
#define REG_PRM_SYS1_VT_RATIO       0x090C  // 系統1 VT比 (0.01～99.99 ※小数点以下2桁固定)
#define REG_PRM_SYS2_VT_RATIO       0x090E  // 系統2 VT比
#define REG_PRM_BLK1_CT_RATIO       0x0910  // 計測ブロック1 CT比 (1～1000)
#define REG_PRM_BLK2_CT_RATIO       0x0912  // 計測ブロック2 CT比

#define REG_PRM_LEAKAGE_CT1_TYPE    0x0C00  // 漏電CT種別 (0:5mA, 1:30mA, 2:100mA, 3:300mA, 4:500mA, 5:1000mA)
#define REG_PRM_LEAKAGE_CT2_TYPE    0x0C02  // 漏電CT種別 (0:5mA, 1:30mA, 2:100mA, 3:300mA, 4:500mA, 5:1000mA)
#define REG_PRM_LEAKAGE_CT3_TYPE    0x0C04  // 漏電CT種別 (0:5mA, 1:30mA, 2:100mA, 3:300mA, 4:500mA, 5:1000mA)
#define REG_PRM_LEAKAGE_CT4_TYPE    0x0C06  // 漏電CT種別 (0:5mA, 1:30mA, 2:100mA, 3:300mA, 4:500mA, 5:1000mA)
#define REG_PRM_LEAKAGE_CT5_TYPE    0x0C08  // 漏電CT種別 (0:5mA, 1:30mA, 2:100mA, 3:300mA, 4:500mA, 5:1000mA)
#define REG_PRM_LEAKAGE_CT6_TYPE    0x0C0A  // 漏電CT種別 (0:5mA, 1:30mA, 2:100mA, 3:300mA, 4:500mA, 5:1000mA)
#define REG_PRM_LEAKAGE_CT7_TYPE    0x0C0C  // 漏電CT種別 (0:5mA, 1:30mA, 2:100mA, 3:300mA, 4:500mA, 5:1000mA)
#define REG_PRM_LEAKAGE_CT8_TYPE    0x0C0E  // 漏電CT種別 (0:5mA, 1:30mA, 2:100mA, 3:300mA, 4:500mA, 5:1000mA)
#define PRM_LEAKAGE_CT_OTG_LA21     0x0000  // 高精度漏電CT1のビットマスク
#define PRM_LEAKAGE_CT_MZ1H    0x0001  // 
#define PRM_LEAKAGE_CT_OTG_LA21x10     0x0002  // 高精度漏電CT1のビットマスク
#define PRM_LEAKAGE_CT_MZ1Hx10    0x0003  // 


// --- ローカット設定 ---
#define REG_PRM_BLK1_LOWCUT_CURR    0x0914  // 計測ブロック1 ローカット電流値 (0.1～19.9%)
#define REG_PRM_BLK2_LOWCUT_CURR    0x0916  // 計測ブロック2 ローカット電流値
#define REG_PRM_LEAKAGE_LOWCUT      0x0918  // 漏電ローカット電流値 (0.1～30.0mA)



// --- 簡易計測設定 ---
#define REG_PRM_SIMPLE_MEASURE_MODE 0x091A  // 簡易計測 (0:OFF/通常計測, 1:ON/簡易計測)
#define REG_PRM_SYS1_SIMPLE_VOLT    0x091C  // 系統1 簡易計測時電圧 (0.1～9999.9V)
#define REG_PRM_SYS2_SIMPLE_VOLT    0x091E  // 系統2 簡易計測時電圧
#define REG_PRM_BLK1_SIMPLE_PF      0x0920  // 計測ブロック1 簡易計測時力率 (0.01～1.00)
#define REG_PRM_BLK2_SIMPLE_PF      0x0922  // 計測ブロック2 簡易計測時力率

// --- 平均回数 ---
#define REG_PRM_AVG_COUNT           0x0924  // 平均回数 (0:OFF, 1:2回, 2:4回... A:1024回)
#define PRM_AVG_0      0x0000  // 平均回数 0 (OFF)
#define PRM_AVG_2      0x0001  // 平均回数 2
#define PRM_AVG_4      0x0002  // 平均回数 4
#define PRM_AVG_8      0x0003  // 平均回数 8
#define PRM_AVG_16     0x0004  // 平均回数 16
#define PRM_AVG_32     0x0005  // 平均回数 32
#define PRM_AVG_64     0x0006  // 平均回数 64
#define PRM_AVG_128    0x0007  // 平均回数 128
#define PRM_AVG_256    0x0008  // 平均回数 256
#define PRM_AVG_512    0x0009  // 平均回数 512
#define PRM_AVG_1024   0x000A  // 平均回数 1024

// --- イベント入力設定 (1〜7) ---
#define REG_PRM_EVT_IN1_FUNC        0x0926  // イベント入力設定1 (0:P.CSP, 1:H-ON, 2:3-ST)
#define REG_PRM_EVT_IN2_FUNC        0x0928  // イベント入力設定2
#define REG_PRM_EVT_IN3_FUNC        0x092A  // イベント入力設定3
#define REG_PRM_EVT_IN4_FUNC        0x092C  // イベント入力設定4
#define REG_PRM_EVT_IN5_FUNC        0x092E  // イベント入力設定5
#define REG_PRM_EVT_IN6_FUNC        0x0930  // イベント入力設定6
#define REG_PRM_EVT_IN7_FUNC        0x0932  // イベント入力設定7

// --- イベント入力 NPN/PNP設定 (1〜7) ---
#define REG_PRM_EVT_IN1_NPN_PNP     0x0934  // イベント入力1 NPN/PNP設定 (0:PNP, 1:NPN)
#define REG_PRM_EVT_IN2_NPN_PNP     0x0936  // イベント入力2 NPN/PNP設定
#define REG_PRM_EVT_IN3_NPN_PNP     0x0938  // イベント入力3 NPN/PNP設定
#define REG_PRM_EVT_IN4_NPN_PNP     0x093A  // イベント入力4 NPN/PNP設定
#define REG_PRM_EVT_IN5_NPN_PNP     0x093C  // イベント入力5 NPN/PNP設定
#define REG_PRM_EVT_IN6_NPN_PNP     0x093E  // イベント入力6 NPN/PNP設定
#define REG_PRM_EVT_IN7_NPN_PNP     0x0940  // イベント入力7 NPN/PNP設定

// --- イベント入力 モード設定 (N-O/N-C) (1〜7) ---
#define REG_PRM_EVT_IN1_MODE        0x0942  // イベント入力1 モード設定 (0:N-O, 1:N-C)
#define REG_PRM_EVT_IN2_MODE        0x0944  // イベント入力2 モード設定
#define REG_PRM_EVT_IN3_MODE        0x0946  // イベント入力3 モード設定
#define REG_PRM_EVT_IN4_MODE        0x0948  // イベント入力4 モード設定
#define REG_PRM_EVT_IN5_MODE        0x094A  // イベント入力5 モード設定
#define REG_PRM_EVT_IN6_MODE        0x094C  // イベント入力6 モード設定
#define REG_PRM_EVT_IN7_MODE        0x094E  // イベント入力7 モード設定

// --- 計測時間設定 ---
#define REG_PRM_MEASURE_START_TIME  0x0950  // 計測開始時刻 (HHMM)
#define REG_PRM_MEASURE_END_TIME    0x0952  // 計測終了時刻 (HHMM)

// --- 3-STATE 設定 (代表項目) ---
#define REG_PRM_BLK1_3STATE_TARGET  0x0954  // 計測ブロック1 3-STATE 判定対象 (0:電力, 1:電流, 2:電圧...)
#define REG_PRM_BLK2_3STATE_TARGET  0x0956  // 計測ブロック2 3-STATE 判定対象
#define REG_PRM_BLK1_3STATE_EVT_IN  0x0958  // 計測ブロック1 3-STATE/原単位 イベント入力
#define REG_PRM_BLK2_3STATE_EVT_IN  0x095A  // 計測ブロック2 3-STATE/原単位 イベント入力
#define REG_PRM_BLK1_3STATE_HI_THR  0x095C  // 計測ブロック1 3-STATE HIGH 閾値
#define REG_PRM_BLK2_3STATE_HI_THR  0x095E  // 計測ブロック2 3-STATE HIGH 閾値
#define REG_PRM_BLK1_3STATE_LO_THR  0x0960  // 計測ブロック1 3-STATE LOW 閾値
#define REG_PRM_BLK2_3STATE_LO_THR  0x0962  // 計測ブロック2 3-STATE LOW 閾値

// --- 温度設定 ---
#define REG_PRM_TEMP_UNIT           0x0968  // 温度単位 (0:摂氏/C, 1:華氏/F)
#define REG_PRM_TEMP_CALIBRATION    0x096A  // 温度補正値1 (-50.0～50.0)

// --- パルス換算設定 (代表項目) ---
#define REG_PRM_PULSE_RATE_1        0x098A  // パルス換算係数設定1 (0.01～9999.99)
// (0x098C〜0x0996 はパルス換算係数設定2〜7)

// --- パルス出力設定 ---
#define REG_PRM_PULSE_OUT_UNIT      0x09B8  // パルス出力単位 (0:1Wh, 1:10Wh, 2:100Wh ...)
#define REG_PRM_PULSE_OUT_CIRCUIT   0x09BA  // パルス出力回路 (0:回路1, 1:回路2 ...)



// --- パラメータエリア (Parameters) ---
// 漏電設定
#define REG_PRM_LEAKAGE_LOW_CUT     0x0918  // 漏電ローカット電流値 [cite: 1712]
#define REG_PRM_LEAKAGE_CMP_1       0x09DE  // 漏電比較値1 [cite: 1738]
#define REG_PRM_LEAKAGE_CMP_2       0x09E0  // 漏電比較値2 [cite: 1738]
#define REG_PRM_LEAKAGE_DELAY_1     0x09EE  // 漏電動作時間1 [cite: 1740]
#define REG_PRM_LEAKAGE_DELAY_2     0x09F0  // 漏電動作時間2 [cite: 1740]

// イベント入力設定
#define REG_PRM_EVT_IN1_SETTING     0x0926  // イベント入力設定1 [cite: 1716]
#define REG_PRM_EVT_IN2_SETTING     0x0928  // イベント入力設定2 [cite: 1716]
#define REG_PRM_EVT_IN1_NPN_PNP     0x0934  // イベント入力1 NPN/PNP 入力モード設定 [cite: 1718]
#define REG_PRM_EVT_IN2_NPN_PNP     0x0936  // イベント入力2 NPN/PNP 入力モード設定 [cite: 1718]
#define REG_PRM_EVT_IN1_MODE        0x0942  // イベント入力1 入力モード設定 [cite: 1720]
#define REG_PRM_EVT_IN2_MODE        0x0944  // イベント入力2 入力モード設定 [cite: 1720]

// 時間設定
#define REG_PRM_MEASURE_START_TIME  0x0950  // 計測開始時刻 [cite: 1722]
#define REG_PRM_MEASURE_END_TIME    0x0952  // 計測終了時刻 [cite: 1723]

// 出力端子設定
#define REG_PRM_OUT1_FUNC           0x09FE  // 出力端子1 機能設定 [cite: 1743]
#define REG_PRM_OUT2_FUNC           0x0A00  // 出力端子2 機能設定 [cite: 1744]
#define REG_PRM_OUT1_STATE          0x0A04  // 出力端子1 状態 (N-O/N-C) [cite: 1745]
#define REG_PRM_OUT2_STATE          0x0A06  // 出力端子2 状態 (N-O/N-C) [cite: 1746]

// 通信設定
#define REG_PRM_UNIT_NO             0x0B00  // ユニットNo. [cite: 1786]
#define REG_PRM_BAUDRATE            0x0B02  // 通信速度 [cite: 1787]

#define PRM_BAUDRATE_9600   0x0000  // 9600bps  
#define PRM_BAUDRATE_19200  0x0001  // 19200bps
#define PRM_BAUDRATE_38400  0x0002  // 38400bps
#define PRM_BAUDRATE_57600  0x0003  // 57600bps //KE1は非対応
#define PRM_BAUDRATE_115200 0x0004  // 115200bps  //KE1は非対応

#define REG_PRM_DATA_BIT            0x0B04  // データビット長 [cite: 1787]

#define PRM_DATA_BIT_7 0x0000  // データビット長 7bit
#define PRM_DATA_BIT_8 0x0001  // データビット長 8bit
    
#define REG_PRM_STOP_BIT            0x0B06  // ストップビット長 [cite: 1788]
#define PRM_STOP_BIT_1 0x0000  // ストップビット長 1bit
#define PRM_STOP_BIT_2 0x0001  // ストップビット長 2bit

#define REG_PRM_PARITY              0x0B08  // 垂直パリティ [cite: 1788]

#define PRM_PARITY_NONE 0x0000  // パリティなし
#define PRM_PARITY_EVEN 0x0001  // 偶数パリティ
#define PRM_PARITY_ODD  0x0002  // 奇数パリティ


#define REG_PRM_TX_WAIT_TIME        0x0B0A  // 送信待ち時間 [cite: 1789]
#define REG_PRM_LINK_CONFIG         0x0BF0  // 連結構成 [cite: 1824]

// ログ保存設定 (1〜6)
#define REG_PRM_LOG1_TARGET         0x0D00  // ログ1 保存対象 [cite: 1825]
#define REG_PRM_LOG2_TARGET         0x0D02  // ログ2 保存対象 [cite: 1826]
#define REG_PRM_LOG3_TARGET         0x0D04  // ログ3 保存対象 [cite: 1826]
#define REG_PRM_LOG4_TARGET         0x0D06  // ログ4 保存対象 [cite: 1827]
#define REG_PRM_LOG5_TARGET         0x0D08  // ログ5 保存対象 [cite: 1827]
#define REG_PRM_LOG6_TARGET         0x0D0A  // ログ6 保存対象 [cite: 1828]
#define REG_PRM_LOG1_CYCLE          0x0D0C  // ログ1 保存周期 [cite: 1828]
#define REG_PRM_LOG2_CYCLE          0x0D0E  // ログ2 保存周期 [cite: 1829]
#define REG_PRM_LOG3_CYCLE          0x0D10  // ログ3 保存周期 [cite: 1829]
#define REG_PRM_LOG4_CYCLE          0x0D12  // ログ4 保存周期 [cite: 1829]
#define REG_PRM_LOG5_CYCLE          0x0D14  // ログ5 保存周期 [cite: 1830]
#define REG_PRM_LOG6_CYCLE          0x0D16  // ログ6 保存周期 [cite: 1830]

// 本体属性・時間情報
#define REG_PRM_ATTR_READ_1         0x0F00  // 本体属性読出し1 [cite: 1832]
#define REG_PRM_ATTR_READ_2         0x0F02  // 本体属性読出し2 [cite: 1833]
#define REG_PRM_ATTR_READ_3         0x0F04  // 本体属性読出し3 [cite: 1833]
#define REG_PRM_ATTR_READ_4         0x0F06  // 本体属性読出し4 [cite: 1834]
#define REG_PRM_TIME_INFO_MD        0x0F08  // 時間情報（月日） [cite: 1834]
#define REG_PRM_TIME_INFO_HMS       0x0F0A  // 時間情報（時分） [cite: 1835]


// =========================================================================
// 4. 動作指令 (Operation Commands) - マニュアル 3.11
// =========================================================================
// ※ファンクションコード: 0x06 (Write Single Register)
// ※書込先アドレス: 常に 0x0000

#define CMD_ADDR_OPERATION        0x0000  // 動作指令の書込先アドレス

// 以下の値は「指令コード(上位1バイト) + 関連情報(下位1バイト)」の書込データです
#define CMD_DATA_RESET_ENERGY     0x0300  // 積算電力量のゼロリセット
#define CMD_DATA_GOTO_MEASURE     0x0400  // 計測モードへ移行
#define CMD_DATA_GOTO_SETTING     0x0700  // 設定モードへ移行
#define CMD_DATA_INIT_HISTORY     0x0800  // 計測履歴初期化
#define CMD_DATA_INIT_SETTINGS    0x0901  // 設定値初期化
#define CMD_DATA_INIT_ALL         0x0903  // 全初期化
#define CMD_DATA_INIT_ALARM       0x0904  // 警報履歴初期化
#define CMD_DATA_READ_VOLT_DIP_0  0x1000  // 瞬低ログデータ読出し (先頭へ移動)
#define CMD_DATA_READ_VOLT_DIP_1  0x1001  // 瞬低ログデータ読出し (ポインタを進める)
#define CMD_DATA_READ_VOLT_DIP_2  0x1002  // 瞬低ログデータ読出し (消去して進める)
#define CMD_DATA_RESET_MAX        0x1200  // 各計測値最大値リセット
#define CMD_DATA_RESET_MIN        0x1300  // 各計測値最小値リセット
#define CMD_DATA_SOFT_RESET       0x9900  // ソフトリセット (無応答になります)



int MODBUS_set_reg(uint16_t add, int16_t data) ;
int MODBUS_get_reg(uint16_t add, int16_t *val) ;
int check_parameter(uint32_t value, uint32_t min, uint32_t max);

#endif /* INC_MODBUS_REG_H_ */
