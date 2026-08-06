/// @file   calc_stat.c
/// @brief   取得した生データから、電圧、漏電の値を計算して保持するモジュール
/// @author  y.sugawara
/// @date    2026/04/19
/// @version 1.0 



#include "prj.h"

st_calc_stat calc_stat_t;

uint32_t get_average_count(uint32_t count_num);
static void set_range_leak1(uint8_t range);

static void set_range_leak1(uint8_t range)
{
    switch(range){
        case 0:
            PORT_HI(K);
            break;
        case 1:
            PORT_LO(K);
            break;
        default:
           PORT_HI(K);
            break;
    }
}

/// @brief　KE1の設定の番号と実際の平均化の回数の対応を取る関数
/// @param count_num KE1の設定の番号
/// @return 実際の平均化の回数
uint32_t get_average_count(uint32_t count_num)
{
    switch(count_num){
        case PRM_AVG_0:      return 1;  // 平均回数 0 (OFF)
        case PRM_AVG_2:      return 2;  // 平均回数 2
        case PRM_AVG_4:      return 4;  // 平均回数 4
        case PRM_AVG_8:      return 8;  // 平均回数 8
        case PRM_AVG_16:     return 16; // 平均回数 16
        case PRM_AVG_32:     return 32; // 平均回数 32
        case PRM_AVG_64:     return 64; // 平均回数 64
        case PRM_AVG_128:    return 128; // 平均回数 128
        case PRM_AVG_256:    return 256; // 平均回数 256
        case PRM_AVG_512:    return 512; // 平均回数 512
        case PRM_AVG_1024:   return 1024; // 平均回数 1024
        default: return 1;
    }
}

/// @brief 
/// @param  
void InitCalcStat( void )
{
  calc_stat_t.volt_cancel_counter = 60;
  for(int i=0; i<3; i++){
     calc_stat_t.volt_total_count[i] = 0;
    calc_stat_t.volt_inst[i] = 0.0f;
    calc_stat_t.volt_total[i] = 0.0f;
    calc_stat_t.volt_max[i] = KE1_MIN_VOL;
    calc_stat_t.volt_min[i] = KE1_MAX_VOL;
  }
  for(int i=0; i<4; i++){
    calc_stat_t.leak_inst[i] = 0.0f;
    calc_stat_t.leakage_cancel_counter[i] = 60;
    calc_stat_t.leak_total[i] = 0.0f;
    calc_stat_t.leak_total_count[i] = 0;
    calc_stat_t.leak_max[i] = KE1_MIN_LEAK;
    calc_stat_t.leak_min[i] = KE1_MAX_LEAK;
  }
   calc_stat_t.avarage_count = get_average_count(g_setup.avarage_count); //平均化の回数

   set_range_leak1(calc_stat_t.calc_leak1_range);


}

/// @brief 
/// @param volt 
/// @param fans 
/// @return 
int average_volt(uint16_t no,float volt, float *fans)
{
    int rslt = 0;
    calc_stat_t.volt_total[no] += volt;
    calc_stat_t.volt_total_count[no]++;
 PORT_TGL(TP8);
    if( calc_stat_t.volt_total_count[no] >= calc_stat_t.avarage_count ){
        *fans = calc_stat_t.volt_total[no] / (float)calc_stat_t.volt_total_count[no];
        calc_stat_t.volt_total[no] = 0.0f;
        calc_stat_t.volt_total_count[no] = 0;
        rslt = 1;
    }
    return rslt;
}

/// @brief 
/// @param no 
/// @param leak 
/// @param fans 
/// @return 
int avarage_leak(uint16_t no, float leak, float *fans)
{
    int rslt = 0;
    calc_stat_t.leak_total[no] += leak;
    calc_stat_t.leak_total_count[no]++;
    if( calc_stat_t.leak_total_count[no] >= calc_stat_t.avarage_count ){

          *fans = calc_stat_t.leak_total[no] / (float)calc_stat_t.leak_total_count[no];
        calc_stat_t.leak_total[no] = 0.0f;
        calc_stat_t.leak_total_count[no] = 0;
        rslt = 1;
    }
    return rslt;
}


