/*
 * hal_flash.h
 *
 *  Created on: 2024/09/23
 *      Author: ysuga
 */

#ifndef INC_HAL_FLASH_H_
#define INC_HAL_FLASH_H_

#define BACKUP_FLASH_SECTOR_TOP     126
#define BACKUP_FLASH_SECTOR_END	    126
#define BACKUP_SUB_FLASH_SECTOR_TOP  127
#define BACKUP_SUB_FLASH_SECTOR_END  127

bool Flash_clear(uint32_t sector ,uint32_t nbpage );

uint32_t* Flash_load( uint32_t *dst_address,uint32_t *flash_address, uint32_t bytesz );

bool Flash_store(uint32_t *flash_dsr_addr,uint64_t *src_addr,uint32_t bytesz );

#endif /* INC_HAL_FLASH_H_ */
