/************************************************************************
 *	Copyright(C) Be-con Corporation 1997,1998 All rights Reserved.	*
 *									*
 *	Compiler    	: STM32 HAL		*
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
#include <stdbool.h>
#include <stdint.h>
#include "fifo.h"


/*==============================================================*
 *			Const, Macro, type definition		*
 *==============================================================*/
static	const	uint8_t	pAfifoVer[] = "Afifo Ver1.00 1997/10/23" ;


/*==============================================================*
 *			Function prototypes			*
 *==============================================================*/
uint8_t	*AfifoVer( void );

// 8bitデータ用FIFOマネージャ
void		AfifoInit( FIFO *pFifo, uint8_t *pBuf, uint16_t size ) ;
uint16_t	AfifoCount( FIFO * );
bool		AfifoIns( FIFO *pFifo, uint8_t );
bool		AfifoPut( FIFO *, uint8_t ) ;
bool		AfifoGet( FIFO *, uint8_t * ) ;


//16bitデータ用FIFOマネージャ
void		HfifoInit( HFIFO *pFifo, uint16_t *pBuf, uint16_t size ) ;
uint16_t	HfifoCount( HFIFO * ) ;
bool		HfifoIns( HFIFO *pFifo, uint16_t ) ;
bool		HfifoPut( HFIFO *, uint16_t ) ;
bool		HfifoGet( HFIFO *, uint16_t * ) ;



//ポインタ用FIFOマネージャ
void		ApfifoInit( PFIFO *pFifo, void **pBuf, uint16_t size ) ;
uint16_t	ApfifoCount( PFIFO *pFifo ) ;
bool		ApfifoIns( PFIFO *pFifo, void *pAdr ) ;
bool		ApfifoPut( PFIFO *pFifo, void *pAdr ) ;
bool		ApfifoGet( PFIFO *pFifo, void **pAdr ) ;

/*==============================================================*
 *			Valiable Definition			*
 *==============================================================*/


/*----------------------------------------------------------------------*
 *		処理概要 ： Ｌｉｂバージョンを返す			*
 *		引    数 ： なし					*
 *		返    値 ： バージョン情報のポインタ			*
 *----------------------------------------------------------------------*/
