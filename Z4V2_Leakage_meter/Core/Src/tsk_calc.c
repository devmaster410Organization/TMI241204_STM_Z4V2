/// @file   tsk_calc.c
/// @brief   漏電、電圧のサンプリング→計算
/// @author  y.sugawara
/// @date    2026/04/19
/// @version 1.0 

#include "prj.h"
#include "stm32g4xx_ll_adc.h"

#define FS_HZ          3600 // Sampling frequency is 1800hz
#define FRAME_SAMPLES  1   // 

#define ADC1_CH_NUM 3 // temp, vbat, vref
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
void Process_ADC_Values( void );
void put_adc_all_queue( void );
static void calc_adc( uint16_t adsel,uint16_t adc_value );



/// @brief Calculate task
/// @param  
volatile uint8_t idxx;
void tsk_calc( void )
{
	uint8_t idx;
	init_sample_buf();
  Culc_vol_init();  //vol
	Start_ADC_DMA();
	Start_Capture_Synced();
 	HAL_TIM_Base_Start_IT(&htim3);  // 1mSec タイマー
  HAL_TIM_Base_Start(&htim15);  // 10uSec カウンター
	sampling_t.hal_status_adc[0] = HAL_OK;
  sampling_t.hal_status_adc[1] = HAL_OK;
  sampling_t.hal_status_adc[2] = HAL_OK;
  sampling_t.hal_status_adc[3] = HAL_OK;
  sampling_t.osMessagePutStat = osOK;

	Leak1Hz_5060_Init( &sampling_t.leak1hz_t[QSEL_IN1_CHANNEL], FS_HZ, 0.00009639222287, 0.30f, 0.10f, 0.995f, 1.30f );
	Leak1Hz_5060_Init( &sampling_t.leak1hz_t[QSEL_IN2_CHANNEL], FS_HZ, 0.00009639222287, 0.30f, 0.10f, 0.995f, 1.30f );

#if 0  
	Leak1Hz_Init( &sampling_t.leak1hz_t[QSEL_IN3_CHANNEL], FS_HZ,1.263953774e-3 ,0.30f, 0.10f, 0.995f );
	Leak1Hz_Init( &sampling_t.leak1hz_t[QSEL_IN4_CHANNEL], FS_HZ,1.263953774e-3,0.30f, 0.10f, 0.995f );
	Leak1Hz_Init( &sampling_t.leak1hz_t[QSEL_IN13_CHANNEL], FS_HZ,1.263953774e-3, 0.30f, 0.10f, 0.995f );
	Leak1Hz_Init( &sampling_t.leak1hz_t[QSEL_IN17_CHANNEL], FS_HZ,1.263953774e-3 ,0.30f, 0.10f, 0.995f );
#endif
  Leak1Hz_5060_Init( &sampling_t.leak1hz_t[QSEL_IN3_CHANNEL], FS_HZ,4.059207897e-4 ,0.30f, 0.10f, 0.995f, 1.30f );
	Leak1Hz_5060_Init( &sampling_t.leak1hz_t[QSEL_IN4_CHANNEL], FS_HZ,2.690710247e-4 ,0.30f, 0.10f, 0.995f, 1.30f );
	Leak1Hz_5060_Init( &sampling_t.leak1hz_t[QSEL_IN12_CHANNEL], FS_HZ,2.690710247E-4, 0.30f, 0.10f, 0.995f, 1.30f );
	Leak1Hz_5060_Init( &sampling_t.leak1hz_t[QSEL_IN13_CHANNEL], FS_HZ,2.690710247E-4,0.30f, 0.10f, 0.995f, 1.30f );
	
	for(;;){
    Process_ADC_Values( );
		sampling_t.osMessageGetCount++;

		uint8_t msg_prio;
		uint8_t msg;
		osStatus_t status = osMessageQueueGet(queue_ADCHandle,&msg,&msg_prio,1000); 
//		PORT_HI(TP8);

		switch( status ){
		case osOK:
			idx = msg;
			idxx = idxx;
			st_sample_buf *pbuf = &sampling_t.sample_buf_t[idx];
			if( idx < SAMPLE_INDEX_MAX ){
				int rslt;
				float f;

        int16_t adcv[2] = { pbuf->buf[QSEL_IN1_CHANNEL], pbuf->buf[QSEL_IN2_CHANNEL] };
        Culc_vol( adcv );

        rslt = Leak1Hz_5060_PushSamples( &sampling_t.leak1hz_t[QSEL_IN1_CHANNEL], &pbuf->buf[QSEL_IN1_CHANNEL], 1,&f);
				if(rslt == 1){
				sampling_t.out_ma[QSEL_IN1_CHANNEL] = f;
				}
				rslt = Leak1Hz_5060_PushSamples( &sampling_t.leak1hz_t[QSEL_IN2_CHANNEL], &pbuf->buf[QSEL_IN2_CHANNEL], 1,&f);
				if(rslt == 1){
					sampling_t.out_ma[QSEL_IN2_CHANNEL] = f;
				}

        rslt = Leak1Hz_5060_PushSamples( &sampling_t.leak1hz_t[QSEL_IN3_CHANNEL], &pbuf->buf[QSEL_IN3_CHANNEL], 1,&f);
				if(rslt == 1){
					sampling_t.out_ma[QSEL_IN3_CHANNEL] = f;
				}
				rslt = Leak1Hz_5060_PushSamples( &sampling_t.leak1hz_t[QSEL_IN4_CHANNEL], &pbuf->buf[QSEL_IN4_CHANNEL], 1,&f);
				if(rslt == 1){
					sampling_t.out_ma[QSEL_IN4_CHANNEL] = f;
				}
				rslt = Leak1Hz_5060_PushSamples( &sampling_t.leak1hz_t[QSEL_IN12_CHANNEL], &pbuf->buf[QSEL_IN12_CHANNEL], 1,&f);
				if(rslt == 1){
					sampling_t.out_ma[QSEL_IN12_CHANNEL] = f;
				}
				rslt = Leak1Hz_5060_PushSamples( &sampling_t.leak1hz_t[QSEL_IN13_CHANNEL], &pbuf->buf[QSEL_IN13_CHANNEL], 1,&f);
				if(rslt == 1){
					sampling_t.out_ma[QSEL_IN13_CHANNEL] = f;
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
//		PORT_LO(TP8);
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
//            PORT_HI(TP8); PORT_LO(TP8);
        } 

      break;      
    case QSEL_IN3_CHANNEL:  // ADCIN1
        for(int i =0;i<2;i++){
//            PORT_HI(TP8); PORT_LO(TP8);
        } 
      break;
    case QSEL_IN4_CHANNEL: // ADCIN2
        for(int i =0;i<2;i++){
//            PORT_HI(TP8); PORT_LO(TP8);
        } 
      break;
    case QSEL_IN12_CHANNEL:  //SDADC1 IN4
        for(int i =0;i<4;i++){
//            PORT_HI(TP8); PORT_LO(TP8);
        } 
      break;      
    case QSEL_IN13_CHANNEL:  //SDADC1 IN5
        for(int i =0;i<5;i++){
//            PORT_HI(TP8); PORT_LO(TP8);
        } 
      break;
    default:
  
  }

}

// 例: TIM2=Master, TIM4=Slave とする

static void Start_Capture_Synced(void)
{
	// --- 同期確保：両タイマのCNTをゼロ化 ---
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_COUNTER(&htim4, 0);

	// --- UGイベントを発火してPSC反映 ---
//  HAL_TIM_GenerateEvent(&htim2, TIM_EVENTSOURCE_UPDATE);
//  HAL_TIM_GenerateEvent(&htim4, TIM_EVENTSOURCE_UPDATE);

	// --- 割り込み禁止で同時スタート（Slave → Master順） ---
	__disable_irq();
	HAL_TIM_Base_Start(&htim4);  // Slave start
	HAL_TIM_Base_Start(&htim2);  // Master start -> TRGOでSlaveが同期
	__enable_irq();

	// --- Input Capture開始（Slave → Master） ---
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);

	// --- Master側 Input Capture ---
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_4);

}
 


