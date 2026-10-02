/*
 * tsk_ui.c
 *
 *  Created on: Apr 14, 2026
 *      Author: ysuga
 */

#include "prj.h"
#include "stm32g4xx_ll_adc.h"

typedef enum{
  UI_SHOW_VER,
  UI_SHOW_MAIN,
  UI_SHOW_VALUE,
  UI_SHOW_ADC,
  UI_SHOW_PHASE,
  UI_SHOW_Z0,
  UI_SHOW_SYSTEM
}UI_Disp_enum;

typedef struct{
  UI_Disp_enum disp;

}UI_t;

UI_t ui_t;

char lcd_str[ 32 ] ;

extern float sysvdda,systemp,sysvbat;
const char *pVer = "v1.00 26.04.14";

UI_Disp_enum ui_show_ver( void );
UI_Disp_enum ui_show_main( void );
UI_Disp_enum ui_show_adc( void );
UI_Disp_enum ui_show_value( void );
UI_Disp_enum ui_show_phase( void );
UI_Disp_enum ui_show_z0( void );
UI_Disp_enum ui_show_system( void );

void setup_check( vodid );


/// @brief task ui
/// @param  void 
void tsk_ui( void )
{
  ChlcdInit();
  ChlcdCls();
  KEY_init();
  ui_t.disp = UI_SHOW_VER;
 	HAL_TIM_Base_Start_IT(&htim1);  // 1mSec タイマー (KeyScan用)

  for(;;){
    switch(ui_t.disp){
      case UI_SHOW_VER:
        ui_t.disp = ui_show_ver();
        ui_t.disp = UI_SHOW_MAIN;
        break;
      case UI_SHOW_MAIN:
        ui_t.disp = ui_show_main();
        ui_t.disp = UI_SHOW_VALUE;
        break;
      case UI_SHOW_VALUE:      
          ui_t.disp = ui_show_value();
        ui_t.disp = UI_SHOW_PHASE;
        break;
      case UI_SHOW_PHASE:
        	ui_t.disp = ui_show_phase();
        ui_t.disp = UI_SHOW_ADC;
        break;
      case UI_SHOW_ADC:
        ui_t.disp = ui_show_adc();
        ui_t.disp = UI_SHOW_Z0;
        break;
      case UI_SHOW_Z0:
          ui_t.disp = ui_show_z0();
        ui_t.disp = UI_SHOW_SYSTEM;
        break;    
      case UI_SHOW_SYSTEM:
          ui_t.disp = ui_show_system();
        ui_t.disp = UI_SHOW_MAIN;
        break;    
      default:
        ui_t.disp = ui_show_ver();
        ui_t.disp = UI_SHOW_MAIN;
        break;
      }
  }
}

/// @brief change baudrate to  strings
/// @param baudrate ()
/// @return 
const char* bps_string(uint8_t baudrate)
{
  switch(baudrate){
    case 0: return "9600";
    case 1: return "19200";
    case 2: return "38400";
    case 3: return "57600";
    case 4: return "115200";
    default: return "Unknown";
  }
}


/// @brief display version information
/// @param  void
/// @return next UI display state
UI_Disp_enum ui_show_ver( void )
{
  UI_Disp_enum uie = UI_SHOW_MAIN;
  ChlcdPrint( 0, 0, "Z4V2 LeakageTester" );
  sprintf( (char*)lcd_str, "%s Build:%s", pVer,__DATE__ );
  ChlcdPrint( 0, 1, lcd_str );

  ChlcdPrint( 0, 2, "Techno MIRAI" );

  sprintf( (char*)lcd_str, "ID[%03d] %sBPS", g_setup.modbus_slave_address, bps_string(g_setup.rs485_baudrate) );
  ChlcdPrint( 0, 3, lcd_str );
  for(int i=0;i<20;i++){
    ChlcdPrint( 0, 0, "Z4V2 LeakTester" );
    osDelay(99);
    setup_check();
  }
  return uie;
}

#define ADC_NUM 6



