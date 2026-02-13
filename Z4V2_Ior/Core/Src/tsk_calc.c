/*
 * apl_tsk1.c
 *
 *  Created on: Jan 9, 2026
 *      Author: ysuga
 */


#include "prj.h"

#define FS_HZ          1800 // Sampling frequency is 1800hz
#define FRAME_SAMPLES  1   // 


#define ADC1_CH_NUM 3 // vref, temp, vbat
#define ADC2_CH_NUM 6 // ADCIN1,2,3,4,5,6

typedef struct{
  uint16_t en;
  int16_t buf[FRAME_SAMPLES*(ADC2_CH_NUM)];
} st_sample_buf;


#define SAMPLE_INDEX_MAX 12 // uint8_t の変数に入れるので最大255
typedef struct {
  /* ADC1: IN0..IN3 (4ch scan) */
  uint16_t adc1_buf[FRAME_SAMPLES * ADC1_CH_NUM ];

  int16_t adc2_buf[FRAME_SAMPLES * ADC2_CH_NUM ];

  HAL_StatusTypeDef hal_status_adc[4];
  osStatus osMessagePutStat;

  float current_vdda ;
  float current_temp ;
  float current_vbat ;

    // ... 既存のメンバ ...
  uint32_t  osMessagePutCount;
  uint32_t osMessagePutErrorCount;
  uint32_t osMessageGetCount;
  uint32_t osMessageGetTimeoutCount;
  uint32_t adc1_callback_count;
  uint32_t sdadc1_callback_count;
  uint32_t adc2_callback_count;
  uint32_t error_sample_buf_overrun_count;

  st_sample_buf sample_buf_t[SAMPLE_INDEX_MAX];
  uint8_t st_sample_buf_index;
  Leak1Hz_5060 leak1hz_t[ADC2_CH_NUM];
  float out_ma[ADC2_CH_NUM];

} st_sampling_cb;

st_sampling_cb sampling_t;


static void put_ad_ring( uint16_t sel, uint16_t adc_value );
static uint16_t get_ad_ring( uint16_t sel);


void tsk_culc( void );
static void init_sample_buf( void );
static void Start_Capture_Synced(void);
static void Start_ADC_DMA(void);
float Calculate_Vdda(uint32_t vrefint_adc_raw);
float Calculate_Temperature(uint32_t ts_adc_raw, float vdda) ;
void Process_ADC_Values(uint32_t raw_temp, uint32_t raw_vref) ;
void put_adc_all_queue( void );

static void calc_adc( uint16_t adsel,uint16_t adc_value );



