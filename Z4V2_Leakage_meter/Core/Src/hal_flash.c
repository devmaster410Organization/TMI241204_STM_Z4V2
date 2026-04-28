/*
 * hal_flash.c
 *
 *  Created on: 2021/11/29
 *      Author: ysuga
 */

#include "prj.h"
#include "stm32g4xx_hal_flash.h"

// Flashから読みだしたデータを退避するRAM上の領域
// 4byteごとにアクセスをするので、アドレスが4の倍数になるように配置する
//static uint8_t work_ram[BACKUP_FLASH_SECTOR_SIZE] __attribute__ ((aligned(4)));

// Flashのsectoe1を消去
bool Flash_clear(uint32_t sector ,uint32_t nbpage )
{
    FLASH_EraseInitTypeDef EraseInitStruct;

    if(sector < BACKUP_FLASH_SECTOR_TOP) return false;
    memset(&EraseInitStruct,0,sizeof(EraseInitStruct));
    HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef erase;
	erase.TypeErase=FLASH_TYPEERASE_PAGES;
	erase.Banks=FLASH_BANK_1;
	erase.Page=sector;
	erase.NbPages=nbpage;

	uint32_t error=0;
	HAL_FLASHEx_Erase(&erase, &error);



    HAL_FLASH_Lock();
    return  error == 0xFFFFFFFF;
}




// Flashのsector1の内容を全てwork_ramに読み出す
// work_ramの先頭アドレスを返す
uint32_t* Flash_load( uint32_t *dst_address,uint32_t *flash_address, uint32_t bytesz )
{

//	memcpy(work_ram, &_backup_flash_start, BACKUP_FLASH_SECTOR_SIZE);
	memcpy(dst_address, flash_address, bytesz);
    return dst_address;
}

// Flashのsector1を消去後、work_ramにあるデータを書き込む
bool Flash_store(uint32_t *flash_dsr_addr,uint64_t *src_addr,uint32_t bytesz )
{
    HAL_FLASH_Unlock();

    // work_ramにあるデータを4バイトごとまとめて書き込む
    HAL_StatusTypeDef result = HAL_OK;
    size_t write_cnt = bytesz / sizeof(uint64_t);
    if( bytesz % sizeof(uint64_t) ){
    	write_cnt++;
    }
    for (size_t i=0; i<write_cnt; i++)
    {
        result = HAL_FLASH_Program(
                    FLASH_TYPEPROGRAM_DOUBLEWORD,
                    (uint32_t)(flash_dsr_addr) + sizeof(uint64_t) * i,
					*src_addr
                );
        src_addr++;
        if (result != HAL_OK) break;
    }

    HAL_FLASH_Lock();

    return result == HAL_OK;
}
