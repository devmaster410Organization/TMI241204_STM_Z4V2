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
UI_Disp_enum ui_show_adc( void );
UI_Disp_enum ui_show_value( void );
UI_Disp_enum ui_show_phase( void );
UI_Disp_enum ui_show_system( void );


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
        break;
      case UI_SHOW_VALUE:      
          ui_t.disp = ui_show_value();
        break;
      case UI_SHOW_PHASE:
        	ui_t.disp = ui_show_phase();
        break;
      case UI_SHOW_ADC:
        ui_t.disp = ui_show_adc();
        break;
      case UI_SHOW_SYSTEM:
          ui_t.disp = ui_show_system();
        break;    
      default:
        ui_t.disp = ui_show_ver();
        break;
      }
  }
}

/// @brief change baud_rate to  strings
/// @param baud_rate ()
/// @return 
const char* bps_string(uint8_t baud_rate)
{
  switch(baud_rate){
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
  UI_Disp_enum uie = UI_SHOW_VALUE;
  ChlcdPrint( 0, 0, "Z4V2 LeakageTester" );
  sprintf( (char*)lcd_str, "%s Build:%s", pVer,__DATE__ );
  ChlcdPrint( 0, 1, lcd_str );

  ChlcdPrint( 0, 2, "Techno MIRAI" );

  sprintf( (char*)lcd_str, "ID[%0d] %sBPS", setup.modbus_slave_address, bps_string(setup.baud_rate) );
  ChlcdPrint( 0, 3, lcd_str );
  for(int i=0;i<20;i++){
    ChlcdPrint( 0, 0, "Z4V2 LeakTester" );
    osDelay(99);
  }
  return uie;
}

#define ADC_NUM 6



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
    GetVValues(fval,3);
    sprintf( (char*)lcd_str, "V0 %5.1fV", fval[0] );
    ChlcdPrint( 0, 0, lcd_str );
    sprintf( (char*)lcd_str, "V1 %5.1fV ", fval[1] );
    ChlcdPrint( 0, 1, lcd_str );
    sprintf( (char*)lcd_str, "V2 %5.1fV", fval[2] );
    ChlcdPrint( 0, 2, lcd_str );

    GetVZValues(fval,ADC_NUM);
    sprintf( (char*)lcd_str, "V0 %5.1fV", fval[0] );
    ChlcdPrint( 0, 3, lcd_str );


    sprintf( (char*)lcd_str, "z0 %7.3f", fval[2] );
    ChlcdPrint( 10, 0, lcd_str );
    sprintf( (char*)lcd_str, "Z1 %7.3f", fval[3] );
    ChlcdPrint( 10, 1, lcd_str );
    sprintf( (char*)lcd_str, "Z2 %7.3f", fval[4] );
    ChlcdPrint( 10, 2, lcd_str );
    sprintf( (char*)lcd_str, "Z3 %7.3f", fval[5] );
    ChlcdPrint( 10, 3, lcd_str );
    osDelay( 99 );
    uint8_t keystat = KEY_pget();
    if( keystat == (K_MODE|K_ON) ){
      done = true;
    }  
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
