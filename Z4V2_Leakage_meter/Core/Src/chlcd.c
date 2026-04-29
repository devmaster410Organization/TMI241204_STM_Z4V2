/// @file   chlcd.c
/// @brief   ST786LC02 LCD Library GPIO bit bang access
/// @author  y.sugawara
/// @date    2026/04/29
/// @version 1.1  uint8_t * → char * へ変更

#include <string.h>
#include "chlcd.h"
#include "strcnv.h"

/*==============================================================*
 *			Const, Macro, type definition						*
 *==============================================================*/

#define LCD_RS_HI PORT_HI(LCD_RS)
#define LCD_RS_LO PORT_LO(LCD_RS)
#define LCD_E_HI  PORT_HI(LCD_E)
#define LCD_E_LO  PORT_LO(LCD_E)
#define LCD_DB4_HI PORT_HI(LCD_DB4)
#define LCD_DB4_LO PORT_LO(LCD_DB4)	
#define LCD_DB5_HI PORT_HI(LCD_DB5)
#define LCD_DB5_LO PORT_LO(LCD_DB5)
#define LCD_DB6_HI PORT_HI(LCD_DB6)
#define LCD_DB6_LO PORT_LO(LCD_DB6)	
#define LCD_DB7_HI PORT_HI(LCD_DB7)
#define LCD_DB7_LO PORT_LO(LCD_DB7)



#define		LINES 		4
#define 	LCD_SIZE	( 20*LINES )
#define 	LINE_SIZE	( LCD_SIZE / LINES )
#define 	UP_LINE		0x00
#define		DWN_LINE	0x40


/*==============================================================*
 *			Function prototypes									*
 *==============================================================*/
char	*ChlcdVersion( void ) ;
void	ChlcdInit( void ) ;
void	ChlcdCls( void ) ;
void	ChlcdPutchar( uint8_t c ) ;
void	ChlcdPuts( uint8_t *str ) ;
void	ChlcdLocate( uint8_t x , uint8_t y ) ;
void	ChlcdSetcgram( uint8_t cgno , uint8_t *adr ) ;

void	ChlcdPrint( uint16_t x, uint16_t y, char *str ) ;
void 	ChlcdnPrint( uint16_t x, uint16_t y, char *str, uint16_t n ) ;
void	ChlcdPutHex( uint16_t x, uint16_t y, uint16_t h, uint16_t keta ) ;
void	ChlcdPutUdec( uint16_t x, uint16_t y, uint16_t ui, uint16_t keta ) ;
void	ChlcdPutSdec( uint16_t x, uint16_t y, int16_t i, uint16_t keta ) ;
void	ChlcdPutZdec( uint16_t x, uint16_t y, uint16_t ui, uint16_t keta ) ;

void	ChlcdCursorOff( void ) ;
void	ChlcdCursorOn( void ) ;

static	void	wait_lcdready( void ) ;
static	void	wait_loop( uint16_t t ) ;


void  lcd_db_out( uint8_t data );

/*==============================================================*
 *			Valiable Definition									*
 *==============================================================*/
uint8_t	cursorX , cursorY ;
uint8_t	*cg_adrs[ 8 ] ;			/* CG address				*/

static	const	uint8_t	line_add[] = { 0x00 , 0x40,0x14,0x54 };
static	char	*const	pChlcdVer = "v1.00 26.04.14" ;

void delay_us(uint32_t us)
{
    us *= (SystemCoreClock / 1000000) / 5;  // クロック周波数に応じて調整
    while(us--)
    {
        __NOP();
    }
}
// DB 出力 4ビットモード
void  lcd_db_out_low4( uint8_t data )
{
	if( data & 0x01 ) LCD_DB4_HI; else LCD_DB4_LO;
	if( data & 0x02 ) LCD_DB5_HI; else LCD_DB5_LO;
	if( data & 0x04 ) LCD_DB6_HI; else LCD_DB6_LO;
	if( data & 0x08 ) LCD_DB7_HI; else LCD_DB7_LO;
	LCD_E_HI;
	delay_us(1);
	LCD_E_LO;

}

