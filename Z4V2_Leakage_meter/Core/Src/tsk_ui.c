/*
 * tsk_ui.c
 *
 *  Created on: Apr 14, 2026
 *      Author: ysuga
 */

#include "prj.h"
#include "stm32g4xx_ll_adc.h"


char lcd_str[ 32 ] ;




extern float sysvdda,systemp,sysvbat;

const char *pVer = "v1.00 26.04.14";


void tsk_ui( void )
{
  int cnt = 0;
  ChlcdInit();
  ChlcdCls();
    ChlcdPrint( 0, 0, (uint8_t*)"Z4V2 LeakTester" );
    ChlcdPrint( 0, 1, (uint8_t*)pVer );
    ChlcdPrint( 0, 2, (uint8_t*)"Techno MIRAI" );
    for(int i=0;i<50;i++){
      ChlcdPrint( 0, 0, (uint8_t*)"Z4V2 LeakTester" );
      osDelay(99);
    }
 
  ChlcdCls();
  for(;;){
    sprintf( (char*)lcd_str, "VDDA = %5.3f V", sysvdda );
    ChlcdPrint( 0, 0, (uint8_t*)lcd_str );


    sprintf( (char*)lcd_str, "TEMP = %5.1f C", systemp  );
    ChlcdPrint( 0, 1, (uint8_t*)lcd_str );


    sprintf( (char*)lcd_str, "VBAT = %5.3f V", sysvbat );
    ChlcdPrint( 0, 2, (uint8_t*)lcd_str );


    sprintf( (char*)lcd_str, "%8d",cnt++ );
    ChlcdPrint( 0, 3, (uint8_t*)lcd_str );
    osDelay( 0 );
  }
}