/// @brief 
/// @param volt 
void PushVoltageStat( float *volt )
{
    int rslt;
    float vddascale = Calc_GetAdcVddaScale();
    if( calc_stat_t.volt_cancel_counter > 0 ){
        calc_stat_t.volt_cancel_counter--;
        goto endoffunc; //値の更新はしない
    }
    for(int i=0; i<3; i++){
        float f = volt[i];
        if((g_setup.ac_phase_wire == PRM_PHASE_WIRE_1P2W) && (i==1 || i==2)){
            calc_stat_t.volt_inst[i] = 0;
            calc_stat_t.volt_max[i] = 0;
            calc_stat_t.volt_min[i] = 0;
            continue;
        }

		f = g_setup.volt_calib[i].gain * (f *vddascale) + g_setup.volt_calib[i].offset;
        float fans;
        rslt = average_volt(i,f,&fans);
        if (rslt)
        {
            calc_stat_t.volt_inst[i] = fans;

            if(fans > calc_stat_t.volt_max[i]){
                calc_stat_t.volt_max[i] = fans;
            }
            if(fans < calc_stat_t.volt_min[i]){
                calc_stat_t.volt_min[i] = fans;
            }
        }
    }
    endoffunc:
}   

//#define RANGE_CHANGE_LEAK1_UPPER 20.0f
//#define RANGE_CHANGE_LEAK1_LOWER 10.0f

//#define RANGE_CHANGE_LEAK1_UPPER 2000.0f
//#define RANGE_CHANGE_LEAK1_LOWER 1000.0f
#define RANGE_CHANGE_LEAK1_UPPER 0000.0f
#define RANGE_CHANGE_LEAK1_LOWER 0000.0f


/// @brief 
/// @param no 
/// @param leak 
void PushLeakageStat( uint16_t no, float leak )
{
    int rslt;
    float g;
    float vddascale = Calc_GetAdcVddaScale();
    if( calc_stat_t.leakage_cancel_counter[no] > 0 ){
        calc_stat_t.leakage_cancel_counter[no]--;
        goto endoffunc; //値の更新はしない
    }
	float f = leak;
	float fans;
	f = g_setup.leakage_calib[no].gain*(f * vddascale) + g_setup.leakage_calib[no].offset;
    rslt = avarage_leak(no, f, &fans);
    if( rslt )
    {
        f = fans;
/*
        if( f <= g_setup.leakage_low_cut){ // low_cut以下は0とみなす
            f = 0.0f;
        }
*/

        calc_stat_t.leak_inst[no] = f;

        if(f > calc_stat_t.leak_max[no]){
            calc_stat_t.leak_max[no] = f;
        }
        if(f < calc_stat_t.leak_min[no]){
            calc_stat_t.leak_min[no] = f;
        }
    }

endoffunc:
}




/// @brief 
/// @param v 
/// @param num 
float GetVInstValue( int ch )
{  
    if( ch  >= VOLT_CH_NUM ){
        return 0.0f;
    }     
    return calc_stat_t.volt_inst[ch];
}

/// @brief 
/// @param v 
/// @param num 
float GetVMaxValue( int ch )
{
    if( ch  >= VOLT_CH_NUM ){
        return 0.0f;
    }     
    return calc_stat_t.volt_max[ch];
}


/// @brief 
/// @param v 
/// @param num 
float GetVMinValue( int ch )
{
    if( ch  >= VOLT_CH_NUM ){
        return 0.0f;
    }     
    return calc_stat_t.volt_min[ch]; 
}		


/// @brief 最小値の初期化
/// @param  
void ResetMinVoltValue(void)
{
	for(int j = 0;j<3;j++){
		calc_stat_t.volt_min[j] = KE1_MAX_VOL;
	}
}

/// @brief 最大値の初期化
/// @param  
void ResetMaxVoltValue(void)
{
	for(int j = 0;j<3;j++){
		calc_stat_t.volt_max[j] = KE1_MIN_VOL;
	}
}





/// @brief 
/// @param num 
float GetLInstValue( int ch )
{
    if( ch  >= LEAK_CH_NUM ){
        return 0.0f;
    }     
    return calc_stat_t.leak_inst[ch];
}

/// @brief 
/// @param v 
/// @param num 
float GetLMaxValue( int ch )
{
    if( ch  >= LEAK_CH_NUM ){
        return 0.0f;
    }     
    return calc_stat_t.leak_max[ch];
}


/// @brief 
/// @param v 
/// @param num 
float GetLMinValue( int ch )
{
    if( ch  >= LEAK_CH_NUM ){
        return 0.0f;
    }     
    return calc_stat_t.leak_min[ch]; 
}   

/// @brief 最小値の初期化
/// @param  
void ResetMinLeakValue(void)
{
	for(int j = 0;j<3;j++){
		calc_stat_t.leak_min[j] = KE1_MAX_LEAK;
	}
}

/// @brief 最大値の初期化
/// @param  
void ResetMaxLeakValue(void)
{
	for(int j = 0;j<3;j++){
		calc_stat_t.leak_max[j] = KE1_MIN_LEAK;
	}
}