void  lcd_db_out_high4( uint8_t data )
{
	if( data & 0x10 ) LCD_DB4_HI; else LCD_DB4_LO;
	if( data & 0x20 ) LCD_DB5_HI; else LCD_DB5_LO;
	if( data & 0x40 ) LCD_DB6_HI; else LCD_DB6_LO;
	if( data & 0x80 ) LCD_DB7_HI; else LCD_DB7_LO;
	LCD_E_HI;
	delay_us(1);
	LCD_E_LO;
}



/*----------------------------------------------------------------------*
 *		処理概要 ： ＬＣＤライブラリのバージョンをかえす				*
 *		引    数 ： void												*
 *		返    値 ： バージョンストリングポインタ						*
 *----------------------------------------------------------------------*/
char	*ChlcdVersion( void )
{
	return pChlcdVer ;
}

 
/*----------------------------------------------------------------------*
 *		処理概要 ： LCD Busy Check Wait									*
 *		引    数 ： void												*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
static	void	wait_lcdready( void )
{
	delay_us( 37);

}

/*----------------------------------------------------------------------*
 *		処理概要 ： Wait Loop											*
 *		引    数 ： t : Loop Count										*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
static	void	wait_loop( uint16_t t )
{
volatile	uint16_t	i;

	while( t-- > 0 ){
		for( i =0 ;i<10000 ; i++ ) ;
	}
}

/*----------------------------------------------------------------------*
 *		処理概要 ： LCD Initialize										*
 *		引    数 ： void												*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	lcd_hard_init( void )
{
	uint8_t dat;
	LCD_RS_LO;
	LCD_E_LO;
	// 電源ON後15ms以上待つ
	HAL_Delay(15);
	
	// 8ビットモードで初期化開始（上位4ビットのみ送信）
	lcd_db_out_high4( 0x30 );  // Function Set: 8bit mode
	HAL_Delay(5);  // 4.1ms以上待つ
	
	lcd_db_out_high4( 0x30 );  // Function Set: 8bit mode
	delay_us(150);  // 100us以上待つ
	
	lcd_db_out_high4( 0x30 );  // Function Set: 8bit mode
	delay_us(150);
	
	// 4ビットモードに切り替え
	lcd_db_out_high4( 0x20 );  // Function Set: 4bit mode
	delay_us(150);
	
	// 以降は4ビットモード（上位4ビット→下位4ビット）で送信
	
	// Function Set: 4bit, 2line, 5x8 dots (0x28)
	dat = 0x28;
	lcd_db_out_high4( dat );
	lcd_db_out_low4( dat );
	delay_us(50);
	
	// Display OFF (0x08)
	dat = 0x08;
	lcd_db_out_high4( dat );
	lcd_db_out_low4( dat );
	delay_us(50);
	
	// Display Clear (0x01)
	dat = 0x01;
	lcd_db_out_high4( dat );
	lcd_db_out_low4( dat );
	HAL_Delay(2);  // Clear命令は1.52ms必要
	
	// Entry Mode Set: Increment, No shift (0x06)
	dat = 0x06;

	lcd_db_out_high4( dat );
	lcd_db_out_low4( dat );
	delay_us(50);
	
	// Display ON, Cursor OFF, Blink OFF (0x0C)
	dat = 0x0c;
	lcd_db_out_high4( dat);
	lcd_db_out_low4( dat );
	delay_us(50);
}

/*----------------------------------------------------------------------*
 *		処理概要 ： カーソル Ｏｆｆ										*
 *		引    数 ： void												*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	ChlcdCursorOff( void )
{
	uint8_t cmd = 0x0c;
	wait_lcdready();
	LCD_RS_LO;
	lcd_db_out_high4( cmd );
	lcd_db_out_low4( cmd );
}

/*----------------------------------------------------------------------*
 *		処理概要 ： カーソル Ｏｎ										*
 *		引    数 ： void												*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	ChlcdCursorOn( void )
{
	uint8_t cmd = 0x0e;
	wait_lcdready() ;
	LCD_RS_LO;
	lcd_db_out_high4( line_add[ cursorY ] + cursorX ) ;
	lcd_db_out_low4( line_add[ cursorY ] + cursorX ) ;
	wait_lcdready();
	lcd_db_out_high4( cmd ) ;
	lcd_db_out_low4( cmd ) ;	
}

/*----------------------------------------------------------------------*
 *		処理概要 ： LCD Library Initialize								*
 *		引    数 ： void												*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	ChlcdInit( void )
{
uint16_t i ;
	for( i=0 ; i<8 ; i++ ) cg_adrs[i] = ( uint8_t* )0 ;
	cursorX = cursorY = 0 ;
	lcd_hard_init() ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： LCD Display All Clear								*
 *		引    数 ： void												*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	ChlcdCls( void )
{
	uint8_t cmd = 0x01;
	LCD_RS_LO;
	wait_lcdready() ;
	lcd_db_out_high4( cmd ) ;
	lcd_db_out_low4( cmd ) ;
	cursorX = cursorY = 0 ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： ＬＣＤに１文字表示する								*
 *		引    数 ： c : 表示文字										*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	ChlcdPutchar( uint8_t c )
{
	uint8_t cmd;
	LCD_RS_LO;
	wait_lcdready() ;
	cmd = line_add[ cursorY ] + cursorX  | 0x80;
	lcd_db_out_high4( cmd ) ;
	lcd_db_out_low4( cmd ) ;

	LCD_RS_HI;
	wait_lcdready() ;
	lcd_db_out_high4( c ) ;
	lcd_db_out_low4( c ) ;

	if( ++cursorX >= LINE_SIZE ){
		cursorX = 0 ;
		if( ++cursorY >= LINES ) cursorY = 0 ;
	}
}

/*----------------------------------------------------------------------*
 *		処理概要 ： ＬＣＤに文字列を表示する							*
 *		引    数 ： *str : 表示文字ポインタ								*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	ChlcdPuts( uint8_t *str )
{
	while( *str ) ChlcdPutchar( *str++ ) ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： 表示位置をロケートする								*
 *		引    数 ： x : Ｘ軸 位置										*
 *		引    数 ： y : Ｙ軸 位置										*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	ChlcdLocate( uint8_t x , uint8_t y )
{
	if( ( x < LINE_SIZE )&&( y < LINES ) ){
		cursorX = x ;
		cursorY = y ;
	}
}

/*----------------------------------------------------------------------*
 *		処理概要 ： ＣＧメモリにデータをセットする						*
 *		引    数 ： cgno : ＣＧ番号										*
 *		引    数 ： *adr : ＣＧアドレス									*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	ChlcdSetcgram( uint8_t cgno , uint8_t *adr )
{
uint16_t i ;
#ifdef SUG
	if( cg_adrs[ cgno ] == adr ) return ;	/* 同じ場合は中止	*/
