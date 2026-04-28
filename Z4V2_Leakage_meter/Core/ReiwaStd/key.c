/************************************************************************
	Copyright(C) Be-con Corporation 2007. All rights Reserved.
								
	Compiler    	: CH38 			
								
	File name	: EPS_KEY.c

	Description	: キー入力ルーチン
	Revision history:

	Version 0.01	: 1999/07/12 by Sugawara: 1st Coded

　ON/OFF,リピート二段階をサポートする。

 ************************************************************************/
#include "prj.h"
#include "key.h"
#include "fifo.h"

//関数プロトタイプ宣言

uint16_t KEY_cnt( void );
uint16_t KEY_Port( void );
static uint16_t scan_port( void );


//　定数宣言
#define		KEY_YSZ			(uint8_t)1
#define		KEY_XSZ			(uint8_t)4
#define 	KEY_OFF			(uint8_t)0
#define		KEY_ON			(uint8_t)1
#define 	CHAT_MAX		(uint8_t)3 // chata times

#define		KRPT_CNT		70	/* ﾘﾋﾟｰﾄになるまでの時間 */
#define		KRPT2_CNT		10 	/* 倍速リピートになるまでのリピート回数 */
#define		KRPT_INI		60	

#define BUFSZ 4
#define	SCAN_INTERVAL	10


//変数宣言・定数
FIFO	fifokey;
static	uint8_t fifokeybuf[BUFSZ];

static	uint8_t	ksstat[ KEY_YSZ ][ KEY_XSZ ] ;
static	uint8_t	kscnt[ KEY_YSZ ][ KEY_XSZ ] ;
static	uint8_t	ksrpt[ KEY_YSZ ][ KEY_XSZ ] ;
static  uint8_t	ksrpt2[ KEY_YSZ ][ KEY_XSZ ];

static	uint8_t	ycnt,scancnt;

static	const uint8_t kdata_on[ KEY_YSZ ][ KEY_XSZ ] ={
{K_MODE	| K_ON, K_UP | K_ON, K_DOWN | K_ON, K_ENT | K_ON},
};

static	const uint8_t kdata_rpt[ KEY_YSZ ][ KEY_XSZ ] ={
{K_MODE | K_RX10, K_UP | K_RX10, K_DOWN | K_RX10, K_ENT | K_RX10},
};

static	const uint8_t kdata_rpt2[ KEY_YSZ ][ KEY_XSZ ] ={
{K_MODE | K_RX20, K_UP | K_RX20, K_DOWN | K_RX20, K_ENT | K_RX20},
};


static	const uint8_t kdata_off[ KEY_YSZ ][ KEY_XSZ ] ={
{K_MODE | K_OFF, K_UP | K_OFF, K_DOWN | K_OFF, K_ENT | K_OFF},
};


/*----------------------------------------------------------------------*
 処理概要 ： キーに割り当てられているボートを一つ一つ確認して uint16_tデータに変換する
 引    数 ： void
 返    値 ： uint16_t　に成形されたポート情報
 *----------------------------------------------------------------------*/

static uint16_t scan_port( void )
{
	uint16_t uw = 0;
	
	if( PORT_READ(PB_SW1) == GPIO_PIN_RESET ){ // 
		uw |= K_MODE;
	}
	if( PORT_READ(PB_SW2) == GPIO_PIN_RESET ){ // 
		uw |= K_UP;
	}
	if( PORT_READ(PB_SW3) == GPIO_PIN_RESET ){ // 
		uw |= K_DOWN;
	}
	if( PORT_READ(PB_SW4) == GPIO_PIN_RESET ){ // 
		uw |= K_ENT;
	}
	return uw;


}

/*----------------------------------------------------------------------*
 処理概要 ： 割り込み内でキースキャンして、キー入力等の処理を行う
 引    数 ： void
 返    値 ： uint16_t　に成形されたポート情報
 *----------------------------------------------------------------------*/
