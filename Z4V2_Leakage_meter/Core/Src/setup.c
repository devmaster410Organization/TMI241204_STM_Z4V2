/*
 * setup.c
 *
 *  Created on: 2024/07/25
 *      Author: skoum
 */



#include "prj.h"
setup_t g_setup;



extern uint32_t _backup_flash_start;	// sector 96-103 top address
extern uint32_t _backup_sub_flash_start;	//sector 104-111 top address

static setup_t tsetupwork __attribute__ ((aligned(4)));	// 関数に渡すときに uint32_t *なので

const setup_t setup_default = {
	.modbus_slave_address = 1,
	.baudrate = PRM_BAUDRATE_38400,
	.stop_bit = PRM_STOP_BIT_1,
	.parity = PRM_PARITY_EVEN,
	.bit_length = PRM_DATA_BIT_8,
	.response_delay_ms = 100,
	.leakage_low_cut = 0.1f,
	.ac_phase_wire = 0,
	.ct_type = {0,0,0,0},
	.avarage_count = 10,

	.tcpDesconip = {192,168,1,100},
	.tcpDesconPort = 502,
	.tcpDescon_silent_timeout = 30000,

	.volt_calib = {{1.0f,0.0f},{1.0f,0.0f}},
	.leakage_calib = {{1.0f,0.0f},{1.0f,0.0f},{1.0f,0.0f},{1.0f,0.0f}},
	.sum = 0
};	

const setup_t setup_max = {
	.modbus_slave_address = 1,
	.baudrate = PRM_BAUDRATE_115200,
	.stop_bit = PRM_STOP_BIT_2,
	.parity = PRM_PARITY_ODD,
	.bit_length = PRM_DATA_BIT_8,
	.response_delay_ms = 100,
	.leakage_low_cut = 0.1f,
	.ac_phase_wire = 0,
	.ct_type = {0,0,0,0},
	.avarage_count = 10,

	.tcpDesconip = {192,168,1,100},
	.tcpDesconPort = 502,
	.tcpDescon_silent_timeout = 30000,

	.volt_calib = {{1.0f,0.0f},{1.0f,0.0f}},
	.leakage_calib = {{1.0f,0.0f},{1.0f,0.0f},{1.0f,0.0f},{1.0f,0.0f}},
	.sum = 0
};	

const setup_t setup_min = {
	.modbus_slave_address = 1,
	.baudrate = PRM_BAUDRATE_9600,
	.stop_bit = PRM_STOP_BIT_1,
	.parity = PRM_PARITY_NONE,
	.bit_length = PRM_DATA_BIT_7,
	.response_delay_ms = 100,
	.leakage_low_cut = 0.1f,
	.ac_phase_wire = 0,
	.ct_type = {0,0,0,0},
	.avarage_count = 10,

	.tcpDesconip = {192,168,1,100},
	.tcpDesconPort = 502,
	.tcpDescon_silent_timeout = 30000,

	.volt_calib = {{1.0f,0.0f},{1.0f,0.0f}},
	.leakage_calib = {{1.0f,0.0f},{1.0f,0.0f},{1.0f,0.0f},{1.0f,0.0f}},
	.sum = 0
};	


//　サム (2バイト)付きで保存する。
int SETUP_write(  setup_t *ptsetup )
{
	bool boo,boo2;
	ptsetup->check_sum = ~AcalSUM((char*)ptsetup,(uint16_t)((char *)&ptsetup->check_sum - (char *)ptsetup));

	tsetupwork = *ptsetup;
	for(int i = BACKUP_FLASH_SECTOR_TOP; i<= BACKUP_FLASH_SECTOR_END ;i++){
		Flash_clear(i,1);
	}
	boo =  Flash_store( &_backup_flash_start ,(uint32_t*)&tsetupwork,sizeof(setup_t));

	for(int i = BACKUP_SUB_FLASH_SECTOR_TOP; i<= BACKUP_SUB_FLASH_SECTOR_END ;i++){
		Flash_clear(i,1);
	}
	
	boo2 =  Flash_store( &_backup_sub_flash_start,(uint32_t*)&tsetupwork,sizeof(setup_t));

	if(boo != true && boo2 != true){
		goto err;
	}

	return 1;
err:
	return 0;
}


//サム (2バイト)付きで保存する。
int SETUP_read(  setup_t *ptsetup )
{
	uint16_t check_sum;

	Flash_load((uint32_t)&tsetupwork,&_backup_flash_start,sizeof(setup_t) );
	check_sum = ~AcalSUM((char*)&tsetupwork,(char *)&tsetupwork.check_sum - (char *)&tsetupwork);

	if(check_sum != tsetupwork.check_sum){

		Flash_load((uint32_t)&tsetupwork,&_backup_sub_flash_start,sizeof(setup_t) );
		check_sum = ~AcalSUM((char*)&tsetupwork,(char *)&tsetupwork.check_sum - (char *)&tsetupwork);
		if(check_sum != tsetupwork.check_sum){
			goto err;
		}

	}
   *ptsetup = tsetupwork;

	return 1;
err:
	return 0;
}
