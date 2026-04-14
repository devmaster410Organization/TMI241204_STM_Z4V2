/*
 * util.c
 *
 *  Created on: Feb 8, 2026
 *      Author: ysuga
 */
#include <stdint.h>

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