/// @brief Calculate task
/// @param  
volatile uint8_t idxx;
void tsk_calc( void )
{
	uint8_t idx;
	init_sample_buf();
	Start_ADC_DMA();
	Start_Capture_Synced();
    
	sampling_t.hal_status_adc[0] = HAL_OK;
    sampling_t.hal_status_adc[1] = HAL_OK;
    sampling_t.hal_status_adc[2] = HAL_OK;
    sampling_t.hal_status_adc[3] = HAL_OK;
    sampling_t.osMessagePutStat = osOK;

	Leak1Hz_5060_Init( &sampling_t.leak1hz_t[QSEL_IN1_CHANNEL], FS_HZ, 1.0f, 0.30f, 0.10f, 0.995f, 1.30f );
	Leak1Hz_5060_Init( &sampling_t.leak1hz_t[QSEL_IN10_CHANNEL], FS_HZ, 1.0f, 0.30f, 0.10f, 0.995f, 1.30f );
#if 0	
	Leak1Hz_Init( &sampling_t.leak1hz_t[QSEL_IN3_CHANNEL], FS_HZ,1.263953774e-3 ,0.30f, 0.10f, 0.995f );
	Leak1Hz_Init( &sampling_t.leak1hz_t[QSEL_IN4_CHANNEL], FS_HZ,1.263953774e-3,0.30f, 0.10f, 0.995f );
	Leak1Hz_Init( &sampling_t.leak1hz_t[QSEL_IN13_CHANNEL], FS_HZ,1.263953774e-3, 0.30f, 0.10f, 0.995f );
	Leak1Hz_Init( &sampling_t.leak1hz_t[QSEL_IN17_CHANNEL], FS_HZ,1.263953774e-3 ,0.30f, 0.10f, 0.995f );
#endif
	Leak1Hz_5060_Init( &sampling_t.leak1hz_t[QSEL_IN3_CHANNEL], FS_HZ,0.000539069995 ,0.30f, 0.10f, 0.995f, 1.30f );
	Leak1Hz_5060_Init( &sampling_t.leak1hz_t[QSEL_IN4_CHANNEL], FS_HZ,0.000539069995,0.30f, 0.10f, 0.995f, 1.30f );
	Leak1Hz_5060_Init( &sampling_t.leak1hz_t[QSEL_IN13_CHANNEL], FS_HZ,0.000539069995, 0.30f, 0.10f, 0.995f, 1.30f );
	Leak1Hz_5060_Init( &sampling_t.leak1hz_t[QSEL_IN17_CHANNEL], FS_HZ,0.000539069995,0.30f, 0.10f, 0.995f, 1.30f );
	
	for(;;){
//      Process_ADC_Values( sampling_t.adc1_buf[4], sampling_t.adc1_buf[5] );
		sampling_t.osMessageGetCount++;

		uint8_t msg_prio;
		uint8_t msg;
		osStatus_t status = osMessageQueueGet(queue_ADCHandle,&msg,&msg_prio,1000); 
		PORT_HI(TP_PA15);

		switch( status ){
		case osOK:
			idx = msg;
			idxx = idxx;
			st_sample_buf *pbuf = &sampling_t.sample_buf_t[idx];
			if( idx < SAMPLE_INDEX_MAX ){
				int rslt;
				float f;
				rslt = Leak1Hz_5060_PushSamples( &sampling_t.leak1hz_t[QSEL_IN1_CHANNEL], &pbuf->buf[QSEL_IN1_CHANNEL], 1,&f);
				if(rslt == 1){
				sampling_t.out_ma[QSEL_IN1_CHANNEL] = f;
				}
				rslt = Leak1Hz_5060_PushSamples( &sampling_t.leak1hz_t[QSEL_IN10_CHANNEL], &pbuf->buf[QSEL_IN10_CHANNEL], 1,&f);
				if(rslt == 1){
					sampling_t.out_ma[QSEL_IN10_CHANNEL] = f;
				}

				rslt = Leak1Hz_5060_PushSamples( &sampling_t.leak1hz_t[QSEL_IN3_CHANNEL], &pbuf->buf[QSEL_IN3_CHANNEL], 1,&f);
				if(rslt == 1){
					sampling_t.out_ma[QSEL_IN3_CHANNEL] = f;
				}
				rslt = Leak1Hz_5060_PushSamples( &sampling_t.leak1hz_t[QSEL_IN4_CHANNEL], &pbuf->buf[QSEL_IN4_CHANNEL], 1,&f);
				if(rslt == 1){
					sampling_t.out_ma[QSEL_IN4_CHANNEL] = f;
				}
				rslt = Leak1Hz_5060_PushSamples( &sampling_t.leak1hz_t[QSEL_IN13_CHANNEL], &pbuf->buf[QSEL_IN13_CHANNEL], 1,&f);
				if(rslt == 1){
					sampling_t.out_ma[QSEL_IN13_CHANNEL] = f;
				}
				rslt = Leak1Hz_5060_PushSamples( &sampling_t.leak1hz_t[QSEL_IN17_CHANNEL], &pbuf->buf[QSEL_IN17_CHANNEL], 1,&f);
				if(rslt == 1){
					sampling_t.out_ma[QSEL_IN17_CHANNEL] = f;
				}
				pbuf->en = 0;
			}else{
				//
			}
			break;
		case osErrorTimeout:
			sampling_t.osMessageGetTimeoutCount++;
        	break;
		default:
        	break;
		}
		PORT_LO(TP_PA15);
    }
}


