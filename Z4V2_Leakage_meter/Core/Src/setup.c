/*
 * setup.c
 *
 *  Created on: 2024/07/25
 *      Author: skoum
 */



#include "prj.h"

extern uint32_t _backup_flash_start;	// sector 96-103 top address
extern uint32_t _backup_sub_flash_start;	//sector 104-111 top address

static T_SETUP tsetupwork __attribute__ ((aligned(4)));	// 関数に渡すときに uint32_t *なので


//　サム (2バイト)付きで保存する。
int SETUP_write(  T_SETUP *ptsetup )
{
	bool boo,boo2;
	ptsetup->check_sum = ~AcalSUM((char*)ptsetup,(char *)&ptsetup->check_sum - (char *)ptsetup);

	tsetupwork = *ptsetup;
	for(int i = BACKUP_FLASH_SECTOR_TOP; i<= BACKUP_FLASH_SECTOR_END ;i++){
		Flash_clear(i,1);
	}
	boo =  Flash_store( &_backup_flash_start ,(uint32_t*)&tsetupwork,sizeof(T_SETUP));

	for(int i = BACKUP_SUB_FLASH_SECTOR_TOP; i<= BACKUP_SUB_FLASH_SECTOR_END ;i++){
		Flash_clear(i,1);
	}
	
	boo2 =  Flash_store( &_backup_sub_flash_start,(uint32_t*)&tsetupwork,sizeof(T_SETUP));

	if(boo != true && boo2 != true){
		goto err;
	}

	return 1;
err:
	return 0;
}


//サム (2バイト)付きで保存する。
int SETUP_read(  T_SETUP *ptsetup )
{
	uint16_t check_sum;

	Flash_load((uint32_t)&tsetupwork,&_backup_flash_start,sizeof(T_SETUP) );
	check_sum = ~AcalSUM((char*)&tsetupwork,(char *)&tsetupwork.check_sum - (char *)&tsetupwork);

	if(check_sum != tsetupwork.check_sum){

		Flash_load((uint32_t)&tsetupwork,&_backup_sub_flash_start,sizeof(T_SETUP) );
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
