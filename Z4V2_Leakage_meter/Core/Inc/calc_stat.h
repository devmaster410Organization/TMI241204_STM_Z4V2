/*
 * calc_stat.h
 *
 *  Created on: May 12, 2026
 *      Author: ysuga
 */

#ifndef INC_CALC_STAT_H_
#define INC_CALC_STAT_H_

#define VOLT_CH_NUM 3
#define LEAK_CH_NUM 4
typedef struct{

  // Management section
  uint32_t volt_cancel_counter;
  uint32_t leakage_cancel_counter[LEAK_CH_NUM];

  uint32_t avarage_count; //平均化の回数

  float volt_total[VOLT_CH_NUM];
  uint32_t volt_total_count[VOLT_CH_NUM];
  float leak_total[LEAK_CH_NUM];
  uint32_t leak_total_count[LEAK_CH_NUM];

  uint8_t calc_leak1_range;// 漏電流のレンジ（0: 0-500mA 1:300-1000mA )

  // Result section
  float volt_inst[VOLT_CH_NUM];
  float leak_inst[LEAK_CH_NUM];

  float volt_max[VOLT_CH_NUM];
  float volt_min[VOLT_CH_NUM];  
  float leak_max[LEAK_CH_NUM];
  float leak_min[LEAK_CH_NUM];

  

}st_calc_stat;

extern st_calc_stat calc_stat_t;

void InitCalcStat( void );
void PushVoltageStat( float *volt );
void PushLeakageStat( uint16_t no, float leak );

float GetVInstValue( int ch );
float GetVMaxValue( int ch );
float GetVMinValue( int ch );
void ResetMinVoltValue(void);
void ResetMaxVoltValue(void);
float GetLInstValue( int ch );  
float GetLMaxValue( int ch );
float GetLMinValue( int ch );
void ResetMinLeakValue(void);
void ResetMaxLeakValue(void);

#endif /* INC_CALC_STAT_H_ */