uint8_t  *AfifoVer( void )
{
	return pAfifoVer ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： 使用Ｆｉｆｏ 初期処理			*
 *		引    数 ： pFifo : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ	*
 *		引    数 ： pBuf  : Ｆｉｆｏ Ｂｕｆｆｅｒ ポインタ	*
 *		引    数 ： size  : Ｆｉｆｏ Ｂｕｆｆｅｒ サイズ	*
 *		返    値 ： なし					*
 *----------------------------------------------------------------------*/
void	AfifoInit( FIFO *pFifo, uint8_t *pBuf, uint16_t size )
{
	pFifo->pBufTop = pBuf ;
	pFifo->pBufLim = pBuf+(size-1) ;
	pFifo->pRead = pBuf ;
	pFifo->pWrite = pBuf ;
	pFifo->size = size ;
	pFifo->count = 0 ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： Ｆｉｆｏに存在する数を返す			*
 *		引    数 ： pFifo : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ	*
 *		返    値 ： 存在する数					*
 *----------------------------------------------------------------------*/
uint16_t	AfifoCount( FIFO *pFifo )
{
	return pFifo->count ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： Ｆｉｆｏの先頭にデータを挿入する		*
 *		引    数 ： pFifo : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ	*
 *		引    数 ： data  : 挿入するデータ			*
 *		返    値 ： ＴＲＵＥ：成功　／　ＦＡＬＳＥ：失敗（ Fifo Full ）	*
 *----------------------------------------------------------------------*/
bool	AfifoIns( FIFO *pFifo, uint8_t data )
{
	if( pFifo->count < pFifo->size ){
		if( pFifo->pRead != pFifo->pBufTop )	pFifo->pRead-- ;
		else	pFifo->pRead = pFifo->pBufLim ;
		*pFifo->pRead = data;
		pFifo->count++;
		return true;
	}
	return false;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： Ｆｉｆｏにデータを入れる			*
 *		引    数 ： pFifo : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ	*
 *		引    数 ： data  : 入れるデータ			*
 *		返    値 ： ＴＲＵＥ：成功　／　ＦＡＬＳＥ：失敗（ Fifo Full ）	*
 *----------------------------------------------------------------------*/
bool	AfifoPut( FIFO *pFifo, uint8_t data )
{
	if( pFifo->count < pFifo->size ){
		*pFifo->pWrite = data ;
		pFifo->count++ ;
		if( pFifo->pWrite != pFifo->pBufLim ) pFifo->pWrite++ ;
		else	pFifo->pWrite = pFifo->pBufTop ;
		return true ;
	}
	return false ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： Ｆｉｆｏからデータを取り出す		*
 *		引    数 ： pFifo : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ	*
 *		引    数 ： data  : 取りだしたデータの格納場所		*
 *		返    値 ： ＴＲＵＥ：成功　／　ＦＡＬＳＥ：失敗（ Fifo Empty ）*
 *----------------------------------------------------------------------*/
bool	AfifoGet( FIFO *pFifo, uint8_t *data )
{
	if( pFifo->count != 0 ){
		*data = *pFifo->pRead ;
		pFifo->count-- ;
		if( pFifo->pRead != pFifo->pBufLim ) pFifo->pRead++ ;
		else	pFifo->pRead = pFifo->pBufTop ;
		return true ;
	}
	return false ;
}


/*----------------------------------------------------------------------*
 *		処理概要 ： 使用Ｆｉｆｏ 初期処理			*
 *		引    数 ： pHFIFO : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ	*
 *		引    数 ： pBuf  : Ｆｉｆｏ Ｂｕｆｆｅｒ ポインタ	*
 *		引    数 ： size  : Ｆｉｆｏ Ｂｕｆｆｅｒ サイズ	*
 *		返    値 ： なし					*
 *----------------------------------------------------------------------*/
void	HfifoInit( HFIFO *pHFIFO, uint16_t *pBuf, uint16_t size )
{
	pHFIFO->pBufTop = pBuf ;
	pHFIFO->pBufLim = pBuf+(size-1) ;
	pHFIFO->pRead = pBuf ;
	pHFIFO->pWrite = pBuf ;
	pHFIFO->size = size ;
	pHFIFO->count = 0 ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： Ｆｉｆｏに存在する数を返す							*
 *		引    数 ： pHFIFO : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ				*
 *		返    値 ： 存在する数											*
 *----------------------------------------------------------------------*/
uint16_t	HfifoCount( HFIFO *pHFIFO )
{
	return pHFIFO->count ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： Ｆｉｆｏの先頭にデータを挿入する					*
 *		引    数 ： pHFIFO : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ				*
 *		引    数 ： data  : 挿入するデータ								*
 *		返    値 ： ＴＲＵＥ：成功　／　ＦＡＬＳＥ：失敗（ HFIFO Full ）	*
 *----------------------------------------------------------------------*/
bool	HfifoIns( HFIFO *pHFIFO, uint16_t data )
{
	if( pHFIFO->count < pHFIFO->size ){
		if( pHFIFO->pRead != pHFIFO->pBufTop )	pHFIFO->pRead-- ;
		else									pHFIFO->pRead = pHFIFO->pBufLim ;
		*pHFIFO->pRead = data ;
		pHFIFO->count++ ;
		return true ;
	}
	return false ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： Ｆｉｆｏにデータを入れる							*
 *		引    数 ： pHFIFO : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ				*
 *		引    数 ： data  : 入れるデータ								*
 *		返    値 ： ＴＲＵＥ：成功　／　ＦＡＬＳＥ：失敗（ HFIFO Full ）	*
 *----------------------------------------------------------------------*/
bool	HfifoPut( HFIFO *pHFIFO, uint16_t data )
{
	if( pHFIFO->count < pHFIFO->size ){
		*pHFIFO->pWrite = data ;
		pHFIFO->count++ ;
		if( pHFIFO->pWrite != pHFIFO->pBufLim ) pHFIFO->pWrite++ ;
		else								  pHFIFO->pWrite = pHFIFO->pBufTop ;
		return true ;
	}
	return false ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： Ｆｉｆｏからデータを取り出す						*
 *		引    数 ： pHFIFO : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ				*
 *		引    数 ： data  : 取りだしたデータの格納場所					*
 *		返    値 ： ＴＲＵＥ：成功　／　ＦＡＬＳＥ：失敗（ HFIFO Empty ）*
 *----------------------------------------------------------------------*/
bool	HfifoGet( HFIFO *pHFIFO, uint16_t *data )
{
	if( pHFIFO->count != 0 ){
		*data = *pHFIFO->pRead ;
		pHFIFO->count-- ;
		if( pHFIFO->pRead != pHFIFO->pBufLim ) pHFIFO->pRead++ ;
		else								 pHFIFO->pRead = pHFIFO->pBufTop ;
		return true ;
	}
	return false ;
}

































/*----------------------------------------------------------------------*
 *		処理概要 ： 使用ポインタＦｉｆｏ 初期処理						*
 *		引    数 ： pFifo : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ				*
 *		引    数 ： ppBuf : Ｆｉｆｏ Ｂｕｆｆｅｒ ポインタ				*
 *		引    数 ： size  : Ｆｉｆｏ Ｂｕｆｆｅｒ サイズ				*
 *		返    値 ： なし												*
 *----------------------------------------------------------------------*/
void	ApfifoInit( PFIFO *pFifo, void **ppBuf, uint16_t size )
{
	pFifo->ppBufTop = ppBuf ;
	pFifo->ppBufLim = ppBuf+(size-1) ;
	pFifo->ppRead = ppBuf ;
	pFifo->ppWrite = ppBuf ;
	pFifo->size = size ;
	pFifo->count = 0 ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： Ｆｉｆｏに存在する数を返す							*
 *		引    数 ： pFifo : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ				*
 *		返    値 ： 存在する数											*
 *----------------------------------------------------------------------*/
uint16_t	ApfifoCount( PFIFO *pFifo )
{
	return pFifo->count ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： Ｆｉｆｏの先頭にデータを挿入する					*
 *		引    数 ： pFifo : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ				*
 *		引    数 ： pAdr  : 挿入するデータ								*
 *		返    値 ： ＴＲＵＥ：成功　／　ＦＡＬＳＥ：失敗（ Fifo Full ）	*
 *----------------------------------------------------------------------*/
bool	ApfifoIns( PFIFO *pFifo, void *pAdr )
{
	if( pFifo->count < pFifo->size ){
		if( pFifo->ppRead != pFifo->ppBufTop )	pFifo->ppRead-- ;
		else									pFifo->ppRead = pFifo->ppBufLim ;
		*pFifo->ppRead = pAdr ;
		pFifo->count++ ;
		return true ;
	}
	return false ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： Ｆｉｆｏにデータを入れる							*
 *		引    数 ： pFifo : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ				*
 *		引    数 ： pAdr  : 入れるデータ								*
 *		返    値 ： ＴＲＵＥ：成功　／　ＦＡＬＳＥ：失敗（ Fifo Full ）	*
 *----------------------------------------------------------------------*/
bool	ApfifoPut( PFIFO *pFifo, void *pAdr )
{
	if( pFifo->count < pFifo->size ){
		*pFifo->ppWrite = pAdr ;
		pFifo->count++ ;
		if( pFifo->ppWrite != pFifo->ppBufLim ) pFifo->ppWrite++ ;
		else									pFifo->ppWrite = pFifo->ppBufTop ;
		return true ;
	}
	return false ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： Ｆｉｆｏからデータを取り出す						*
 *		引    数 ： pFifo : Ｆｉｆｏ　Ｓｔｒｕｃｔ ポインタ				*
 *		引    数 ： pAdr  : 取りだしたデータの格納場所					*
 *		返    値 ： ＴＲＵＥ：成功　／　ＦＡＬＳＥ：失敗（ Fifo Empty ）*
 *----------------------------------------------------------------------*/
bool	ApfifoGet( PFIFO *pFifo, void **pAdr )
{
	if( pFifo->count != 0 ){
		*pAdr = *pFifo->ppRead ;
		pFifo->count-- ;
		if( pFifo->ppRead != pFifo->ppBufLim ) pFifo->ppRead++ ;
		else								   pFifo->ppRead = pFifo->ppBufTop ;
		return true ;
	}
	return false ;
}