// 割り込み処理内からコールされるので注意してください。
void	KeyScan( void )
{
	uint16_t fil,k,x;
	uint8_t *psstat,*pscnt,*psrpt,*psrpt2;
	

	--scancnt;
	if( scancnt == 0 ){
		scancnt = SCAN_INTERVAL;
		k = scan_port();

		fil = 0x0001 ;
		psstat = ksstat[ ycnt ];
		pscnt  = kscnt[ ycnt ];
		psrpt  = ksrpt[ ycnt ];
		psrpt2 = ksrpt2[ycnt];
		for( x=0 ; x< KEY_XSZ ; x++ ){
			if( KEY_OFF == *psstat ){ /* ksstat = OFF */
				if(  !(k & fil)  )	*pscnt = 0 ;
				else{
					if( ++(*pscnt)>CHAT_MAX){
						*psstat = KEY_ON ;
						*pscnt = 0 ;
						*psrpt = 0 ;
						AfifoPut(&fifokey,kdata_on[ycnt][x]);
						//g_sys.PowOffTimer = POW_OFF_TIME; // Timer
					}
				}
			}else{	/* keystat = ON */
				if(  k & fil){
					if( ++(*psrpt) > KRPT_CNT ){
						if( *psrpt2 > KRPT2_CNT ){
							AfifoPut(&fifokey,kdata_rpt2[ycnt][x]);
						}else{
							AfifoPut(&fifokey,kdata_rpt[ycnt][x]);
							(*psrpt2)++;
						
						}
						*psrpt= KRPT_INI;
					}
					*pscnt = 0 ;
				}else{
					if( ++(*pscnt )>CHAT_MAX){
						*psstat = KEY_OFF ;
						*pscnt = 0 ;
						*psrpt = 0;
						*psrpt2= 0;
						AfifoPut(&fifokey,kdata_off[ycnt][x]);
					}
				}
			}
			fil <<= 1 ;
			psstat++;
			pscnt ++;
			psrpt ++;
			psrpt2++;
		}
		if( ++ycnt >= KEY_YSZ){
			ycnt = 0;
		}
	}
}



/*----------------------------------------------------------------------*
 処理概要 ： キー入力があるまで待ち続ける
 引    数 ： void
 返    値 ： キー入力結果
 *----------------------------------------------------------------------*/
uint8_t KEY_get( void )
{
	uint8_t uc;
	while( AfifoCount(&fifokey)==0);
	_DI;
	AfifoGet(&fifokey,&uc);
	_EI;
	return uc;
}


/*----------------------------------------------------------------------*
 処理概要 ： キー入力が無ければ即返るキー入力
 引    数 ： void
 返    値 ： キー入力結果,0x00の場合はキー入力無し
 *----------------------------------------------------------------------*/
uint8_t KEY_pget( void )
{
	uint8_t uc;
	if( AfifoCount(&fifokey)){
		_DI;
		AfifoGet(&fifokey,&uc);
		_EI;
	}else{
		uc=0;
	}
	return uc;
}

/*----------------------------------------------------------------------*
 処理概要 ： キー入力数をカウントする。
 引    数 ： void
 返    値 ： void
 *----------------------------------------------------------------------*/
uint16_t KEY_cnt( void )
{
	return AfifoCount(&fifokey);
}


/*----------------------------------------------------------------------*
 処理概要 ： キー入力がバッファを空にする。
 引    数 ： void
 返    値 ： void
 *----------------------------------------------------------------------*/
void KEY_clr( void )
{
	_DI;
	AfifoInit(&fifokey,fifokeybuf,sizeof(fifokeybuf));
	_EI;
}

/*----------------------------------------------------------------------*
 処理概要 ： キー入力の為の初期化、割り込みの開始
 引    数 ： void
 返    値 ： void
 *----------------------------------------------------------------------*/
void KEY_init( void)
{
	int x,y;
	ycnt  =0;
	scancnt = SCAN_INTERVAL;
	for( y = 0; y < KEY_YSZ; y++ ){
		for( x=0 ; x< KEY_XSZ ; x++ ){
			kscnt[ y ][ x ] = 0 ;
			ksrpt[ y ][ x ] = 0 ;
			ksstat[ y ][ x ] = KEY_OFF; 
		}
	}
	AfifoInit(&fifokey,fifokeybuf,sizeof(fifokeybuf));
}

/*----------------------------------------------------------------------*
 処理概要 ： キーポートのモニター
 引    数 ： void
 返    値 ： void
 *----------------------------------------------------------------------*/
uint16_t KEY_Port( void )
{
	int x,y;
	uint16_t ret = 0;
	uint16_t sbit = 1;
	for( y = 0; y < KEY_YSZ; y++ ){
		for( x=0 ; x< KEY_XSZ ; x++ ){
			if( ksstat[ y ][ x ] != KEY_OFF){
				ret |= sbit;
				sbit <<= 1;
			}
		}
	}
	return ret;
}

uint8_t KEY_isPush(void)
{
	uint8_t k;
	k = scan_port();	
	return k;
}



