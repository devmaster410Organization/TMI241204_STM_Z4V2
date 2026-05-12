/*
 * calc_stat.c
 *
 *  Created on: May 12, 2026
 *      Author: ysuga
 */




#include "prj.h"

void init_calc_stat( void )
{
  for(int i=0; i<3; i++){
    calc_stat_t.volt_inst[i] = 0.0f;
  }
  for(int i=0; i<4; i++){
    calc_stat_t.leak_inst[i] = 0.0f;
  }
}


void push_voltage_stat( float *volt )
{
    float vddascale = Calc_GetAdcVddaScale();
    if( g_sys.volt_cancel_counter > 0 ){
        g_sys.volt_cancel_counter--;
    }
    for(int i=0; i<3; i++){
        float f = volt[i];
		f = g_setup.volt_calib[i].gain * (f *vddascale) + g_setup.volt_calib[i].offset;
        calc_stat_t.volt_inst[i] = f;
    }
}   


void push_leakage_stat( uint16_t no, float leak )
{
    float vddascale = Calc_GetAdcVddaScale();
    if( g_sys.leakage_cancel_counter[no] > 0 ){
        g_sys.leakage_cancel_counter[no]--;
    }
	float f = leak;
	f = g_setup.leakage_calib[no].gain*(f * vddascale) + g_setup.leakage_calib[no].offset;
    calc_stat_t.leak_inst[no] = f;
}