/// @brief 
/// @param  
static void init_sample_buf( void )
{
  memset( &sampling_t.sample_buf_t, 0, sizeof(sampling_t.sample_buf_t) );
  sampling_t.st_sample_buf_index = 0;
  
}


static void calc_adc( uint16_t sel,uint16_t adc_value )
{
  switch( sel ){
    case QSEL_IN1_CHANNEL:  // ADCIN0
        for(int i =0;i<1;i++){
            PORT_HI(TP_PA15); PORT_LO(TP_PA15);
        } 

      break;      
    case QSEL_IN3_CHANNEL:  // ADCIN1
        for(int i =0;i<2;i++){
            PORT_HI(TP_PA15); PORT_LO(TP_PA15);
        } 
      break;
    case QSEL_IN4_CHANNEL: // ADCIN2
        for(int i =0;i<2;i++){
            PORT_HI(TP_PA15); PORT_LO(TP_PA15);
        } 
      break;
    case QSEL_IN10_CHANNEL:  //SDADC1 IN4
        for(int i =0;i<4;i++){
            PORT_HI(TP_PA15); PORT_LO(TP_PA15);
        } 
      break;      
    case QSEL_IN13_CHANNEL:  //SDADC1 IN5
        for(int i =0;i<5;i++){
            PORT_HI(TP_PA15); PORT_LO(TP_PA15);
        } 
      break;
    default:
  
  }

}

// 例: TIM2=Master, TIM3=Slave とする

static void Start_Capture_Synced(void)
{
	// --- Slave側 Input Capture 開始（割り込み or DMA）---
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3);
	// 必要なら CH3/CH4 も

	// --- Master側 Input Capture も使うなら先に開始してOK ---
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_4);

	// --- カウンタ開始順：Slave → Master ---
	HAL_TIM_Base_Start(&htim1);  // Slave counter running (resetを待つ)
	HAL_TIM_Base_Start(&htim3);  // Master starts -> Update(TRGO)でSlaveがCNT=0に
}
 


uint16_t ccr_buf[10];



void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
    	ccr_buf[0] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
      // ...
    }else  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
    	ccr_buf[1]= HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
      // ...
    }else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
    	ccr_buf[2]= HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3 );
      // ...
    }else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
      ccr_buf[3] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4 );
      // ...
    }
    // CH2/CH3/CH4...
  }
  else if (htim->Instance == TIM1)
  {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
      ccr_buf[4] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
      // ...
    }
  }
}




static void Start_ADC_DMA(void)
{
  /* --- ADC1 calibration（起動時1回） --- */
  if (HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED) != HAL_OK) Error_Handler();
  if (HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED) != HAL_OK) Error_Handler();
  /* --- Start DMA (trigger-wait) --- */
  if ((sampling_t.hal_status_adc[0]=HAL_ADC_Start_DMA(&hadc1,(uint32_t*)sampling_t.adc1_buf, FRAME_SAMPLES * ADC1_CH_NUM)) != HAL_OK) Error_Handler();
  if ((sampling_t.hal_status_adc[1]=HAL_ADC_Start_DMA(&hadc2, (uint32_t*)sampling_t.adc2_buf, FRAME_SAMPLES * ADC2_CH_NUM)) != HAL_OK) Error_Handler();
 
  /* --- Start TIM4 (generate CC1 + CC4 at same moment) --- */
  if (HAL_TIM_Base_Start_IT(&htim7) != HAL_OK) Error_Handler();//ADC1 Trigger 1Sec
  if (HAL_TIM_Base_Start_IT(&htim8) != HAL_OK) Error_Handler();//ADC2 Trigger 3600SPS

}



/// adcリングバッファ関連
/// Vref,VBAT,

