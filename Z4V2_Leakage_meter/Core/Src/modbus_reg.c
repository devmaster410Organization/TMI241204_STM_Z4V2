/// @file    modbus_reg.c
/// @brief   Modbus register access function/ regisuter definition
/// @author  Y.Sugawara
/// @date    2026/4/28
/// @version 1.0


#include "prj.h"
#include "modbus_reg.h"




/* =========================================================================
 * OMRON KE1-PGR1C Compatible Modbus Register Map
 * ========================================================================= */

 /* =========================================================================
 * OMRON KE1-PGR1C Modbus Register Map
 * ========================================================================= */



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
#define REG_PRM_DATA_BIT            0x0B04  // データビット長 [cite: 1787]
#define REG_PRM_STOP_BIT            0x0B06  // ストップビット長 [cite: 1788]
#define REG_PRM_PARITY              0x0B08  // 垂直パリティ [cite: 1788]
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




/// @brief modbus writeのとき、add番地にint16_t dataを書き込む
/// @param add 
/// @param data 
/// @return 
int MODBUS_set_reg(uint16_t add, int16_t data) 
{
	int ret = EXCEPTION_CODE_OK;
//	int a,b;
//	int sadd;


  switch(add){
    case CMD_ADDR_OPERATION:
      switch(data){
        case CMD_DATA_RESET_ENERGY:
        // 積算電力量のゼロリセット
        // 本計測器には機能がないので 何もしない。 EXCEPTION_CODE_OK を返す。
          break;
        case CMD_DATA_GOTO_MEASURE:// 計測モードへ移行
          g_sys.mode = MODE_MEASURE;
            break;
        case CMD_DATA_GOTO_SETTING:// 設定モードへ移行 
          g_sys.mode = MODE_SETUP;
          break;
        case CMD_DATA_INIT_HISTORY:
        // 計測履歴初期化
          break;
        case CMD_DATA_INIT_SETTINGS:
        // 設定値初期化
          break;
        case CMD_DATA_INIT_ALL:
        // 全初期化
          break;
        case CMD_DATA_INIT_ALARM:
        // 警報履歴初期化
          break;
        case CMD_DATA_READ_VOLT_DIP_0:
        // 瞬低ログデータ読出し (先頭へ移動)
          break;
        case CMD_DATA_READ_VOLT_DIP_1:
        // 瞬低ログデータ読出し (ポインタを進める)              
          break;
        case CMD_DATA_READ_VOLT_DIP_2:
        // 瞬低ログデータ読出し (消去して進める)
          break;
        case CMD_DATA_RESET_MAX:
        // 各計測値最大値リセット
          Calc_ResetMaxVoltValue();
          Calc_ResetMaxLeakValue();
          break;
        case CMD_DATA_RESET_MIN:
        // 各計測値最小値リセット     
          Calc_ResetMinVoltValue();
          Calc_ResetMinLeakValue();
          break;
        case CMD_DATA_SOFT_RESET:
          NVIC_SystemReset(); // ソフトリセット (無応答になります)
           break;                                                   
       default:
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA;
      }
      break;
    
    default:
    ret = EXCEPTION_CODE_ILLEGAL_ADDRESS;
      break;
  }
	return ret;
}

static uint16_t dummycnt = 0;
/// @brief modbus readのとき、add番地の値をvalに入れる
/// @param add 
/// @param val 
/// @return 
int MODBUS_get_reg(uint16_t add, int16_t *val) 
{
	int ret = EXCEPTION_CODE_OK;
//	int a,b;
//	int sadd;

	switch( add ){
    case 0x0000:
      *val = g_sys.dip_sw;
      break;
		default:
//				ret = EXCEPTION_CODE_ILLIGAL_ADDRESS;
      *val = dummycnt++;

		break;
	}
	return ret;
}