uint16_t ccr_buf[10];

uint16_t ccr_logp = 0;
uint16_t ccr_log[200];

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
    	ccr_buf[0] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
      // ...
    }else  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
    	ccr_buf[1]= HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
    }
    // CH2/CH3/CH4...
  }
  else if (htim->Instance == TIM4)
  {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
      ccr_buf[2] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
      // ...
    }else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
      ccr_buf[3] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
      // ...
    }else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
      ccr_buf[4] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
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
  uint16_t adbuf_ave[3];
}st_adc_ave;

st_adc_ave adc_ave_t = {
  .adc_ave_count = {0, 0, 0},
  .adbuf = {{0}, {0}, {0}},
  .adbuf_ave = {0, 0, 0}
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
  adc_ave_t.adbuf_ave[sel] = total / ADC_AVE_COUNT;
//  return (uint16_t)(total / ADC_AVE_COUNT);
  return adc_ave_t.adbuf_ave[sel];
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
  PORT_TGL(TP8);

    //PORT_TGL(TP_PA9);
    sampling_t.adc2_callback_count++;

    put_adc_all_queue( );
//    PORT_LO(LD2);
  }else if (hadc->Instance == ADC1)
  {
//PORT_TGL(LD2);
    put_ad_ring(QSEL_TEMP_CHANNEL, sampling_t.adc1_buf[0]);
    put_ad_ring(QSEL_VBAT_CHANNEL, sampling_t.adc1_buf[1]);
    put_ad_ring(QSEL_VREF_CHANNEL, sampling_t.adc1_buf[2]);
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

//  PORT_HI(LD2);
}