#define ADC_AVE_COUNT 5
#define ADC_RING_TEMP 0
#define ADC_RING_VREF 1
#define ADC_RING_VBAT 2
typedef struct{
  uint16_t adc_ave_count[3];
  uint16_t adbuf[3][ADC_AVE_COUNT];
}st_adc_ave;

st_adc_ave adc_ave_t = {
  .adc_ave_count = {0, 0, 0},
  .adbuf = {{0}, {0}, {0}}
};
void put_ad_ring( uint16_t sel, uint16_t adc_value )
{
  switch( sel ){
    case QSEL_TEMP_CHANNEL:  // 温度センサ
      sel = ADC_RING_TEMP;
      break;      
    case QSEL_VREF_CHANNEL:  // 内部基準電圧
      sel = ADC_RING_VREF;
      break;
    case QSEL_VBAT_CHANNEL:  // VBAT
      sel = ADC_RING_VBAT;
      break;
    default:
      return;

  }
  adc_ave_t.adbuf[sel][ adc_ave_t.adc_ave_count[sel] ] = adc_value;
  adc_ave_t.adc_ave_count[sel]++;
  if( adc_ave_t.adc_ave_count[sel] >= ADC_AVE_COUNT ){
    adc_ave_t.adc_ave_count[sel] = 0;
  }
} 

uint16_t get_ad_ring( uint16_t sel)
{
  uint32_t total = 0;
  switch( sel ){
    case QSEL_TEMP_CHANNEL:  // 温度センサ
      sel = ADC_RING_TEMP;
      break;      
    case QSEL_VREF_CHANNEL:  // 内部基準電圧
      sel = ADC_RING_VREF;
      break;
    case QSEL_VBAT_CHANNEL:  // VBAT
      sel = ADC_RING_VBAT;
      break;
    default:
      return 0;
  }
  for(int i=0;i<ADC_AVE_COUNT;i++){
    total += adc_ave_t.adbuf[sel][i];
  }
  return (uint16_t)(total / ADC_AVE_COUNT);

}




void put_adc_all_queue( void )
{

  for(int i =0;i<FRAME_SAMPLES;i++){
    st_sample_buf *pbuf = &sampling_t.sample_buf_t[sampling_t.st_sample_buf_index];
    if(pbuf->en != 0){
        sampling_t.error_sample_buf_overrun_count++;
    }else{
      int j = 0;
      for(int i = 0;i<FRAME_SAMPLES;i++){
        pbuf->buf[0] = sampling_t.adc2_buf[j++];
        pbuf->buf[1] = sampling_t.adc2_buf[j++];
        pbuf->buf[2] = sampling_t.adc2_buf[j++];
        pbuf->buf[3] = sampling_t.adc2_buf[j++];
        pbuf->buf[4] = sampling_t.adc2_buf[j++];
        pbuf->buf[5] = sampling_t.adc2_buf[j++];
        sampling_t.osMessagePutStat = osMessageQueuePut(queue_ADCHandle, &sampling_t.st_sample_buf_index, 0,0);
        sampling_t.osMessagePutCount++;
        if( sampling_t.osMessagePutStat != osOK ){
          sampling_t.osMessagePutErrorCount++;
        }
        sampling_t.st_sample_buf_index++;
        if( sampling_t.st_sample_buf_index >= SAMPLE_INDEX_MAX ){
          sampling_t.st_sample_buf_index = 0;
        }
      }
    }
  }

}


/* 全完了：adc1_buf[ADC_DMA_LEN/2 .. ADC_DMA_LEN - 1] が更新済み */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  if( hadc->Instance == ADC2 )
  {
PORT_TGL(TP_PA9);
    sampling_t.adc2_callback_count++;

    put_adc_all_queue( );
    PORT_LO(LD2);
  }else if (hadc->Instance == ADC1)
  {
PORT_TGL(LD2);
    put_ad_ring(QSEL_TEMP_CHANNEL, sampling_t.adc1_buf[0]);
    put_ad_ring(QSEL_VREF_CHANNEL, sampling_t.adc1_buf[1]);
    put_ad_ring(QSEL_VBAT_CHANNEL, sampling_t.adc1_buf[2]);
    sampling_t.adc1_callback_count++;
  }
}

