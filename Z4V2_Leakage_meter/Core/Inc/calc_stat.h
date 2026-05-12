/*
 * calc_stat.h
 *
 *  Created on: May 12, 2026
 *      Author: ysuga
 */

#ifndef INC_CALC_STAT_H_
#define INC_CALC_STAT_H_

typedef struct{
  float volt_inst[3];
  float leak_inst[4];
}st_calc_stat;

extern st_calc_stat calc_stat_t;

void push_voltage_stat( float *volt );
void push_leakage_stat( uint16_t no, float leak );


#endif /* INC_CALC_STAT_H_ */