/* 校正データへのアクセス用マクロ */
//#define TS_CAL1_ADDR        ((uint16_t*) ((uint32_t)0x1FFFF7B8))
//#define TS_CAL2_ADDR        ((uint16_t*) ((uint32_t)0x1FFFF7C2))

/* STM32G4 internal VBAT channel measures Vbat/3 */
#define VBAT_DIVIDER 3

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
    if( vrefint_adc_raw == 0 ){
        return 0; // ゼロ除算防止
    }
  return (float)__LL_ADC_CALC_VREFANALOG_VOLTAGE(vrefint_adc_raw, LL_ADC_RESOLUTION_12B) / 1000.0f;
}

/* 2. 温度センサのADC値から、摂氏(℃)を算出する関数 */
float Calculate_Temperature(uint32_t ts_adc_raw, float vdda) {
  uint32_t vdda_mv = (uint32_t)(vdda * 1000.0f);

  if( vdda_mv == 0 ){
    return 0.0f;
  }

  return (float)__LL_ADC_CALC_TEMPERATURE(vdda_mv, ts_adc_raw, LL_ADC_RESOLUTION_12B);
}


float sysvdda,systemp,sysvbat;


/* --- メイン処理での使用イメージ --- */
void Process_ADC_Values( void ) {
    // 1. まず現在の電源電圧(VDDA)を求める
  sampling_t.current_vdda = Calculate_Vdda(get_ad_ring(QSEL_VREF_CHANNEL));
    
    // 2. VDDAを用いて正確な温度を求める
  sampling_t.current_temp = Calculate_Temperature(get_ad_ring(QSEL_TEMP_CHANNEL), sampling_t.current_vdda);
    // current_temp が現在のチップ温度(℃)です

    // 3. VBAT電圧を求める
    sampling_t.current_vbat = Calculate_Vbat(get_ad_ring(QSEL_VBAT_CHANNEL), sampling_t.current_vdda);
  sysvdda = sampling_t.current_vdda;
  systemp = sampling_t.current_temp;
  sysvbat = sampling_t.current_vbat;
  
  }


void GetADCRawValues( uint16_t *adc_values,int num)
{
  for(int i =0;i<num;i++){
	  adc_values[i] = sampling_t.adc2_buf[i];
  }
}



void GetVZValues( float *adc_values,int num)
{
    for(int i =0;i<num;i++){
    	adc_values[i] = sampling_t.out_ma[i];
    }
}