/* エラー時（DMAエラーやオーバラン等） */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  if (hadc->Instance == ADC1)
  {
    /* エラー内容を確認 */
    uint32_t err = HAL_ADC_GetError(hadc);

    /* 例：ここでLED点灯やログ、リスタートなど */
    (void)err;
  }else if( hadc->Instance == ADC2){
    /* エラー内容を確認 */
    uint32_t err = HAL_ADC_GetError(hadc);

    /* 例：ここでLED点灯やログ、リスタートなど */
    (void)err;

  }
}

void adc_start( void )
{

  PORT_HI(LD2);
}

/* 校正データへのアクセス用マクロ */
#define TS_CAL1_ADDR        ((uint16_t*) ((uint32_t)0x1FFFF7B8))
#define TS_CAL2_ADDR        ((uint16_t*) ((uint32_t)0x1FFFF7C2))

/* STM32Fシリーズの多くは VBAT = ADC値 * 4 で計算されます */
#define VBAT_DIVIDER 4

/**
 * @brief VBAT電圧(V)を算出する関数
 * @param vbat_adc_raw VBATチャネルから取得したADC生値
 * @param vdda Calculate_Vddaで求めた現在の電源電圧(V)
 * @retval 算出されたVBAT電圧(V)
 */
float Calculate_Vbat(uint32_t vbat_adc_raw, float vdda) {
    /* 1. ADC生値を現在のVDDAに基づき電圧に変換 */
    /* 12bit ADC (4095.0f) を想定 */
    float vbat_measured = (float)vbat_adc_raw * vdda / 4095.0f;
    
    /* 2. 内部ブリッジ（分圧）を考慮して元の電圧を復元 */
    float vbat_val = vbat_measured * (float)VBAT_DIVIDER;
    
    return vbat_val;
}

/* 1. 内部基準電圧(VREFINT)から、現在の正確なVDDA電圧を算出する関数 */
float Calculate_Vdda(uint32_t vrefint_adc_raw) {
    // 3.3V(校正時電圧) * 校正値 / 現在の測定値
    return 3.3f * (float)(*VREFINT_CAL_ADDR) / (float)vrefint_adc_raw;
}

/* 2. 温度センサのADC値から、摂氏(℃)を算出する関数 */
float Calculate_Temperature(uint32_t ts_adc_raw, float vdda) {
    /* * ADC値はVDDAに依存するため、まず校正時の3.3V相当に正規化します。
     * もしVDDAが正確に3.3Vなら、ts_adc_raw_norm = ts_adc_raw です。
     */
    float ts_adc_raw_norm = (float)ts_adc_raw * vdda / 3.3f;
    
    float temperature = (110.0f - 30.0f) / (float)(*TS_CAL2_ADDR - *TS_CAL1_ADDR);
    temperature *= (ts_adc_raw_norm - (float)(*TS_CAL1_ADDR));
    temperature += 30.0f;
    
    return temperature;
}

/* --- メイン処理での使用イメージ --- */
void Process_ADC_Values(uint32_t raw_temp, uint32_t raw_vref) {
    // 1. まず現在の電源電圧(VDDA)を求める
    sampling_t.current_vdda = Calculate_Vdda(get_ad_ring(ADC_RING_VREF));
    
    // 2. VDDAを用いて正確な温度を求める
    sampling_t.current_temp = Calculate_Temperature(get_ad_ring(ADC_RING_TEMP), sampling_t.current_vdda);
    // current_temp が現在のチップ温度(℃)です

    // 3. VBAT電圧を求める
    sampling_t.current_vbat = Calculate_Vbat(get_ad_ring(ADC_RING_VBAT), sampling_t.current_vdda);
}

