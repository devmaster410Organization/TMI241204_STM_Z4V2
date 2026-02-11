/// @file    prj.h
/// @brief   プロジェクト共通ヘッダ
/// @author  Y.Sugawara
/// @date    2026/2/9
/// @version 1.0



#ifndef INC_PRJ_H_
#define INC_PRJ_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "stm32_def.h"
#include "main.h"
#include "cmsis_os.h"


void apl_main(void);

// copy from main.c Periferal
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc2;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;

extern UART_HandleTypeDef huart2;

///

void tsk_calc( void );


extern osMessageQId queue_USBHandle;
extern osMessageQId queue_ADCHandle;

#define QSEL_IN1_CHANNEL   0x0000
#define QSEL_IN10_CHANNEL   0x0001
#define QSEL_IN3_CHANNEL   0x0002
#define QSEL_IN4_CHANNEL   0x0003
#define QSEL_IN13_CHANNEL   0x0004
#define QSEL_IN17_CHANNEL   0x0005
#define QSEL_TEMP_CHANNEL   0x0006
#define QSEL_VREF_CHANNEL   0x0007
#define QSEL_VBAT_CHANNEL   0x0008

#include "calc_leak.h"

#endif /* INC_PRJ_H_ */
