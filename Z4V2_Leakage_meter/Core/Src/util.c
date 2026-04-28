/*
 * util.c
 *
 *  Created on: Feb 8, 2026
 *      Author: ysuga
 */

#include <stdint.h>
#include "prj.h"

/* 構造体の定義例 */
typedef struct {
    uint32_t sample_rate;
    float gain;
    uint8_t mode;
    // 2KBを超えないように注意
} SetupConfig_t;

/* 特定のセクションに配置 */
__attribute__((section(".setup0")))
const SetupConfig_t setup0_data;

__attribute__((section(".setup1")))
const SetupConfig_t setup1_data;

sys_t g_sys;



/// @brief GetDsw関数は、DIPスイッチの状態を読み取るための関数です。各DIPスイッチの状態をビットマスクとして返します。
/// @param  void
/// @return uint8_t dsw - DIPスイッチの状態を表すビットマスク。各ビットは対応するDIPスイッチの状態を示します。
uint8_t GetDsw( void ) //DIPスイッチの状態を取得
{
	uint8_t dsw =0;
	if( PORT_READ(DSW_1) == GPIO_PIN_RESET)dsw |= BIT0;
	if( PORT_READ(DSW_2) == GPIO_PIN_RESET)dsw |= BIT1;
	if( PORT_READ(DSW_3) == GPIO_PIN_RESET)dsw |= BIT2;
	if( PORT_READ(DSW_4) == GPIO_PIN_RESET)dsw |= BIT3;

	return dsw;
}


