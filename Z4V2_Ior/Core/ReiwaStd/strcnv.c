// (C) 2016 FUJI Technology Inc.
/************************************************************************
 *																		*
 *	Compiler		: Hitachi R8C										*
 *																		*
 *	File name		: strcnv.c											*
 *																		*
 *	Description		: R8C CPU Standerd Library						*
 *																		*
 *	Revision history:													*
 *																		*
 *	Version 1.00	: 1997/11/05	: 1st Coded							*
 *																		*
 ************************************************************************/
#include	<string.h>
#include	<stdint.h>


/*==============================================================*
 *			Const, Macro, type definition						*
 *==============================================================*/

const	uint8_t	ascData[] = {	'0', '1', '2', '3', '4', '5', '6', '7',
								'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
 } ;

const	uint8_t	binData[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
 } ;

typedef	union {		/* ��A�h���X�R�[���������	*/
	uint16_t	d ;
	uint8_t	t[2] ;
} A3STD_FM ;


/*==============================================================*
 *			Function prototypes									*
 *==============================================================*/
uint8_t	*AstdVer( void ) ;
uint8_t	AcalBCC( uint8_t *pstr , uint16_t len ) ;
uint16_t	AcalSUM( char *pstr , uint16_t len ) ;
uint16_t	AcalCRC( uint8_t *pstr , uint16_t len ) ;
uint16_t	Aatoi( char *pstr , uint16_t len ) ;
void	Aitoa( char *pstr, uint16_t data, uint16_t keta , uint8_t zs ) ;
uint16_t	Astoi( uint8_t *pstr, uint16_t len ) ;
void	Aitos( uint8_t *pstr, uint16_t data, uint16_t keta , uint8_t zs ) ;
uint32_t	Aatol( char *pstr , uint16_t len ) ;
void	Altoa( char *pstr, uint32_t data, uint16_t keta , uint8_t zs ) ;
uint32_t	Astol( uint8_t *pstr, uint16_t len ) ;
void	Aftoa( char *pstr, float f );
uint8_t	AhextoAsc( uint8_t hexdata ) ;
uint8_t	AasctoHex( uint8_t ascdata ) ;
uint16_t	StrToUh( uint8_t *p ) ;
void	UhToStr( uint8_t *p , uint16_t d ) ;

static	uint16_t	crc_cal( uint8_t data, uint16_t crc_var ) ;


uint16_t Bcd2Bin(uint16_t bcd );
uint16_t Bin2Bcd(uint16_t bin );


/*==============================================================*
 *			Valiable Definition									*
 *==============================================================*/




/*----------------------------------------------------------------------*
 *		�����T�v �F �a�b�b���쐬����									*
 *		��	  �� �F *pstr	: ��v�Z�f�[�^�̊J�n�|�C���^				*
 *		��	  �� �F len		: �v�Z�o�C�g��								*
 *		��	  �l �F �v�Z��̂a�b�b�̒l									*
 *----------------------------------------------------------------------*/