/// @brief display calculated values (e.g. voltage, impedance)  
/// @param  void
/// @return next UI display state
UI_Disp_enum ui_show_main( void )
{
  bool done = false;
  UI_Disp_enum uie = UI_SHOW_VALUE;
  float fval[ADC_NUM];

  ChlcdCls();
  KEY_clr();
  while( done == false ){
    if( g_sys.mode == MODE_SETUP ){
      sprintf( (char*)lcd_str, "SETUP  " );
    } else {
      sprintf( (char*)lcd_str, "MEASURE" );
    }
    ChlcdPrint( 0, 3, lcd_str );

    sprintf( (char*)lcd_str, "V0:%5.1fV", GetVInstValue(0) );
    ChlcdPrint( 0, 0, lcd_str );
    if( g_setup.ac_phase_wire != 0 ){ // 単相2線
      sprintf( (char*)lcd_str, "V1:%5.1fV ", GetVInstValue(1) );
      ChlcdPrint( 0, 1, lcd_str );
      sprintf( (char*)lcd_str, "V2:%5.1fV", GetVInstValue(2) );
      ChlcdPrint( 0, 2, lcd_str );
    }
/*    sprintf( (char*)lcd_str, "Freq:%4.1fHz", GetVFreq() );
    ChlcdPrint( 0, 3, lcd_str );  
*/


    sprintf( (char*)lcd_str, "Z0:%4dmA", (int)GetLInstValue(0) );
    ChlcdPrint( 10, 0, lcd_str );
    sprintf( (char*)lcd_str, "Z1:%4dmA", (int)GetLInstValue(1) );
    ChlcdPrint( 10, 1, lcd_str );
    sprintf( (char*)lcd_str, "Z2:%4dmA", (int)GetLInstValue(2)  );
    ChlcdPrint( 10, 2, lcd_str ); 
    sprintf( (char*)lcd_str, "Z3:%4dmA", (int)GetLInstValue(3) );
    ChlcdPrint( 10, 3, lcd_str );
    osDelay( 99 );
    uint8_t keystat = KEY_pget();
    if( keystat == (K_MODE|K_ON) ){
      done = true;
    }  
    setup_check();
  }
  return uie;
}


/// @brief display calculated values (e.g. voltage, impedance)  
/// @param  void
/// @return next UI display state
UI_Disp_enum ui_show_value( void )
{
  bool done = false;
  UI_Disp_enum uie = UI_SHOW_PHASE;
  float fval[ADC_NUM];

  ChlcdCls();
  KEY_clr();
  while( done == false ){
    
    sprintf( (char*)lcd_str, "V0 %5.1fV", GetVInstValue(0) );
    ChlcdPrint( 0, 0, lcd_str );
    sprintf( (char*)lcd_str, "V1 %5.1fV ", GetVInstValue(1) );
    ChlcdPrint( 0, 1, lcd_str );
    sprintf( (char*)lcd_str, "V2 %5.1fV", GetVInstValue(2) );
    ChlcdPrint( 0, 2, lcd_str );


    sprintf( (char*)lcd_str, "Z0 %7.3f", GetLInstValue(0) );
    ChlcdPrint( 10, 0, lcd_str );
    sprintf( (char*)lcd_str, "Z1 %7.3f", GetLInstValue(1) );
    ChlcdPrint( 10, 1, lcd_str );
    sprintf( (char*)lcd_str, "Z2 %7.3f", GetLInstValue(2)  );
    ChlcdPrint( 10, 2, lcd_str ); 
    sprintf( (char*)lcd_str, "Z3 %7.3f", GetLInstValue(3) );
    ChlcdPrint( 10, 3, lcd_str );
    osDelay( 99 );
    uint8_t keystat = KEY_pget();
    if( keystat == (K_MODE|K_ON) ){
      done = true;
    }  
    setup_check();
  }
  return uie;
}



/// @brief display ADC values
/// @param  void
/// @return next UI display state
UI_Disp_enum ui_show_adc( void )
{
  bool done = false;
  uint16_t adc_values[ADC_NUM];
  UI_Disp_enum uie = UI_SHOW_SYSTEM ;
  ChlcdCls();
  KEY_clr();
  while( done == false ){
    GetADCRawValues(adc_values , ADC_NUM);

    sprintf( (char*)lcd_str, "ADC%u:%4u", 1, adc_values[0] );
    ChlcdPrint( 0, 0, lcd_str );
    sprintf( (char*)lcd_str, "ADC%u:%4u", 2, adc_values[1] );
    ChlcdPrint( 0, 1, lcd_str );

    sprintf( (char*)lcd_str, "ADC%u:%4u", 3, adc_values[2] );
    ChlcdPrint( 10, 0, lcd_str );
    sprintf( (char*)lcd_str, "ADC%u:%4u", 4, adc_values[3] );
    ChlcdPrint( 10, 1, lcd_str );
    sprintf( (char*)lcd_str, "ADC%u:%4u", 5, adc_values[4] );
    ChlcdPrint( 10, 2, lcd_str );
    sprintf( (char*)lcd_str, "ADC%u:%4u", 6, adc_values[5] );
    ChlcdPrint( 10, 3, lcd_str );


    osDelay( 99 );
    uint8_t keystat = KEY_pget();
    if( keystat == (K_MODE|K_ON) ){
      done = true;
    }
    setup_check();

  }
  return uie;
}

const char wire_name[4][8] = { "1P2W", "1P3W", "3P3W" };

