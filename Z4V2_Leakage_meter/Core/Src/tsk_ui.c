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