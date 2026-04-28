/*
 * tsk_modbus.h
 *
 *  Created on: Apr 28, 2026
 *      Author: ysuga
 */
/// @file    modbus_reg.c
/// @brief   Modbus slave task 
/// @author  Y.Sugawara
/// @date    2026/4/28
/// @version 1.0

        

#ifndef INC_TSK_MODBUS_H_
#define INC_TSK_MODBUS_H_

#define MODBUS_UNIT_MAX	99

#define MBTCP_HEADER_SZ	6
#define MBTCP_LEN_POS	4

#define BLOAD_CAST_ID 0x00  //MODBUS

// 通信モード
#define XPMODE_WAIT   0
#define XPMODE_HEADER	1
#define XPMODE_BODY	2
#define	XPMODE_THRUEGH	3
#define XPMODE_ERROR	4

#define XPMODE_RECIEVING 5
#define XPMODE_WAIT_SILENT 6

// MOD BUS ファンクションコード
#define MODBUS_FUNC_READ_N  0x03
#define MODBUS_FUNC_WRITE_N  0x10
#define MODBUS_FUNC_WRITE_1 0x06
#define MODBUS_FUNC_ECOHBACK 0x08

#define MODBUS_FUNC_ECOHBACK_ERR 0x88

// MOD BUS errコード
#define EXCEPTION_CODE_OK 0x00
#define	EXCEPTION_CODE_ILLIGAL_FUNCTION 0x01
#define EXCEPTION_CODE_ILLIGAL_ADDRESS 0x02
#define EXCEPTION_CODE_UNACCEPTABLE_DATA 0x03
#define ERRCODE_04_DOSA_ERR 0x04

// MOD BUS ファンクションコードのパケット長
//#define MODBUS_FUNC_READ_N_LEN  (MBTCP_HEADER_SZ+6)
#define MODBUS_FUNC_WRITE_1_LEN 8
#define MODBUS_FUNC_ECOHBACK_LEN 8

void modbus_slave(void);


#endif /* INC_TSK_MODBUS_H_ */
