/*
 * stmgpio.h
 *
 *  Created on: 2022/01/10
 *      Author: ysuga
 *
 *      STM32 Cube IDE で標準的に使う　定義をまとめもの
 *
 */

#ifndef INC_STM32_STD_H_
#define INC_STM32_STD_H_

#define _DI __disable_irq()
#define _EI __enable_irq()


// GPIO操作
// GPIO_PinState;
// High : GPIO_PIN_SET
// Low  : GPIO_PIN_RESET

#define PORT_HI(X) HAL_GPIO_WritePin(X ## _GPIO_Port , X ## _Pin ,GPIO_PIN_SET)
#define PORT_LO(X) HAL_GPIO_WritePin(X ## _GPIO_Port , X ## _Pin ,GPIO_PIN_RESET)
#define PORT_TGL(X) HAL_GPIO_TogglePin(X ## _GPIO_Port , X ## _Pin)

#define PORT_READ(X) HAL_GPIO_ReadPin( X ## _GPIO_Port , X ## _Pin)

// bit define
#define 	BIT0	0x01
#define 	BIT1	0x02
#define 	BIT2	0x04
#define 	BIT3	0x08
#define 	BIT4	0x10
#define 	BIT5	0x20
#define 	BIT6	0x40
#define 	BIT7	0x80
#define 	BIT8	0x100
#define 	BIT9	0x200
#define 	BIT10	0x400
#define 	BIT11	0x800
#define 	BIT12	0x1000
#define 	BIT13	0x2000
#define 	BIT14	0x4000
#define 	BIT15	0x8000

#endif /* INC_STM32_STD_H_ */