#endif
	LCD_RS_LO;	
	cg_adrs[ cgno ] = adr ;
	cgno <<= 3 ;
	cgno |= 0x40 ;
	wait_lcdready() ;
	lcd_db_out_high4( cgno ) ;
	lcd_db_out_low4( cgno ) ;

	LCD_RS_HI;
	for( i=0 ; i<8 ; i++ ){
		wait_lcdready() ;
		lcd_db_out_high4( *adr ) ;
		lcd_db_out_low4( *adr ) ;
		adr++ ;		/* CG_data set ( auto inc mode ) */
	}
}

/*----------------------------------------------------------------------*
 *		処理概要 ： 指定の位置に文字列を表示							*
 *		引    数 ： x    : 列											*
 *		引    数 ： y    : 行											*
 *		引    数 ： *str : 表示文字列ポインタ							*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	ChlcdPrint( uint16_t x, uint16_t y , char *str )
{
	if( ( x < LINE_SIZE )&&( y < LINES ) ){
		cursorX = x ;
		cursorY = y ;
		while( *str ) ChlcdPutchar( *str++ ) ;
	}
}

/*----------------------------------------------------------------------*
 *		処理概要 ： 指定の位置にｎ文字の文字列を表示					*
 *		引    数 ： x    : 列											*
 *		引    数 ： y    : 行											*
 *		引    数 ： *str : 表示文字列ポインタ							*
 *		引    数 ： n    : 表示文字長									*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void 	ChlcdnPrint( uint16_t x , uint16_t y , char *str , uint16_t n )
{
	if( ( x < LINE_SIZE )&&( y < LINES ) ){
		cursorX = x ;
		cursorY = y ;
		while( n-- ) ChlcdPutchar( *str++ ) ;
	}
}
#ifdef	_NOT_USED
uint8_t	itoc( int i )
{
	uint8_t uc;
	if( i>=0 && i<10){
		uc = '0'+i;
	}else if( i >= 10 && i<16){
		uc = 'A'+i-10;
	}else{
		uc = '?';
	}
	return uc;
}

void	Aitoa( uint8_t *dst,uint16_t h,uint16_t keta,uint8_t zs)
{
	uint8_t uc;
	uint8_t *p;
	uint16_t first;
	int kk;
	p = dst + keta -1;
	
	first = TRUE;
	while(keta--){
		kk = h % 0x10;
		uc = itoc( kk );
		if( kk ==0){
			if( h ||( first == TRUE )){
				*p = '0';
			}else{
				*p = zs;
			}
		}else{
			*p = uc;
		}
		h/= 0x10;
		first = FALSE;
		p--;
	}
}

void	Aitos( uint8_t *dst,uint16_t h,uint16_t keta,uint8_t zs)
{
	uint8_t uc;
	uint8_t *p;
	uint16_t first;
	int kk;
	p = dst + keta -1;
	
	first = TRUE;
	while(keta--){
		kk = h % 10;
		uc = itoc( kk );
		if( kk ==0){
			if( h ||( first == TRUE )){
				*p = '0';
			}else{
				*p = zs;
			}
		}else{
			*p = uc;
		}
		h/= 10;
		first = FALSE;
		p--;
	}
}
#endif

/*----------------------------------------------------------------------*
 *		処理概要 ： １６進数の表示										*
 *		引    数 ： x    : 列											*
 *		引    数 ： y    : 行											*
 *		引    数 ： h    : 表示データ									*
 *		引    数 ： keta : 表示文字長									*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	ChlcdPutHex( uint16_t x , uint16_t y , uint16_t h, uint16_t keta )
{
char	ustr[ 10 ] ;

	Aitoa( ustr , h , keta , '0' );
	ChlcdnPrint( x , y , ustr , keta ) ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： １０進数の表示										*
 *		引    数 ： x    : 列											*
 *		引    数 ： y    : 行											*
 *		引    数 ： ui   : 表示データ									*
 *		引    数 ： keta : 表示文字長									*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	ChlcdPutUdec( uint16_t x , uint16_t y , uint16_t ui , uint16_t keta )
{
uint8_t ustr[ 10 ] ;

	Aitos( ustr , ui , keta , '0' ) ;
	ChlcdnPrint( x, y, ustr, keta ) ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： １０進数の表示（符号付き）							*
 *		引    数 ： x    : 列											*
 *		引    数 ： y    : 行											*
 *		引    数 ： i    : 表示データ									*
 *		引    数 ： keta : 表示文字長									*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	ChlcdPutSdec( uint16_t x , uint16_t y , int16_t i , uint16_t keta )
{
uint8_t	ustr[ 10 ] ;
int16_t	absi ;

	memset( ustr , 0 , 10 );
	if( i < 0 ) absi = -i ;
	Aitos( ustr , absi , keta-1 , '0' ) ;
	ChlcdLocate( x+1,y ) ;
	ChlcdPuts( ustr ) ;

	ChlcdLocate( x,y ) ;
	if( i == 0 )	ChlcdPutchar( ' ' ) ;
	else if( i>0 )	ChlcdPutchar( '+' ) ;
	else 			ChlcdPutchar( '-' ) ;
}

/*----------------------------------------------------------------------*
 *		処理概要 ： １０進数の表示（０サプレス付き）					*
 *		引    数 ： x    : 列											*
 *		引    数 ： y    : 行											*
 *		引    数 ： i    : 表示データ									*
 *		引    数 ： keta : 表示文字長									*
 *		返    値 ： void												*
 *----------------------------------------------------------------------*/
void	ChlcdPutZdec( uint16_t x , uint16_t y , uint16_t ui , uint16_t keta )
{
uint8_t ustr[ 10 ] ;

	Aitos( ustr , ui , keta , '0' ) ;
	ChlcdnPrint( x, y, ustr, keta ) ;
}

