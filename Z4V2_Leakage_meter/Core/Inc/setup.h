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


	uint8_t tcpDesconip[4];
	uint16_t tcpDesconPort;
	uint16_t tcpDescon_silent_timeout; // TCP切断のタイムアウト時間	

	calib_param_t volt_calib[2];
    calib_param_t leakage_calib[4];
	uint16_t sum;
}T_SETUP;
extern T_SETUP tSetup;

int SETUP_write(  T_SETUP *ptsetup );
int SETUP_read(  T_SETUP *ptsetup );
#endif /* INC_SETUP_H_ */
