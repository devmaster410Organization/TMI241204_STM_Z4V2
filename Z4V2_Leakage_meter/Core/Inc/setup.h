/*
 * setup.h
 *
 *  Created on: Dec 19, 2021
 *      Author: user
 */

#ifndef INC_SETUP_H_
#define INC_SETUP_H_

#include "hal_flash.h"
typedef struct {
    float gain;   // a: 傾き
    float offset; // b: 切片
} calib_param_t;  // Calibration Parameterの略

typedef struct{
    uint8_t modbus_slave_address;
	uint8_t baud_rate;
	uint8_t stop_bit;
	uint8_t parity;
	uint8_t bit_length;
	uint16_t response_delay_ms;
	uint16_t check_sum;
	float leakage_low_cut; // 漏電ローカット電流値 (0.1～30.0mA)
	uint8_t ac_phase_wire; // 相線式 (0:単相2線, 1:単相3線, 2:三相3線, 3:三相4線)
	uint8_t ct_type[4]; // CTタイプ (0:クランプCT, 1:貫通CT) を表す値
	uint16_t avarage_count; // 平均化に使用するサンプル数 (例: 10)		

	uint8_t tcpDesconip[4];
	uint16_t tcpDesconPort;
	uint16_t tcpDescon_silent_timeout; // TCP切断のタイムアウト時間	
	//一次補正 y = ax + b の a,b
	calib_param_t volt_calib[2];
    calib_param_t leakage_calib[4];
	uint16_t sum;
}setup_t;
extern setup_t setup;
extern const setup_t default_setup;
extern const setup_t setup_max;
extern const setup_t setup_min;

int SETUP_write(  setup_t *ptsetup );
int SETUP_read(  setup_t *ptsetup );
#endif /* INC_SETUP_H_ */
