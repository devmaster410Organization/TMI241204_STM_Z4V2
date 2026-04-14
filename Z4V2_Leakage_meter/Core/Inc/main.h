/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_DB7_Pin GPIO_PIN_13
#define LCD_DB7_GPIO_Port GPIOC
#define LCD_RS_Pin GPIO_PIN_14
#define LCD_RS_GPIO_Port GPIOC
#define LCD_E_Pin GPIO_PIN_15
#define LCD_E_GPIO_Port GPIOC
#define VAC1_Pin GPIO_PIN_0
#define VAC1_GPIO_Port GPIOA
#define VAC2_Pin GPIO_PIN_1
#define VAC2_GPIO_Port GPIOA
#define LCD_DB4_Pin GPIO_PIN_2
#define LCD_DB4_GPIO_Port GPIOA
#define LCD_DB5_Pin GPIO_PIN_3
#define LCD_DB5_GPIO_Port GPIOA
#define LCD_DB6_Pin GPIO_PIN_4
#define LCD_DB6_GPIO_Port GPIOA
#define LAC1_Pin GPIO_PIN_5
#define LAC1_GPIO_Port GPIOA
#define LAC2_Pin GPIO_PIN_6
#define LAC2_GPIO_Port GPIOA
#define LAC3_Pin GPIO_PIN_7
#define LAC3_GPIO_Port GPIOA
#define PB_SW1_Pin GPIO_PIN_0
#define PB_SW1_GPIO_Port GPIOB
#define PB_SW2_Pin GPIO_PIN_1
#define PB_SW2_GPIO_Port GPIOB
#define LAC4_Pin GPIO_PIN_2
#define LAC4_GPIO_Port GPIOB
#define DSW_3_Pin GPIO_PIN_12
#define DSW_3_GPIO_Port GPIOB
#define DSW_4_Pin GPIO_PIN_13
#define DSW_4_GPIO_Port GPIOB
#define GPI_PB15_Pin GPIO_PIN_15
#define GPI_PB15_GPIO_Port GPIOB
#define K_Pin GPIO_PIN_8
#define K_GPIO_Port GPIOA
#define DSW_1_Pin GPIO_PIN_9
#define DSW_1_GPIO_Port GPIOA
#define DSW_2_Pin GPIO_PIN_10
#define DSW_2_GPIO_Port GPIOA
#define VPH1_Pin GPIO_PIN_15
#define VPH1_GPIO_Port GPIOA
#define LPH1_Pin GPIO_PIN_3
#define LPH1_GPIO_Port GPIOB
#define PB_SW3_Pin GPIO_PIN_4
#define PB_SW3_GPIO_Port GPIOB
#define PB_SW4_Pin GPIO_PIN_5
#define PB_SW4_GPIO_Port GPIOB
#define LPH2_Pin GPIO_PIN_6
#define LPH2_GPIO_Port GPIOB
#define LPH3_Pin GPIO_PIN_7
#define LPH3_GPIO_Port GPIOB
#define LPH4_Pin GPIO_PIN_9
#define LPH4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
