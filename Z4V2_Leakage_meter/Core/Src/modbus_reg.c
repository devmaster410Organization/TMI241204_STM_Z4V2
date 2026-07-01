/// @file    modbus_reg.c
/// @brief   Modbus register access function/ regisuter definition
/// @author  Y.Sugawara
/// @date    2026/4/28
/// @version 1.0


#include "prj.h"
#include "modbus_reg.h"


typedef struct {
  // read-only
  uint32_t  reg_inst_voltage[3];   // 電圧1〜3
  uint32_t  reg_inst_frequency[1]; // 周波数1
  uint32_t  reg_inst_temperature[1];   // 温度1
  uint32_t reg_inst_leakage[8];    // 漏電1〜8

  uint32_t  reg_max_voltage[3];   // 電圧1〜3 MAX
  uint32_t  reg_max_leakage[8]; // 漏電1 MAX
  uint32_t  reg_min_voltage[3];   // 電圧1〜3 MIN
  uint32_t  reg_min_leakage[8]; // 漏電1 MIN
  uint32_t reg_version;
  uint32_t reg_status;
  uint32_t reg_prm_leakage_low_cut;
  uint32_t reg_prm_time_info_md;
  uint32_t reg_prm_time_info_hms;

  // write-only
  uint16_t reg_prm_sys1_phase_wire_upper;
  uint16_t reg_prm_leakage_ct_upper[8];
  uint16_t reg_prm_leakage_low_cut_upper;
  uint16_t reg_cmd_avg_count_upper;
  uint16_t reg_prm_unit_no_upper;
  uint16_t reg_prm_baudrate_upper;
  uint16_t reg_prm_data_bit_upper;
  uint16_t reg_prm_rs485_stop_bit_upper;
  uint16_t reg_prm_parity_upper;
  uint16_t reg_prm_tx_wait_timer_upper;
  uint16_t reg_cmd_operation;

}modbusbuf_t;
modbusbuf_t modbusbuf;


/// @brief セットアップが変更したことを記録する。tsk_calc内でg_sys.setup_updateを検知したら、tsk_setupに通知するために0に戻す。
/// @param  
void setup_update( void )
{
  g_sys.setup_update = 1;
  g_sys.setup_update_time = xTaskGetTickCount();
}