uint8_t	AcalBCC( uint8_t *pstr, uint16_t len )
{
uint8_t	bcc = 0 ;

	while( len-- ) bcc ^= *pstr++ ;
	return	bcc ;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �`�F�b�N�T�����쐬����								*
 *		��	  �� �F *pstr	: ��v�Z�f�[�^�̊J�n�|�C���^				*
 *		��	  �� �F len		: �v�Z�o�C�g��								*
 *		��	  �l �F �v�Z��̂r�t�l�̒l									*
 *----------------------------------------------------------------------*/
uint16_t	AcalSUM( char *pstr, uint16_t len )
{
uint16_t	sum = 0 ;

	while( len-- ) sum += ( uint16_t )( *pstr++ ) ;
	return	sum ;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �b�q�b���쐬����									*
 *		��	  �� �F *pstr : �쐬����擪�|�C���^						*
 *		��	  �� �F len   : �v�Z���钷��								*
 *		��	  �l �F �v�Z��̂b�q�b�̒l									*
 *----------------------------------------------------------------------*/
uint16_t	AcalCRC( uint8_t *pstr, uint16_t len )
{
uint16_t	crc=0 ;

	while( len-- ) crc = crc_cal( *pstr++ , crc ) ;
	return crc ;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �b�q�b���쐬����									*
 *		��	  �� �F data	: ��v�Z�f�[�^								*
 *		��	  �� �F crc_ver : �v�Z�O�̂b�q�b�̒l						*
 *		��	  �l �F �v�Z��̂b�q�b�̒l									*
 *----------------------------------------------------------------------*/
static	uint16_t	crc_cal( uint8_t data, uint16_t crc_var )
{
uint8_t	chk ;
uint16_t	tmp=0 ;

	chk = ( uint8_t )( crc_var>>8 ) ^ data ;

		/* ��ʃr�b�g��s */
	if( chk & 0x01 ) tmp ^= 0x8303 ;
	if( chk & 0x02 ) tmp ^= 0x8183 ;
	if( chk & 0x04 ) tmp ^= 0x80c3 ;
	if( chk & 0x08 ) tmp ^= 0x8063 ;
	if( chk & 0x10 ) tmp ^= 0x8033 ;
	if( chk & 0x20 ) tmp ^= 0x801b ;
	if( chk & 0x40 ) tmp ^= 0x800f ;
	if( chk & 0x80 ) tmp ^= 0x8005 ;

	chk = ( uint8_t )( tmp>>8 ) ^ ( uint8_t )( crc_var & 0x00ff ) ;
	tmp = ( tmp & 0x00ff ) | ( ( uint16_t )chk << 8 ) ;
	return tmp ;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �P�U�i��������Q�o�C�g���l�ɕϊ�					*
 *		��	  �� �F *pstr	: �ϊ�������								*
 *		��	  �� �F len		: �ϊ�������̒���							*
 *		��	  �l �F �v�Z��̒l											*
 *----------------------------------------------------------------------*/
uint16_t	Aatoi( char *pstr , uint16_t len )
{
uint16_t	i,tmp=0 ;
uint8_t	d ;

	if( len > 4 ) len = 4 ;

	for( i=0 ; i<len ; i++ ){
		d = binData[ *pstr++ ] ;
		if( d != 0xff ){
			tmp = ( ( tmp<<4 )|( uint16_t )d ) ;
		}else{
			tmp = 0 ;
			break ;
		}
	}
	return tmp ;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �Q�o�C�g���l���P�U�i������ɕϊ�					*
 *		��	  �� �F *pstr	: �ϊ���̃Z�b�g�|�C���^					*
 *		��	  �� �F data	: �ϊ����l									*
 *		��	  �� �F keta	: �ϊ�������̌���							*
 *		��	  �� �F zs		: �T�v���X����								*
 *		��	  �l �F �Ȃ�												*
 *----------------------------------------------------------------------*/
void	Aitoa( char *pstr, uint16_t data, uint16_t keta , uint8_t zs )
{
uint16_t	i ;
char 	*p,h,tmp[4];

	if( keta ){
		if( zs )	memset( tmp , zs , 4 ) ;
		else		memset( tmp , ' ' , 4 ) ;

		p = &tmp[3] ;
		for( i=0 ; i<4 ; i++ ){
			h =( uint8_t )( data & 0x0f ) ;		/* data % 16	*/
			if( data )	*p-- = ascData[ h ] ;
			else		break ;
			data >>= 4 ;						/* data /= 16	*/
		}
		if( keta > 4 ){
			while( keta-- != 4 ){
				if( zs ) *pstr++ = zs ;
				else	 *pstr++ = ' ' ;
			}
			memcpy( pstr , tmp , 4 ) ;
		}else{
			memcpy( pstr , &tmp[ 4-keta ] , keta ) ;
		}
	}
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �P�O�i��������Q�o�C�g���l�ɕϊ�					*
 *		��	  �� �F *pstr	: �ϊ�������								*
 *		��	  �� �F len		: �ϊ�������̒���							*
 *		��	  �l �F �v�Z��̒l											*
 *----------------------------------------------------------------------*/
uint16_t	Astoi( uint8_t *pstr, uint16_t len )
{
uint16_t	i,tmp = 0 ;
uint8_t	d ;

	if( len > 5 )	len = 5 ;

	for( i=0 ; i<len ; i++ ){
		d = binData[ *pstr++ ] ;
		if( d < 10 ){
			tmp = ( ( tmp * 10 ) + ( uint16_t )d ) ;
		}else{
			tmp = 0 ;
			break ;
		}
	}
	return tmp ;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �Q�o�C�g���l���P�O�i������ɕϊ�					*
 *		��	  �� �F *pstr	: �ϊ���̃Z�b�g�|�C���^					*
 *		��	  �� �F data	: �ϊ����l									*
 *		��	  �� �F keta	: �ϊ�������̌���							*
 *		��	  �� �F zs		: �T�v���X����								*
 *		��	  �l �F �Ȃ�												*
 *----------------------------------------------------------------------*/
void	Aitos( uint8_t *pstr, uint16_t data, uint16_t keta , uint8_t zs )
{
uint16_t	i ;
uint8_t	*p,h,tmp[5] ;

	if( keta ){
		if( zs )	memset( tmp , zs , 5 ) ;
		else		memset( tmp , ' ' , 5 ) ;

		p = &tmp[4] ;
		for( i=0 ; i<5 ; i++ ){
			h =( uint8_t )( data % 10 ) ;
			if( data || i == 0)	*p-- = ascData[ h ] ;
			else		break ;
			data /= 10 ;
		}

		if( keta > 5 ){
			while( keta-- != 5 ){
				if( zs ) *pstr++ = zs ;
				else	 *pstr++ = ' ' ;
			}
			memcpy( pstr , tmp , 5 ) ;
		}else{
			memcpy( pstr , &tmp[ 5-keta ] , keta ) ;
		}
	}
}


/*----------------------------------------------------------------------*
 *		�����T�v �F �Q�o�C�g���l���P�O�i������ɕϊ�(�T�C����)		*
 *		��	  �� �F *pstr	: �ϊ���̃Z�b�g�|�C���^					*
 *		��	  �� �F data	: �ϊ����l									*
 *		��	  �� �F keta	: �ϊ�������̌���							*
 *		��	  �� �F zs		: �T�v���X����								*
 *		��	  �l �F �Ȃ�												*
 *----------------------------------------------------------------------*/
void	AitoDec( char *pstr, short data, uint16_t keta , uint8_t zs )
{
uint16_t	i ;
uint8_t	*p,h,tmp[5] ;
char sign;

	if( keta ){
		if(data <0 ) {
			sign = '-';
			data = -data;
		}else{
			sign = ' ';
		}
		
		if( zs )	memset( tmp , zs , 5 ) ;
		else		memset( tmp , ' ' , 5 ) ;

		p = &tmp[4] ;
		for( i=0 ; i<5 ; i++ ){
			h =( uint8_t )( data % 10 ) ;
			if( data || i == 0)	*p-- = ascData[ h ] ;
			else		break ;
			data /= 10 ;
		}

		if( keta > 5 ){
			while( keta-- != 5 ){
				if( zs ) *pstr++ = zs ;
				else	 *pstr++ = ' ' ;
			}
			memcpy( pstr , tmp , 5 ) ;
		}else{
			memcpy( pstr , &tmp[ 5-keta ] , keta ) ;
		}
		*pstr = sign;
	}
}


/*----------------------------------------------------------------------*
 *		�����T�v �F �P�U�i��������S�o�C�g���l�ɕϊ�					*
 *		��	  �� �F *pstr	: �ϊ�������								*
 *		��	  �� �F len		: �ϊ�������̒���							*
 *		��	  �l �F �v�Z��̒l											*
 *----------------------------------------------------------------------*/
uint32_t	Aatol( char *pstr , uint16_t len )
{
uint32_t	tmp=0 ;
uint16_t	i;
uint8_t	d ;

	if( len > 8 ) len = 8 ;

	for( i = 0 ; i < len ; i++ ){
		d = binData[ *pstr++ ] ;
		if( d != 0xff ){
			tmp = ( ( tmp<<4 )|d ) ;
		}else{
			tmp = 0 ;
			break ;
		}
	}
	return tmp ;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �P�U�i������𕂓������_���ɕϊ�					*
 *		��	  �� �F *pstr	: �ϊ�������								*
 *		��	  �l �F �v�Z��̒l(float �l)											*
 *----------------------------------------------------------------------*/
float	Aatof( char *pstr  ) 
{
	int len = 8;
	union{
		float f;
		uint32_t ui32;
	}u_fc;
	uint16_t	i;
	uint8_t	d ;

	u_fc.ui32= 0;
	for( i = 0 ; i < len ; i++ ){
		d = binData[ *pstr++ ] ;
		if( d != 0xff ){
			u_fc.ui32 = ( ( u_fc.ui32<<4 )|d ) ;
		}else{
			u_fc.ui32 = 0 ;
			break ;
		}
	}
	return u_fc.f ;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �S�o�C�g���l���P�U�i������ɕϊ�					*
 *		��	  �� �F *pstr	: �ϊ���̃Z�b�g�|�C���^					*
 *		��	  �� �F data	: �ϊ����l									*
 *		��	  �� �F keta	: �ϊ�������̌���							*
 *		��	  �� �F zs		: �T�v���X����								*
 *		��	  �l �F �Ȃ�												*
 *----------------------------------------------------------------------*/
void	Altoa( char *pstr, uint32_t data, uint16_t keta , uint8_t zs )
{
uint16_t	i ;
uint8_t	*p,h,tmp[8] ;

	if( keta ){
		if( zs )	memset( tmp , zs , 8 ) ;
		else		memset( tmp , ' ' , 8 ) ;

		p = &tmp[7] ;
		for( i=0 ; i<8 ; i++ ){
			h =( uint8_t )( data & 0x0f ) ;		/* data % 16	*/
			if( data )	*p-- = ascData[ h ] ;
			else		break ;
			data >>= 4 ;						/* data /= 16	*/
		}
		if( keta > 8 ){
			while( keta-- != 8 ){
				if( zs ) *pstr++ = zs ;
				else	 *pstr++ = ' ' ;
			}
			memcpy( pstr , tmp , 8 ) ;
		}else{
			memcpy( pstr , &tmp[ 8-keta ] , keta ) ;
		}
	}
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �P�O�i��������S�o�C�g���l�ɕϊ�					*
 *		��	  �� �F *pstr	: �ϊ�������								*
 *		��	  �� �F len		: �ϊ�������̒���							*
 *		��	  �l �F �v�Z��̒l											*
 *----------------------------------------------------------------------*/
uint32_t	Astol( uint8_t *pstr, uint16_t len )
{
uint32_t	tmp=0 ;
uint16_t	i ;
uint8_t	d ;

	if( len > 10 )	len = 10 ;

	for( i=0 ; i<len ; i++ ){
		d = binData[ *pstr++ ] ;
		if( d < 10 ){
			tmp = ( ( tmp * 10 ) + ( uint16_t )d ) ;
		}else{
			tmp = 0 ;
			break ;
		}
	}
	return tmp ;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �S�o�C�g���l���P�O�i������ɕϊ�					*
 *		��	  �� �F *pstr	: �ϊ���̃Z�b�g�|�C���^					*
 *		��	  �� �F data	: �ϊ����l									*
 *		��	  �� �F keta	: �ϊ�������̌���							*
 *		��	  �� �F zs		: �T�v���X����								*
 *		��	  �l �F �Ȃ�												*
 *----------------------------------------------------------------------*/
void	Altos( uint8_t *pstr, uint32_t data, uint16_t keta , uint8_t zs )
{
uint16_t	i ;
uint8_t	*p,h,tmp[10] ;

	if( keta ){
		if( zs )	memset( tmp , zs , 10 ) ;
		else		memset( tmp , ' ' , 10 ) ;
		

		p = &tmp[9] ;
		*p = '0';
		for( i=0 ; i<10 ; i++ ){
			h =( uint8_t )( data % 10 ) ;
			if( data )	*p-- = ascData[ h ] ;
			else		break ;
			data /= 10 ;
		}

		if( keta > 10 ){
			while( keta-- != 10 ){
				if( zs ) *pstr++ = zs ;
				else	 *pstr++ = ' ' ;
			}
			memcpy( pstr , tmp , 10 ) ;
		}else{
			memcpy( pstr , &tmp[ 10-keta ] , keta ) ;
		}
	}
}

/*----------------------------------------------------------------------*
 *		�����T�v �F float��16�i��������ɕϊ�							*
 *		��	  �� �F *pstr	: �ϊ���̃Z�b�g�|�C���^					*
 *		��	  �� �F data	: �ϊ����l									*
 *		��	  �� �F zs		: �T�v���X����								*
 *		��	  �l �F �Ȃ�												*
 *----------------------------------------------------------------------*/
//char ff[8]; for debug & kakunin
void	Aftoa( char *pstr, float f )
{
	union{
		float f;
		uint32_t ui32;
	}u_fc;
	
	u_fc.f = f;
	
	Altoa((char*)pstr,u_fc.ui32,8,'0');
//	memcpy(ff,pstr,0);	for debug
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �P�U�i�����P�O�i�A�X�L�[�ϊ�						*
 *		��	  �� �F hexdata : �P�U�i��									*
 *		��	  �l �F �P�O�i�A�X�L�[�l									*
 *----------------------------------------------------------------------*/
uint8_t	AhextoAsc( uint8_t hexdata )
{
	if( hexdata < 16 )	return	ascData[ hexdata ] ;
	return 0 ;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �P�O�i�A�X�L�[���P�U�i���ϊ�						*
 *		��	  �� �F bcddata : �P�O�i�A�X�L�[�l							*
 *		��	  �l �F �P�U�i��											*
 *----------------------------------------------------------------------*/
uint8_t	AasctoHex( uint8_t ascdata )
{
	return binData[ ascdata ] ;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �Q�o�C�g�̃f�[�^���t�r�g�n�q�s�ɕϊ�				*
 *		��	  �� �F p : �ϊ��f�[�^�|�C���^								*
 *		��	  �l �F void												*
 *----------------------------------------------------------------------*/
uint16_t	StrToUh( uint8_t *p )
{
A3STD_FM	fmCh ;

	fmCh.t[0] = *p++ ;
	fmCh.t[1] = *p ;
	return fmCh.d ;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F �r�g�n�q�s�f�[�^���Q�o�C�g�b�������ɑ��			*
 *		��	  �� �F p : ����|�C���^									*
 *		��	  �� �F d : �ϊ��f�[�^										*
 *		��	  �l �F void												*
 *----------------------------------------------------------------------*/
void	UhToStr( uint8_t *p , uint16_t d )
{
A3STD_FM	fmCh ;

	fmCh.d = d ;
	*p++ = fmCh.t[0] ;
	*p	 = fmCh.t[1] ;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F BCD���o�C�i���[�ɕύX���܂��B						*
 *		��	  �� �F UWORD bcd  : BCD�f�[�^								*
 *		��	  �l �F �o�C�i���[�f�[�^									*
 *----------------------------------------------------------------------*/
uint16_t Bcd2Bin(uint16_t bcd )
{
	int i;
	uint16_t uw;
	uint16_t ans = 0;
	uint16_t baisu = 1;

	for(i=0;i<4;i++){
		uw = bcd & 0x0F;
		
		ans += ( uw*baisu );
		
		bcd >>= 4;
		baisu *= 10;
	}
	return ans;
}

/*----------------------------------------------------------------------*
 *		�����T�v �F BCD���o�C�i���[�ɕύX���܂��B						*
 *		��	  �� �F UWORD bcd  : BCD�f�[�^								*
 *		��	  �l �F B�o�C�i���[�f�[�^									*
 *----------------------------------------------------------------------*/
uint16_t Bin2Bcd(uint16_t bin )
{
	int i;
	uint16_t uw;
	uint16_t ans = 0;
	uint16_t baisu = 1;

	for(i=0;i<4;i++){
		uw = bin % 10;
		
		ans += ( uw*baisu );
		
		bin /=10;
		baisu *= 16;
	}
	return ans;
}



/*----------------------------------------------------------------------*
�����T�v �F  ������2�o�C�g��[L][U]��[UL]
��	  �� �F  uint8_t *ub							
��	  �l �F _UWORD							
		 �F  le = little endian	
 *----------------------------------------------------------------------*/
uint16_t get_uword_le(uint8_t *ub) {
    uint16_t u, l;
    l = *ub;
    ub++;
    u = *ub;
    u <<= 8;

    u |= l;
    return u;
}


// be = big endian
// ������2�o�C�g��
//[U][L]��[UL];
uint16_t get_uword_be(uint8_t *ub) {
    uint16_t u, l;
    u = *ub;
    ub++;
    l = *ub;
    u <<= 8;

    u |= l;
    return u;
}


// be = big endian
// ������2�o�C�g��
//[U][L]��[UL];
int32_t get_long_be(uint8_t *ub) {
    int32_t l;
    l = *ub;
    ub++;
    l <<= 8;
    l |= *ub;

    ub++;
    l <<= 8;
    l |= *ub;

    ub++;
    l <<= 8;
    l |= *ub;
    return l;
}





// little endian
//[UL]��[L][U];
// _UWORD�o�C�g���L�����N�^�^�z��ɂ����B
void put_uword_le(uint8_t *dst, uint16_t src) 
{
    *dst = (uint8_t) src;
    dst++;
    *dst = src >> 8;
}

// big endian
//[UL]��[U][L];
// _UWORD�o�C�g���L�����N�^�^�z��ɂ����B
void put_uword_be(uint8_t *dst, uint16_t src) 
{
    *dst = src >> 8;
    dst++;
    *dst = (uint8_t) src;
}




