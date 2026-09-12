/// @file   calc_volt.c
/// @brief   ADCデータから漏電計算
/// @author  y.sugawara
/// @date    2026/04/19
/// @version 1.0 

#include "prj.h"
#include <math.h>




typedef struct{
	uint16_t sec_cnt;	// 1秒カウント

	int32_t adc_center_total[2];
	uint16_t adc_total_centor_seccnt;
	float adc_center[2];	//adcのセンターライン

	uint16_t adc_total_seccnt;
	float adc_total[3];
/*
	float vol[3];
	float vol_max[3];
	float vol_min[3];
*/
	uint8_t cur_max_send_flg;	// 0: not send, 1: send
	uint8_t adc_center_first_flg;

}T_AC;

static T_AC tAc;



//電圧の換算定数
#define CONST_A (0.00007503242452f/0.2668607472f)
#define CONST_B	0
#define BASE_NOISE 0 //85518 
#define	CONST_ROOT2	1.41421356f




float CnvVolAbs( int32_t cur_ad_integ, float timsec )
{
	float fans;
	fans = (((float)cur_ad_integ/timsec)-BASE_NOISE);
	if(fans < 0.0f){
		fans = 0.0f;
	}else{
		fans  = CONST_A*fans;
	}
	return fans;
}


///
// ±そのままの出力変換
float CnvVolBi( int32_t cur_ad_integ, float timsec )
{
	float fans;
	fans = (((float)cur_ad_integ/timsec)-BASE_NOISE);
	fans  = CONST_A*fans;
	return fans;
}

/// @brief 
/// @param fcur 
/// @param timsec 
/// @return 
int32_t CnvAdcBi( float fcur , float timsec )
{
	int32_t ans;
	fcur = ( fcur / CONST_A ) ;
	ans = fcur * timsec;
	return ans;
}

/// @brief 
/// @param  
void Culc_vol_init(void)
{
	tAc.adc_total_centor_seccnt = 0;
	tAc.adc_center_first_flg = 1;
	for(int j = 0;j<2;j++){
		tAc.adc_center_total[j] = 0;
		tAc.adc_center[j] = 4096/2; // 4096	は　ADCの中点。　ADCの生値で、センサーに電流が流れていないときの値。　これを中心値として、差分を取る。
	}
	tAc.adc_total_seccnt = 0;
	for(int j = 0;j<3;j++){
		tAc.adc_total[j] = 0;
	}
}



/// @brief 
/// @param ct 
/// @param cur 
void cnv_2ct3cur( float *ct, float *cur )
{
	float s,t,r;
	r = *ct++;
	t = *ct++;
	s = 0 - r + t;
	cur[0] = r;
	cur[1] = s;
	cur[2] = t;
}

/// @brief 
/// @param adcv 
void adj_centor(int16_t *adcv)
{
	// center value の平均値を取る。　ADCのサンプリングレートは1秒間にI32_ADC_HZ回
	// ct センサーの数だけ計算する。
	tAc.adc_total_centor_seccnt++;
	for(int j = 0;j<2;j++){
		tAc.adc_center_total[j] += adcv[j] ; 
	}
	if(tAc.adc_total_centor_seccnt >= I32_ADC_HZ){
		tAc.adc_total_centor_seccnt = 0;
		for(int j = 0;j<2;j++){
			float center;			
			center = (float)tAc.adc_center_total[j]/I32_ADC_HZ;
			if(tAc.adc_center_first_flg ){	//　first time 
				tAc.adc_center[j] = center;				
			}else{
				if( (tAc.adc_center[j] - 1.0f) > center){//粗調整
					tAc.adc_center[j] -= 1.0f;
				}else if( (tAc.adc_center[j] + 1.0f) < center){//粗調整
					tAc.adc_center[j] += 1.0f;
				}else{	//微調整
					tAc.adc_center[j] = center;
				}
			}
			tAc.adc_center_total[j] = 0 ; 
		}
		tAc.adc_center_first_flg = 0;
	}
}

/// @brief 
/// @param cur 
/// @return 
static int culc_vol( float *cur , float *volt)
{
	int flg = 0;
	float vdda_scale = 1.0f;
	tAc.adc_total_seccnt++;
	for(int i = 0;i<3;i++){
		tAc.adc_total[i] += fabsf(cur[i]);
	}
	if( tAc.adc_total_seccnt >= V_ADC_HZ ){
		tAc.adc_total_seccnt = 0;
		flg = 1;


		vdda_scale = Calc_GetAdcVddaScale();
		for(int i = 0;i<3;i++){
			float f;
			f =  CnvVolAbs( tAc.adc_total[i], 0.1f	) * vdda_scale;	//0.1secで平均値をとる
			*volt++  = f;
			tAc.adc_total[i] = 0.0f;
		}
	}
	
	return flg;
}


/// @brief 
/// @param adcv :uint16_t data_ac[2] : ADCの生値。　センサーに電流が流れていないときの値を中心値として、差分を取る。
/// @param volt : float volt[3] : 電圧値の出力。　単位はV。　0: 1相目, 1: 2相目, 2: 3相目
/// @return 1: 計算完了, 0: 計算中 
int Culc_vol( int16_t *adcv, float *volt)
{
	int flg;
	float ct_2_diff[2] ;
	float ct_values_diff[3];

	for(int j = 0;j<2;j++){
		//　基準電圧Vcenterとの差分の絶対値を取る
		ct_2_diff[j] = (float)adcv[j] - tAc.adc_center[j];
	}

	cnv_2ct3cur(ct_2_diff, ct_values_diff);
	flg = culc_vol(ct_values_diff, volt);
	adj_centor(adcv);
	return flg;
}
