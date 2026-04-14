// ST7066U Charactor LCD Driver
//

#pragma once

#include "main.h"
#include "stm32_def.h"

extern	uint8_t	*ChlcdVersion( void ) ;
extern	void	ChlcdInit( void ) ;
extern	void	ChlcdCls( void ) ;
extern	void	ChlcdPutchar( uint8_t c ) ;
extern	void	ChlcdPuts( uint8_t *str ) ;
extern	void	ChlcdLocate( uint8_t x , uint8_t y ) ;
extern	void	ChlcdSetcgram( uint8_t cgno , uint8_t *adr ) ;

extern	void	ChlcdPrint( uint16_t x, uint16_t y, uint8_t *str ) ;
extern	void 	ChlcdnPrint( uint16_t x, uint16_t y, uint8_t *str, uint16_t n ) ;
extern	void	ChlcdPutHex( uint16_t x, uint16_t y, uint16_t h, uint16_t keta ) ;
extern	void	ChlcdPutUdec( uint16_t x, uint16_t y, uint16_t ui, uint16_t keta ) ;
extern	void	ChlcdPutSdec( uint16_t x, uint16_t y, int16_t i, uint16_t keta ) ;
extern	void	ChlcdPutZdec( uint16_t x, uint16_t y, uint16_t ui, uint16_t keta ) ;

extern	void	ChlcdCursorOff( void ) ;
extern	void	ChlcdCursorOn( void ) ;

