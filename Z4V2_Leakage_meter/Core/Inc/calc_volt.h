/// @file    calc_vol.c
/// @brief   ADCデータから漏電計算
/// @author  y.sugawara
/// @date    2026/04/19
/// @version 1.0 

#ifndef INC_CALC_VOLT_H_
#define INC_CALC_VOLT_H_

float CnvVolAbs( int32_t cur_ad_integ, float timsec );
float CnvVolBi( int32_t cur_ad_integ, float timsec );
int32_t CnvAdcBi( float fcur , float timsec );
void Culc_vol_init(void);
int Culc_vol( int16_t *data_ac);
void GetVValues( float *v,int num );
void Calc_ResetMinVoltValue(void);
void Calc_ResetMaxVoltValue(void);

void GetVMaxValues( float *v,int num );
void GetVMinValues( float *v,int num );



#endif /* INC_CALC_VOLT_H_ */
