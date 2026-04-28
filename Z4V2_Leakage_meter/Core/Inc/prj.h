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

#include "stm32_def.h"
#include "cmsis_os.h"

#include "key.h"
#include "chlcd.h"
#include "uart_drv.h"
#include "setup.h"
#include "ver.h"

void apl_main(void);

// copy from main.c Periferal
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;

extern CRC_HandleTypeDef hcrc;

extern TIM_HandleTypeDef htim2; // Input Capture
extern TIM_HandleTypeDef htim3;     
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim15;

extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_tx;

///

void tsk_calc( void );
void tsk_ui( void );
void tsk_usb( void );
void GetADCRawValues( uint16_t *adc1_values,int num);
void GetVZValues( float *adc1_values,int num);


typedef struct{
    uint8_t dip_sw;
    uint8_t setup_update;
    uint8_t modbus_slave_address;
}sys_t;
extern sys_t g_sys;

extern osMessageQId queue_USBHandle;
extern osMessageQId queue_ADCHandle;

#define QSEL_IN1_CHANNEL   0x0000
#define QSEL_IN2_CHANNEL   0x0001
#define QSEL_IN3_CHANNEL   0x0002
#define QSEL_IN4_CHANNEL   0x0003
#define QSEL_IN12_CHANNEL   0x0004
#define QSEL_IN13_CHANNEL   0x0005
#define QSEL_TEMP_CHANNEL   0x0006
#define QSEL_VREF_CHANNEL   0x0007
#define QSEL_VBAT_CHANNEL   0x0008


#include "calc_leak.h"
#include "calc_volt.h"
#include "util.h"
#include "tsk_modbus.h"
#include "modbus_reg.h"
#endif /* INC_PRJ_H_ */
