/************************************************************************
 *	Copyright(C) Be-con Corporation 1997,1998 All rights Reserved.	*
 *									*
 *	Compiler    	: HEW / Renesas R8C				*
 *									*
 *	File name		: fifo.h				*
 *									*
 *	Description		: Fifo Manager				*
 *									*
 *	Revision history:						*
 *									*
 *	Version 1.00	: 2012/10/25	: 1st Coded			*
 *									*
 ************************************************************************/
#ifndef		_FIFO_H
#define		_FIFO_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t	*pBufTop;
	uint8_t	*pBufLim;
	uint8_t	*pRead;
	uint8_t	*pWrite;
	uint16_t	size;
	uint16_t	count;
}FIFO;

extern uint8_t		*AfifoVer( void );
extern void 		AfifoInit( FIFO *pFifo, uint8_t *pBuf, uint16_t size );
extern uint16_t 	AfifoCount( FIFO * );
extern bool		AfifoIns( FIFO *, uint8_t );
extern bool		AfifoPut( FIFO *, uint8_t );
extern bool		AfifoGet( FIFO *, uint8_t * );


typedef struct {
	uint16_t	*pBufTop ;
	uint16_t	*pBufLim ;
	uint16_t	*pRead ;
	uint16_t	*pWrite ;
	uint16_t	size ;
	uint16_t	count ;
} HFIFO ;

void	HfifoInit( HFIFO *pFifo, uint16_t *pBuf, uint16_t size ) ;
uint16_t	HfifoCount( HFIFO * ) ;
bool	HfifoIns( HFIFO *pFifo, uint16_t ) ;
bool	HfifoPut( HFIFO *, uint16_t ) ;
bool	HfifoGet( HFIFO *, uint16_t * ) ;





typedef struct {
	void	**ppBufTop ;
	void	**ppBufLim ;
	void	**ppRead ;
	void	**ppWrite ;
	uint16_t	size ;
	uint16_t	count ;
} PFIFO ;

extern void		ApfifoInit( PFIFO *pFifo, void **pBuf, uint16_t size ) ;
extern uint16_t	ApfifoCount( PFIFO *pFifo ) ;
extern bool		ApfifoIns( PFIFO *pFifo, void *pAdr ) ;
extern bool		ApfifoPut( PFIFO *pFifo, void *pAdr ) ;
extern bool		ApfifoGet( PFIFO *pFifo, void **pAdr ) ;

#endif