/// @brief 位相差と I0r をインプットキャプチャ方式(IC)と計算方式(DSP)で並べて表示する
///        I0 は共通（GetLInstValue）。UP/DOWN で CH 切替
///   0: CH1 1P2W I0 12.345mA
///   1:     Ph[deg] I0r[mA]
///   2: IC  -123.45  12.345      インプットキャプチャ（get_all_phase）
///   3: DSP -123.45  12.345      DFT 計算（calc_ior.c）
/// @param  void
/// @return next UI display state
UI_Disp_enum ui_show_z0( void )
{
  bool done = false;

  UI_Disp_enum uie = UI_SHOW_SYSTEM ;
  int ch = 0;
  ChlcdCls();
  KEY_clr();
  while( done == false ){
    sprintf((char*)lcd_str,"CH%1d %4s I0%7.3fmA",ch+1,wire_name[g_setup.ac_phase_wire],GetLInstValue(ch));
    ChlcdPrint( 0, 0, lcd_str );

    ChlcdPrint( 0, 1, "    Ph[deg] I0r[mA] " );

    // インプットキャプチャ方式: get_all_phase() の結果は leak100ms_t[NUM_LPHx + 1] に入っている
    const Leak100ms_5060 *pic = &sampling_t.leak100ms_t[ch + NUM_LPH1 + 1];
    float ic_deg = pic->rag * 180.0f / (float)M_PI;
    if( pic->diff_ccr_value >= 0 && isfinite(ic_deg) ){
      ic_deg = remainderf(ic_deg, 360.0f); // DSP 側と同じ ±180° に揃える
      sprintf((char*)lcd_str,"IC  %7.2f %7.3f ", ic_deg, pic->i0r);
    }else{
      sprintf((char*)lcd_str,"IC     ----    ---- ");  // 信号検知無し
    }
    ChlcdPrint( 0, 2, lcd_str );

    // 計算方式: 100ms DFT
    if( Ior_IsValid(ch) ){
      sprintf((char*)lcd_str,"DSP %7.2f %7.3f ", Ior_GetPhaseDeg(ch), Ior_GetI0r(ch));
    }else{
      sprintf((char*)lcd_str,"DSP    ----  (no V) ");
    }
    ChlcdPrint( 0, 3, lcd_str );


    osDelay( 99 );
    uint8_t keystat = KEY_pget();
    switch( keystat ){
      case K_MODE|K_ON:
        done = true;
        break;
      case K_UP|K_ON:
        // Handle the UP key press here
        ch = (ch + 1) % 4; // Move to the next channel
        ChlcdCls();
        break;
      case K_DOWN|K_ON:
        // Handle the DOWN key press here
        ch = (ch + 3) % 4; // Move to the previous channel
        ChlcdCls();
        break;
    }
    setup_check();

  }
  return uie;
}



UI_Disp_enum ui_show_phase( void )
{
  bool done = false;
  UI_Disp_enum uie = UI_SHOW_ADC;
  ChlcdCls();
  KEY_clr();
  while( done == false ){
      sprintf( (char*)lcd_str, "V0 Cycle:%5u", GetVCycle() );
      ChlcdPrint( 0, 0, lcd_str );

      sprintf( (char*)lcd_str, "V0 Freq:%6.3fHz", GetVFreq() );
      ChlcdPrint( 0, 1, lcd_str );  
    osDelay( 999 );
    uint8_t keystat = KEY_pget();
    if( keystat == (K_MODE|K_ON) ){
      done = true;
    }  
    setup_check();
  }
  return uie;
}


/// @brief display system information
/// @param  void
/// @return next UI display state
UI_Disp_enum ui_show_system( void )
{
  bool done = false;
  UI_Disp_enum uie = UI_SHOW_VALUE;
  ChlcdCls();
  while(done ==false ){
    sprintf( lcd_str, "VDDA = %5.3f V", sysvdda );
    ChlcdPrint( 0, 0, lcd_str );


    sprintf( (char*)lcd_str, "TEMP = %5.1f C", systemp  );
    ChlcdPrint( 0, 1, lcd_str );


    sprintf( (char*)lcd_str, "VBAT = %5.3f V", sysvbat );
    ChlcdPrint( 0, 2, lcd_str );

    sprintf( (char*)lcd_str, "[%02X]", GetDsw());
    ChlcdPrint( 16, 0,lcd_str );

    osDelay( 1 );


    sprintf( (char*)lcd_str, "%ld ",(long)( __HAL_TIM_GET_COUNTER(&htim2) -__HAL_TIM_GET_COUNTER(&htim4)) );
    ChlcdPrint( 0, 3,lcd_str );

    uint8_t keystat = KEY_pget();
    if( keystat  ){
      sprintf( (char*)lcd_str, "KEY:%02X", keystat );
      ChlcdPrint( 13, 3, lcd_str );

      if( keystat == (K_MODE|K_ON) ){
        done = true;
      }
    }
  }
  
  return uie;
}



void setup_check( vodid )
{
  if( g_sys.setup_update ){
    if( HAL_GetTick() - g_sys.setup_update_time > 1000 ){ // 1 second
      g_sys.setup_update = 0;
      SETUP_write(&g_setup );
    }
  }
}