/// @brief パラメータの値がmin〜maxの範囲内にあるかチェックする。範囲外なら-1、正常なら0を返す。
/// @param value チェックする値
/// @param min 最小値
/// @param max 最大値
/// @return 範囲内なら0、範囲外なら-1
int check_parameter(uint32_t value, uint32_t min, uint32_t max)
{
  if( value <= min || value >= max ){
    return -1; // 不正な値
  }else{
    return 0; // 正常
  }
}

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
          g_sys.mode_next = MODE_MEASURE;
            break;
        case CMD_DATA_GOTO_SETTING:// 設定モードへ移行 
          g_sys.mode_next = MODE_SETUP;
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
          ResetMaxVoltValue();
          ResetMaxLeakValue();
          break;
        case CMD_DATA_RESET_MIN:
        // 各計測値最小値リセット     
          ResetMinVoltValue();
          ResetMinLeakValue();
          break;
        case CMD_DATA_SOFT_RESET:
          NVIC_SystemReset(); // ソフトリセット (無応答になります)
           break;                                                   
       default:
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA;
      }
      break;
    case REG_PRM_SYS1_PHASE_WIRE:
      if( g_sys.mode == MODE_SETUP ){
        modbusbuf.reg_prm_sys1_phase_wire_upper = data;
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;
    case REG_PRM_SYS1_PHASE_WIRE+1:
      if( g_sys.mode == MODE_SETUP ){
        uint32_t tmp = modbusbuf.reg_prm_sys1_phase_wire_upper;
        tmp = (tmp<<16) | (uint16_t)data;
        if( check_parameter( tmp, setup_min.ac_phase_wire, setup_max.ac_phase_wire ) != 0 ){
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA; // 不正な値
        }else{
          g_setup.ac_phase_wire = tmp ; 
          setup_update();
        }
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;

    case REG_PRM_LEAKAGE_CT1_TYPE:
      if( g_sys.mode == MODE_SETUP ){
        modbusbuf.reg_prm_leakage_ct_upper[0] = data;
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;
    case REG_PRM_LEAKAGE_CT1_TYPE+1:
      if( g_sys.mode == MODE_SETUP ){
        uint32_t tmp = modbusbuf.reg_prm_leakage_ct_upper[0];
        tmp = (tmp<<16) | (uint16_t)data;
        if( check_parameter( tmp, setup_min.ct_type[0], setup_max.ct_type[0] ) != 0 ){
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA; // 不正な値
        }else{
          g_setup.ct_type[0] = tmp ;
          setup_update();
        }
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;

    case REG_PRM_LEAKAGE_CT2_TYPE:
      if( g_sys.mode == MODE_SETUP ){
        modbusbuf.reg_prm_leakage_ct_upper[1] = data;
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;
    case REG_PRM_LEAKAGE_CT2_TYPE+1:
      if( g_sys.mode == MODE_SETUP ){
        uint32_t tmp = modbusbuf.reg_prm_leakage_ct_upper[1];
        tmp = (tmp<<16) | (uint16_t)data;
        if( check_parameter( tmp, setup_min.ct_type[1], setup_max.ct_type[1] ) != 0 ){
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA; // 不正な値
        }else{
          g_setup.ct_type[1] = tmp ;
          setup_update();
        }
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;

    case REG_PRM_LEAKAGE_CT3_TYPE:
      if( g_sys.mode == MODE_SETUP ){
        modbusbuf.reg_prm_leakage_ct_upper[2] = data;
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;
    case REG_PRM_LEAKAGE_CT3_TYPE+1:
      if( g_sys.mode == MODE_SETUP ){
        uint32_t tmp = modbusbuf.reg_prm_leakage_ct_upper[2];
        tmp = (tmp<<16) | (uint16_t)data;
        if( check_parameter( tmp, setup_min.ct_type[2], setup_max.ct_type[2] ) != 0 ){
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA; // 不正な値
        }else{
          g_setup.ct_type[2] = tmp ;
          setup_update();
        }
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;

    case REG_PRM_LEAKAGE_CT4_TYPE:
      if( g_sys.mode == MODE_SETUP ){
        modbusbuf.reg_prm_leakage_ct_upper[3] = data;
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;
    case REG_PRM_LEAKAGE_CT4_TYPE+1:
      if( g_sys.mode == MODE_SETUP ){
        uint32_t tmp = modbusbuf.reg_prm_leakage_ct_upper[3];
        tmp = (tmp<<16) | (uint16_t)data;
        if( check_parameter( tmp, setup_min.ct_type[3], setup_max.ct_type[3] ) != 0 ){
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA; // 不正な値
        }else{
          g_setup.ct_type[3] = tmp ;
          setup_update();
        }
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;

    case REG_PRM_LEAKAGE_LOW_CUT:
      if( g_sys.mode == MODE_SETUP ){
        modbusbuf.reg_prm_leakage_low_cut_upper = data;
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;
    case REG_PRM_LEAKAGE_LOW_CUT+1:
      if( g_sys.mode == MODE_SETUP ){
        uint32_t tmp = modbusbuf.reg_prm_leakage_low_cut_upper;
        tmp = (tmp<<16) | (uint16_t)data;
        if( check_parameter( tmp, setup_min.leakage_low_cut, setup_max.leakage_low_cut ) != 0 ){
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA; // 不正な値
        }else{
          g_setup.leakage_low_cut = tmp ;
          setup_update();
        }
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;

    case REG_PRM_AVG_COUNT:
      if( g_sys.mode == MODE_SETUP ){
        modbusbuf.reg_cmd_avg_count_upper = data;
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;
    case REG_PRM_AVG_COUNT+1:
      if(g_sys.mode == MODE_SETUP ){
        uint32_t tmp = modbusbuf.reg_cmd_avg_count_upper;
        tmp = (tmp<<16) | (uint16_t)data;
        if( check_parameter( tmp, setup_min.avarage_count, setup_max.avarage_count ) != 0 ){
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA; // 不正な値
        }else{
          g_setup.avarage_count = tmp ;
          setup_update();
        }
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;

    case REG_PRM_UNIT_NO:
      if( g_sys.mode == MODE_SETUP ){
        modbusbuf.reg_prm_unit_no_upper = data;
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;
    case REG_PRM_UNIT_NO+1:
      if( g_sys.mode == MODE_SETUP ){
        uint32_t tmp = modbusbuf.reg_prm_unit_no_upper;
        tmp = (tmp<<16) | (uint16_t)data;
        if( check_parameter( tmp, setup_min.modbus_slave_address, setup_max.modbus_slave_address ) != 0 ){
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA; // 不正な値
        }else{
          g_setup.modbus_slave_address = tmp ;
          setup_update();
        }
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;

    case REG_PRM_BAUDRATE:
      if( g_sys.mode == MODE_SETUP ){
        modbusbuf.reg_prm_baudrate_upper = data;
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;
    case REG_PRM_BAUDRATE+1:
      if( g_sys.mode == MODE_SETUP ){
        uint32_t tmp = modbusbuf.reg_prm_baudrate_upper;
        tmp = (tmp<<16) | (uint16_t)data;
        if( check_parameter( tmp, setup_min.rs485_baudrate, setup_max.rs485_baudrate ) != 0 ){
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA; // 不正な値
        }else{
          g_setup.rs485_baudrate = tmp ;
          setup_update();
        }
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;

    case REG_PRM_DATA_BIT:
      if( g_sys.mode == MODE_SETUP ){
        modbusbuf.reg_prm_data_bit_upper = data;
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;
    case REG_PRM_DATA_BIT+1:
      if( g_sys.mode == MODE_SETUP ){
        uint32_t tmp = modbusbuf.reg_prm_data_bit_upper;
        tmp = (tmp<<16) | (uint16_t)data;
        if( check_parameter( tmp, setup_min.rs485_bit_length, setup_max.rs485_bit_length ) != 0 ){
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA; // 不正な値
        }else{
          g_setup.rs485_bit_length = tmp ;
          setup_update();
        }
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;

    case REG_PRM_STOP_BIT:
      if( g_sys.mode == MODE_SETUP ){
        modbusbuf.reg_prm_rs485_stop_bit_upper = data;
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;
    case REG_PRM_STOP_BIT+1:
      if( g_sys.mode == MODE_SETUP ){
        uint32_t tmp = modbusbuf.reg_prm_rs485_stop_bit_upper;
        tmp = (tmp<<16) | (uint16_t)data;
        if( check_parameter( tmp, setup_min.rs485_stop_bit, setup_max.rs485_stop_bit ) != 0 ){
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA; // 不正な値
        }else{
          g_setup.rs485_stop_bit = tmp ;
          setup_update();
        }
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;

    case REG_PRM_PARITY:
      if( g_sys.mode == MODE_SETUP ){
        modbusbuf.reg_prm_parity_upper = data;
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;
    case REG_PRM_PARITY+1:
      if( g_sys.mode == MODE_SETUP ){
        uint32_t tmp = modbusbuf.reg_prm_parity_upper;
        tmp = (tmp<<16) | (uint16_t)data;
        if( check_parameter( tmp, setup_min.rs485_parity, setup_max.rs485_parity ) != 0 ){
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA; // 不正な値
        }else{
          g_setup.rs485_parity = tmp ;
          setup_update();
        }
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;

    case REG_PRM_TX_WAIT_TIME:
      if( g_sys.mode == MODE_SETUP ){
        modbusbuf.reg_prm_tx_wait_timer_upper = data;
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;
    case REG_PRM_TX_WAIT_TIME+1:
      if( g_sys.mode == MODE_SETUP ){
        uint32_t tmp = modbusbuf.reg_prm_tx_wait_timer_upper;
        tmp = (tmp<<16) | (uint16_t)data;
        if( check_parameter( tmp, setup_min.response_delay_ms, setup_max.response_delay_ms ) != 0 ){
          ret = EXCEPTION_CODE_UNACCEPTABLE_DATA; // 不正な値
        }else{
          g_setup.response_delay_ms = tmp ;
          setup_update();
        }
      }else{
        ret = EXCEPTION_CODE_ILLEGAL_FUNCTION; // 設定モード以外では書き込み不可
      }
      break;

    default:
    ret = EXCEPTION_CODE_ILLEGAL_ADDRESS;
      break;
  }
	return ret;
}




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
    case REG_INST_VOLTAGE_1:
      ;
      modbusbuf.reg_inst_voltage[0] = (uint32_t)(GetVInstValue(0)*10.0);
      *val = modbusbuf.reg_inst_voltage[0]>>16; // 上位16ビットを返す
      break;
    case REG_INST_VOLTAGE_1+1:
      *val = modbusbuf.reg_inst_voltage[0]&0xFFFF; // 下位16ビットを返す
      break;

    case REG_INST_VOLTAGE_2:
      modbusbuf.reg_inst_voltage[1] = (uint32_t)(GetVInstValue(1)*10.0); 
      *val = modbusbuf.reg_inst_voltage[1]>>16; // 上位16ビットを返す
      break;
    case REG_INST_VOLTAGE_2+1:
      *val = modbusbuf.reg_inst_voltage[1]&0xFFFF; // 下位16ビットを返す
      break;
    case REG_INST_VOLTAGE_3:
      modbusbuf.reg_inst_voltage[2] = (uint32_t)(GetVInstValue(2) *10.0);
      *val = modbusbuf.reg_inst_voltage[2]>>16; // 上位16ビットを返す
      break;
    case REG_INST_VOLTAGE_3+1:
      *val = modbusbuf.reg_inst_voltage[2]&0xFFFF; // 下位16ビットを返す
      break;

    case REG_INST_FREQUENCY_1:
      modbusbuf.reg_inst_frequency[0] = GetVFreq()*10.0f;
      *val = modbusbuf.reg_inst_frequency[0]>>16; // 上位16ビットを返す
      break;
     case REG_INST_FREQUENCY_1+1:
      *val = modbusbuf.reg_inst_frequency[0]&0xFFFF; // 下位16ビットを返す
      break;

    case REG_INST_TEMPERATURE_1:
      modbusbuf.reg_inst_temperature[0] = sampling_t.current_temp*10.0;;
      *val = modbusbuf.reg_inst_temperature[0]>>16; // 上位16ビットを返す
      break;
    case REG_INST_TEMPERATURE_1+1:
      *val = modbusbuf.reg_inst_temperature[0]&0xFFFF; // 下位16ビットを返す
      break;

    case REG_INST_LEAKAGE_1:
    modbusbuf.reg_inst_leakage[0] = GetLInstValue(0); //小数点以下切り捨て
      *val = modbusbuf.reg_inst_leakage[0]>>16; // 上位16ビットを返す
      break;
    case REG_INST_LEAKAGE_1+1:
      *val = modbusbuf.reg_inst_leakage[0]&0xFFFF; // 下位16ビットを返す
      break;

    case REG_INST_LEAKAGE_2:
      modbusbuf.reg_inst_leakage[1] = GetLInstValue(1); //小数点以下切り捨て
      *val = modbusbuf.reg_inst_leakage[1]>>16; // 上位16ビットを返す 
      break;
    
    case REG_INST_LEAKAGE_2+1:
      *val = modbusbuf.reg_inst_leakage[1]&0xFFFF; // 下位16ビットを返す
      break;

    case REG_INST_LEAKAGE_3:
      modbusbuf.reg_inst_leakage[2] = GetLInstValue(2); //小数点以下切り捨て
      *val = modbusbuf.reg_inst_leakage[2]>>16; // 上位16ビットを返す
      break;
    case REG_INST_LEAKAGE_3+1:
      *val = modbusbuf.reg_inst_leakage[2]&0xFFFF; // 下位16ビットを返す
      break;

    case REG_INST_LEAKAGE_4:
      modbusbuf.reg_inst_leakage[3] = GetLInstValue(3); //小数点以下切り捨て
      *val = modbusbuf.reg_inst_leakage[3]>>16; // 上位16ビットを返す
      break;
    case REG_INST_LEAKAGE_4+1:
      *val = modbusbuf.reg_inst_leakage[3]&0xFFFF; // 下位16ビットを返す
      break;


    case REG_MAX_VOLTAGE_1:
      modbusbuf.reg_max_voltage[0] = GetVMaxValue(0)*10.0f;
      *val = modbusbuf.reg_max_voltage[0]>>16; // 上位16ビットを返す
      break;  
    case REG_MAX_VOLTAGE_1+1:
      *val = modbusbuf.reg_max_voltage[0]&0xFFFF; // 下位16ビットを返す
      break;

    case REG_MAX_VOLTAGE_2:
      modbusbuf.reg_max_voltage[1] = GetVMaxValue(1)*10.0f;
      *val = modbusbuf.reg_max_voltage[1]>>16; // 上位16ビットを返す
      break;  
    case REG_MAX_VOLTAGE_2+1:
      *val = modbusbuf.reg_max_voltage[1]&0xFFFF; // 下位16ビットを返す
      break;

      case REG_MAX_VOLTAGE_3:
      modbusbuf.reg_max_voltage[ 2] = GetVMaxValue(2)*10.0f;
      *val = modbusbuf.reg_max_voltage[2]>>16; // 上位16ビットを返す
      break;
    case REG_MAX_VOLTAGE_3+1:
      *val = modbusbuf.reg_max_voltage[2]&0xFFFF; // 下位16ビットを返す
      break;

    case REG_MAX_LEAKAGE_1:
      modbusbuf.reg_max_leakage[0] = GetLMaxValue(0);//小数点以下切り捨て
      *val = modbusbuf.reg_max_leakage[0]>>16; // 上位16ビットを返す
      break;
    case REG_MAX_LEAKAGE_1+1:
      *val = modbusbuf.reg_max_leakage[0]&0xFFFF; // 下位16ビットを返す
      break;
    case REG_MAX_LEAKAGE_2:
      modbusbuf.reg_max_leakage[1] = GetLMaxValue(1);//小数点以下切り捨て
      *val = modbusbuf.reg_max_leakage[1]>>16; // 上位16ビットを返す
      break;
    case REG_MAX_LEAKAGE_2+1:
      *val = modbusbuf.reg_max_leakage[1]&0xFFFF; // 下位16ビットを返す
      break;
    case REG_MAX_LEAKAGE_3:
      modbusbuf.reg_max_leakage[2] = GetLMaxValue(2);//小数点以下切り捨て
      *val = modbusbuf.reg_max_leakage[2]>>16; // 上位16ビットを返す
      break;
    case REG_MAX_LEAKAGE_3+1:
      *val = modbusbuf.reg_max_leakage[2]&0xFFFF; // 下位16ビットを返す
      break;
    case REG_MAX_LEAKAGE_4:
      modbusbuf.reg_max_leakage[3] = GetLMaxValue(3);//小数点以下切り捨て  
      *val = modbusbuf.reg_max_leakage[3]>>16; // 上位16ビットを返す
      break;
    case REG_MAX_LEAKAGE_4+1:
      *val = modbusbuf.reg_max_leakage[3]&0xFFFF; // 下位16ビットを返す
      break;

    case REG_MIN_VOLTAGE_1:
      modbusbuf.reg_min_voltage[0] = GetVMinValue(0)*10.0f;
      *val = modbusbuf.reg_min_voltage[0]>>16; // 上位16ビットを返す
      break;
    case REG_MIN_VOLTAGE_1+1:
      *val = modbusbuf.reg_min_voltage[0]&0xFFFF; // 下位16ビットを返す
      break;
    case REG_MIN_VOLTAGE_2:
      modbusbuf.reg_min_voltage[1] = GetVMinValue(1)*10.0f;
      *val = modbusbuf.reg_min_voltage[1]>>16; // 上位16ビットを返す
      break;   
    case REG_MIN_VOLTAGE_2+1:
      *val = modbusbuf.reg_min_voltage[1]&0xFFFF; // 下位16ビットを返す
      break;
    case REG_MIN_VOLTAGE_3:
      modbusbuf.reg_min_voltage[2] = GetVMinValue(2)*10.0f;  
      *val = modbusbuf.reg_min_voltage[2]>>16; // 上位16ビットを返す
      break;
    case REG_MIN_VOLTAGE_3+1:
      *val = modbusbuf.reg_min_voltage[2]&0xFFFF; // 下位16ビットを返す
      break;

    case REG_MIN_LEAKAGE_1:
      modbusbuf.reg_min_leakage[0] = GetLMinValue(0);  //小数点以下切り捨て
      *val = modbusbuf.reg_min_leakage[0]>>16; // 上位16ビットを返す
      break;
    case REG_MIN_LEAKAGE_1+1:
      *val = modbusbuf.reg_min_leakage[0]&0xFFFF; // 下位16ビットを返す
      break;
    case REG_MIN_LEAKAGE_2:
      modbusbuf.reg_min_leakage[1] = GetLMinValue(1);//小数点以下切り捨て
      *val = modbusbuf.reg_min_leakage[1]>>16; // 上位16ビットを返す
      break;
    case REG_MIN_LEAKAGE_2+1:
      *val = modbusbuf.reg_min_leakage[1]&0xFFFF; // 下位16ビットを返す
      break;  
    case REG_MIN_LEAKAGE_3:
      modbusbuf.reg_min_leakage[2] = GetLMinValue(2);//小数点以下切り捨て
      *val = modbusbuf.reg_min_leakage[2]>>16; // 上位16ビットを返す
      break;
    case REG_MIN_LEAKAGE_3+1:
      *val = modbusbuf.reg_min_leakage[2]&0xFFFF; // 下位16ビットを返す
      break;
    case REG_MIN_LEAKAGE_4:
      modbusbuf.reg_min_leakage[3] = GetLMinValue(3);//小数点以下切り捨て
      *val = modbusbuf.reg_min_leakage[3]>>16; // 上位16ビットを返す
      break;
    case REG_MIN_LEAKAGE_4+1:
      *val = modbusbuf.reg_min_leakage[3]&0xFFFF; // 下位16ビットを返す
      break;
    case REG_VERSION:
      modbusbuf.reg_version = 0x00000000; // バージョン を表す値 (上位16ビットがメジャーバージョン、下位16ビットがマイナーバージョン)
      *val = 0x0000; // バージョン 1.00  を表す値
      break;
    case REG_VERSION+1:
      *val = 0x0100; // バージョン 1.00 を表す値
      break;
    case REG_STATUS:
      modbusbuf.reg_status = 0x00000000 ; // ステータス  を表す値 (例: 正常動作中)
      *val = modbusbuf.reg_status>>16; // 上位16ビットを返す
      break;
    case REG_STATUS+1:
      *val = modbusbuf.reg_status&0xFFFF; // 下位16ビットを返す
      break;

    case REG_PRM_SYS1_PHASE_WIRE:
      *val = g_setup.ac_phase_wire>>16; // 系統1 適用相線式 を表す値 (例: 0:単相2線, 1:単相3線, 2:三相3線, 3:三相4線)
      break;

    case REG_PRM_SYS1_PHASE_WIRE+1:
      *val = g_setup.ac_phase_wire&0xFFFF; // 系統1 適用相線式 を表す値 (例: 0:単相2線, 1:単相3線, 2:三相3線, 3:三相4線)
      break;
    case REG_PRM_SYS2_PHASE_WIRE:
      *val = 0; // 系統2 適用相線式 を表す値 (例: 0:単相2線, 1:単相3線, 2:三相3線, 3:三相4線)
      break;
    case REG_PRM_SYS2_PHASE_WIRE+1:
      *val = 0; // 系統2 適用相線式 を表す値 (例: 0:単相2線, 1:単相3線, 2:三相3線, 3:三相4線)
      break;
    case REG_PRM_BLK1_SYNC_SEL:
      *val = 0; // 計測ブロック1 同期選択 を表す値 (例: 0:系統1, 1:系統2)
      break;
    case REG_PRM_BLK1_SYNC_SEL+1:
      *val = 0; // 計測ブロック1 同期選択 を表す値 (例: 0:系統1, 1:系統2)
      break;
    case REG_PRM_LEAKAGE_CT1_TYPE:
      *val = g_setup.ct_type[0]>>16; // 漏電CT1タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break;
    case REG_PRM_LEAKAGE_CT1_TYPE+1:
      *val = g_setup.ct_type[0]&0xFFFF; // 漏電CT1タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break; 
    case REG_PRM_LEAKAGE_CT2_TYPE:
      *val = g_setup.ct_type[1]>>16; // 漏電CT2タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break;
    case REG_PRM_LEAKAGE_CT2_TYPE+1:
      *val = g_setup.ct_type[1]&0xFFFF; // 漏電CT2タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break; 
    case REG_PRM_LEAKAGE_CT3_TYPE:
      *val = g_setup.ct_type[2]>>16; // 漏電CT3タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break;
    case REG_PRM_LEAKAGE_CT3_TYPE+1:
      *val = g_setup.ct_type[2]&0xFFFF; // 漏電CT3タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break; 
    case REG_PRM_LEAKAGE_CT4_TYPE:
      *val = g_setup.ct_type[3]>>16; // 漏電CT4タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break;
    case REG_PRM_LEAKAGE_CT4_TYPE+1:
      *val = g_setup.ct_type[3]&0xFFFF; // 漏電CT4タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break; 
    case REG_PRM_LEAKAGE_CT5_TYPE:
      *val = 0; // 漏電CT5タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break;
    case REG_PRM_LEAKAGE_CT5_TYPE+1:
      *val = 0; // 漏電CT5タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break; 
    case REG_PRM_LEAKAGE_CT6_TYPE:
      *val = 0; // 漏電CT6タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break;
    case REG_PRM_LEAKAGE_CT6_TYPE+1:
      *val = 0; // 漏電CT6タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break; 
    case REG_PRM_LEAKAGE_CT7_TYPE:
      *val = 0; // 漏電CT7タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break;
    case REG_PRM_LEAKAGE_CT7_TYPE+1:
      *val = 0; // 漏電CT7タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break; 
    case REG_PRM_LEAKAGE_CT8_TYPE:
      *val = 0; // 漏電CT8タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break;
    case REG_PRM_LEAKAGE_CT8_TYPE+1:
      *val = 0; // 漏電CT8タイプ を表す値 (例: 0:クランプCT, 1:貫通CT)
      break; 

    case REG_PRM_LEAKAGE_LOW_CUT:
      modbusbuf.reg_prm_leakage_low_cut = (uint32_t)(g_setup.leakage_low_cut*10.0f); // 漏電ローカット電流値 0.1mA を表す値
      *val = modbusbuf.reg_prm_leakage_low_cut>>16; // 上位16ビットを返す
      break;
    case REG_PRM_LEAKAGE_LOW_CUT+1:
      *val = modbusbuf.reg_prm_leakage_low_cut & 0xFFFF; // 下位16ビットを返す
      break;

    case REG_PRM_AVG_COUNT:
      *val = 0;
      break;
    case REG_PRM_AVG_COUNT+1:
      *val = g_setup.avarage_count; // 平均化回数 を表す値 (例: 10)
      break;

    case REG_PRM_UNIT_NO:
      *val = g_setup.modbus_slave_address>>16; // ユニット番号 を表す値
      break;
    case REG_PRM_UNIT_NO+1:
      *val = g_setup.modbus_slave_address&0xFFFF; // ユニット番号 を表す値
      break;
    case REG_PRM_BAUDRATE:
      *val = g_setup.rs485_baudrate>>16; // 通信速度 を表す値 (例: 9600)
      break;
    case REG_PRM_BAUDRATE+1:
      *val = g_setup.rs485_baudrate&0xFFFF; // 通信速度 を表す値 (例: 9600)
      break;
    case REG_PRM_DATA_BIT:
      *val = g_setup.rs485_bit_length >>16; // データビット長 を表す値 (例: 8)
      break;
    case REG_PRM_DATA_BIT+1:
      *val = g_setup.rs485_bit_length & 0xFFFF; // データビット長 を表す値 (例: 8)
      break;
    case REG_PRM_STOP_BIT:
      *val = g_setup.rs485_stop_bit >>16; // ストップビット長 を表す値 (例: 1)
      break;
    case REG_PRM_STOP_BIT+1:
      *val = g_setup.rs485_stop_bit & 0xFFFF; // ストップビット長 を表す値 (例: 1)
      break;
    case REG_PRM_PARITY:
      *val = g_setup.rs485_parity >>16; // 垂直パリティ
      break;
    case REG_PRM_PARITY+1:
      *val = g_setup.rs485_parity & 0xFFFF; // 垂直パリティ
      break;  
    case REG_PRM_TX_WAIT_TIME:
      *val = g_setup.response_delay_ms >>16; // 送信待ち時間 を表す値 (例: 100ms)
      break;
    case REG_PRM_TX_WAIT_TIME+1:
      *val = g_setup.response_delay_ms & 0xFFFF; // 送信待ち時間 を表す値 (例: 100ms)
      break;  
    case REG_PRM_LINK_CONFIG:
      *val = 0; // 連結構成 を表す値 (例: 0: 非連結, 1: 連結)
      break;
    case REG_PRM_LINK_CONFIG+1:
      *val = 0; // 連結構成 を表す値 (例: 0: 非連結, 1: 連結)
      break;
    case REG_PRM_ATTR_READ_1:
      *val = 0; // 読み取り属性1 を表す値 (例: 0: 読み取り不可, 1: 読み取り可能)
      break;
    case REG_PRM_ATTR_READ_1+1:
      *val = 0; // 読み取り属性1 を表す値 (例: 0: 読み取り不可, 1: 読み取り可能)
      break;
    case REG_PRM_ATTR_READ_2:
      *val = 0; // 読み取り属性2 を表す値 (例: 0: 読み取り不可, 1: 読み取り可能)
      break;
    case REG_PRM_ATTR_READ_2+1:
      *val = 0; // 読み取り属性2 を表す値 (例: 0: 読み取り不可, 1: 読み取り可能)
      break;
    case REG_PRM_ATTR_READ_3:
      *val = 0; // 読み取り属性3 を表す値 (例: 0: 読み取り不可, 1: 読み取り可能)
      break;
    case REG_PRM_ATTR_READ_3+1:
      *val = 0; // 読み取り属性3 を表す値 (例: 0: 読み取り不可, 1: 読み取り可能)
      break;
    case REG_PRM_ATTR_READ_4:
      *val = 0; // 読み取り属性4 を表す値 (例: 0: 読み取り不可, 1: 読み取り可能)
      break;
    case REG_PRM_ATTR_READ_4+1:
      *val = 0; // 読み取り属性4  を表す値 (例: 0: 読み取り不可, 1: 読み取り可能)
      break;
    case REG_PRM_TIME_INFO_MD:
      modbusbuf.reg_prm_time_info_md = 0; // 時間情報（月日） を表す値 (例: 0xMMDD)
      *val = modbusbuf.reg_prm_time_info_md>>16; // 上位16ビットを返す
      break;
    case REG_PRM_TIME_INFO_MD+1:
      *val = modbusbuf.reg_prm_time_info_md&0xFFFF; // 下位16ビットを返す
      break;
    case REG_PRM_TIME_INFO_HMS: 
      modbusbuf.reg_prm_time_info_hms = 0; // 時間情報（時分） を表す値 (例: 0xHHMM)
      *val = modbusbuf.reg_prm_time_info_hms>>16; // 上位16ビットを返す
      break;  
    case REG_PRM_TIME_INFO_HMS+1:
      *val = modbusbuf.reg_prm_time_info_hms&0xFFFF; // 下位16ビットを返す
      break;
    default:
      *val = 0;
      break;
	}
	return ret;
}

