/// @file    prj.h
/// @brief   プロジェクト共通ヘッダ
/// @author  Y.Sugawara
/// @date    2026/2/9
/// @version 1.0



#ifndef INC_PRJ_H_
#define INC_PRJ_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fifo.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "stm32g4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "cmsis_os.h"
#include "usb_device.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <uart_drvsmpl.h>

#include "stm32_def.h"
#include "cmsis_os.h"

#include "key.h"
#include "chlcd.h"
#include "strcnv.h"
#include "setup.h"
#include "ver.h"

void apl_main(void);

// copy from main.c Periferal
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;

extern  CRC_HandleTypeDef hcrc;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim15;

extern UART_HandleTypeDef huart3;

#include "calc_leak.h"
#include "calc_volt.h"
#include "calc_stat.h"

///

void tsk_calc( void );
void tsk_ui( void );
void tsk_usb( void );
void tsk_modbus_slave( void );
void GetADCRawValues( uint16_t *adc1_values,int num);
void GetVZValues( float *adc1_values,int num);
float Calc_GetAdcVddaScale( void );

typedef enum{
    MODE_MEASURE = 0,
    MODE_SETUP
}sys_mode_t;

typedef struct{
    uint8_t dip_sw; //現在のDIPSWの状態
//-- setup
    uint8_t setup_update; // 0: no update, 1: update requested. g_setup.cのtsk_setup内で1にセットされる。tsk_calc内で1を検知したら、g_setup.cのtsk_setupに通知するために0に戻す。
    uint32_t setup_update_time;// setup_update == 1にした時のhal_tickの値
//-- mode
    sys_mode_t mode;  //
    sys_mode_t mode_next; // モード遷移先。mode_next != mode のとき、modeをmode_nextに切り替える


    uint16_t volt_cancel_counter;
    uint16_t leakage_cancel_counter[4]
}sys_t;
extern sys_t g_sys;

extern osMessageQId queue_USBHandle;
extern osMessageQId queue_ADCHandle;

// queue_USBHandle message format (16bit): [15:8]=source id, [7:0]=payload byte
#define USBMSG_SRC_USB     (0x01U)
#define USBMSG_PACK(src,ch)  ((uint16_t)((((uint16_t)(src) & 0xFFU) << 8) | ((uint16_t)(ch) & 0x00FFU)))
#define USBMSG_GET_SRC(msg)  ((uint8_t)((((uint16_t)(msg)) >> 8) & 0xFFU))
#define USBMSG_GET_CHAR(msg) ((uint8_t)(((uint16_t)(msg)) & 0x00FFU))



#define KE1_MIN_VOL 99999.9f
#define KE1_MAX_VOL 0.0f



#define QSEL_IN1_CHANNEL   0x0000
#define QSEL_IN2_CHANNEL   0x0001
#define QSEL_IN3_CHANNEL   0x0002
#define QSEL_IN4_CHANNEL   0x0003
#define QSEL_IN12_CHANNEL   0x0004
#define QSEL_IN13_CHANNEL   0x0005
#define QSEL_TEMP_CHANNEL   0x0006
#define QSEL_VREF_CHANNEL   0x0007
#define QSEL_VBAT_CHANNEL   0x0008


#define FS_HZ          (3600) // Sampling frequency is 1800hz
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
  uint32_t adc1_callback_count_last;
  uint32_t sdadc1_callback_count;
  uint32_t adc2_callback_count;
  uint32_t error_sample_buf_overrun_count;

  st_sample_buf sample_buf_t[SAMPLE_INDEX_MAX];
  uint8_t st_sample_buf_index;
  Leak100ms_5060 leak100ms_t[ADC2_CH_NUM];
  float out_ma[ADC2_CH_NUM];
  float out_ma_max[ADC2_CH_NUM];
  float out_ma_min[ADC2_CH_NUM];
  float k_ma[ADC2_CH_NUM];
  uint16_t v0_cycle_time;
  float V0Hz;
} st_sampling_cb;

extern st_sampling_cb sampling_t;
void Pase_init( void );
void Phase_push_edge( uint16_t no, uint16_t ccr ,GPIO_PinState state );
uint32_t Get_cycle_time( uint16_t no );
uint16_t GetVCycle( void );
float GetVFreq( void );
float Calc_GetAdcVddaScale( void );


#include "util.h"
#include "tsk_modbus.h"
#include "modbus_reg.h"

#endif /* INC_PRJ_H_ */
