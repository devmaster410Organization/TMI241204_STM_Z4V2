// (C) 2016 FUJI Technology Inc.
/************************************************************************
 *		C Compiler		: Hitachi R8C
 *		File name		: strcnv.c
 *		Description		: R8C CPU Standard Library
 *		Revision history:
 *		Version 1.00	: 1997/11/05	: 1st Coded
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

/**< Union for address code conversion */
typedef	union {
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
 *			Variable Definition									*
 *==============================================================*/

/*----------------------------------------------------------------------*
 * @brief Calculate Block Check Character (BCC)
 * @param[in] pstr Pointer to starting address of data to calculate
 * @param[in] len   Number of bytes to calculate
 * @return Calculated BCC value
 *----------------------------------------------------------------------*/
uint8_t	AcalBCC( uint8_t *pstr, uint16_t len )
{
uint8_t	bcc = 0 ;

	while( len-- ) bcc ^= *pstr++ ;
	return	bcc ;
}

/**
 * @brief Calculate checksum of data
 * @param[in] pstr Pointer to starting address of data to calculate
 * @param[in] len   Number of bytes to calculate
 * @return Calculated checksum value
 */
uint16_t	AcalSUM( char *pstr, uint16_t len )
{
uint16_t	sum = 0 ;

	while( len-- ) sum += ( uint16_t )( *pstr++ ) ;
	return	sum ;
}

/**
 * @brief Calculate CRC16 of data
 * @param[in] pstr Pointer to starting address of data to calculate
 * @param[in] len   Number of bytes to calculate
 * @return Calculated CRC16 value
 */
uint16_t	AcalCRC( uint8_t *pstr, uint16_t len )
{
uint16_t	crc=0 ;

	while( len-- ) crc = crc_cal( *pstr++ , crc ) ;
	return crc ;
}

/**
 * @brief Calculate CRC16 value bit by bit
 * @param[in] data    Input data byte
 * @param[in] crc_var Previous CRC16 value
 * @return Calculated CRC16 value
 */
static	uint16_t	crc_cal( uint8_t data, uint16_t crc_var )
{
uint8_t	chk ;
uint16_t	tmp=0 ;

	chk = ( uint8_t )( crc_var>>8 ) ^ data ;

	/* Each bit processing */
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

/**
 * @brief Convert hexadecimal ASCII string to 16-bit unsigned value
 * @param[in] pstr Pointer to ASCII string to convert
 * @param[in] len   Length of ASCII string
 * @return Converted value
 */
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

/**
 * @brief Convert 16-bit unsigned value to hexadecimal ASCII string
 * @param[out] pstr Pointer to output buffer
 * @param[in] data  16-bit value to convert
 * @param[in] keta  Number of digits in output
 * @param[in] zs    Zero suppress character
 * @return None
 */
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

/**
 * @brief Convert decimal ASCII string to 16-bit unsigned value
 * @param[in] pstr Pointer to ASCII string to convert
 * @param[in] len   Length of ASCII string
 * @return Converted value
 */
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

/**
 * @brief Convert 16-bit unsigned value to decimal ASCII string
 * @param[out] pstr Pointer to output buffer
 * @param[in] data  16-bit value to convert
 * @param[in] keta  Number of digits in output
 * @param[in] zs    Zero suppress character
 * @return None
 */
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

/**
 * @brief Convert signed short value to decimal ASCII string
 * @param[out] pstr Pointer to output buffer
 * @param[in] data  Signed short value to convert
 * @param[in] keta  Number of digits in output
 * @param[in] zs    Zero suppress character
 * @return None
 */
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

/**
 * @brief Convert hexadecimal ASCII string to 32-bit unsigned value
 * @param[in] pstr Pointer to ASCII string to convert
 * @param[in] len   Length of ASCII string
 * @return Converted value
 */
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

/**
 * @brief Convert hexadecimal ASCII string to float value
 * @param[in] pstr Pointer to ASCII string to convert
 * @return Calculated float value
 */
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

/**
 * @brief Convert 32-bit unsigned value to hexadecimal ASCII string
 * @param[out] pstr Pointer to output buffer
 * @param[in] data  32-bit value to convert
 * @param[in] keta  Number of digits in output
 * @param[in] zs    Zero suppress character
 * @return None
 */
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

/**
 * @brief Convert decimal ASCII string to 32-bit unsigned value
 * @param[in] pstr Pointer to ASCII string to convert
 * @param[in] len   Length of ASCII string
 * @return Converted value
 */
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

/**
 * @brief Convert 32-bit unsigned value to decimal ASCII string
 * @param[out] pstr Pointer to output buffer
 * @param[in] data  32-bit value to convert
 * @param[in] keta  Number of digits in output
 * @param[in] zs    Zero suppress character
 * @return None
 */
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

/**
 * @brief Convert float value to hexadecimal ASCII string
 * @param[out] pstr Pointer to output buffer
 * @param[in] f     Float value to convert
 * @return None
 */
void	Aftoa( char *pstr, float f )
{
	union{
		float f;
		uint32_t ui32;
	}u_fc;
	
	u_fc.f = f;
	
	Altoa((char*)pstr,u_fc.ui32,8,'0');
}

/**
 * @brief Convert hexadecimal value to ASCII character
 * @param[in] hexdata 4-bit hexadecimal value
 * @return ASCII character representation
 */
uint8_t	AhextoAsc( uint8_t hexdata )
{
	if( hexdata < 16 )	return	ascData[ hexdata ] ;
	return 0 ;
}

/**
 * @brief Convert ASCII character to hexadecimal value
 * @param[in] ascdata ASCII character
 * @return Hexadecimal value
 */
uint8_t	AasctoHex( uint8_t ascdata )
{
	return binData[ ascdata ] ;
}

/**
 * @brief Convert 2-byte data to uint16_t (little endian)
 * @param[in] p Pointer to data buffer
 * @return uint16_t value
 */
uint16_t	StrToUh( uint8_t *p )
{
A3STD_FM	fmCh ;

	fmCh.t[0] = *p++ ;
	fmCh.t[1] = *p ;
	return fmCh.d ;
}

/**
 * @brief Convert uint16_t to 2-byte data (little endian)
 * @param[out] p Pointer to output buffer
 * @param[in] d  uint16_t value to convert
 * @return None
 */
void	UhToStr( uint8_t *p , uint16_t d )
{
A3STD_FM	fmCh ;

	fmCh.d = d ;
	*p++ = fmCh.t[0] ;
	*p	 = fmCh.t[1] ;
}

/**
 * @brief Convert BCD value to binary representation
 * @param[in] bcd BCD format data
 * @return Binary value
 */
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

/**
 * @brief Convert binary value to BCD representation
 * @param[in] bin Binary format data
 * @return BCD value
 */
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

/**
 * @brief Get 16-bit unsigned value from 2-byte buffer (little endian)
 * @param[in] ub Pointer to byte buffer
 * @return uint16_t value
 */
uint16_t get_uword_le(uint8_t *ub) {
    uint16_t u, l;
    l = *ub;
    ub++;
    u = *ub;
    u <<= 8;

    u |= l;
    return u;
}

/**
 * @brief Get 16-bit unsigned value from 2-byte buffer (big endian)
 * @param[in] ub Pointer to byte buffer
 * @return uint16_t value
 */
uint16_t get_uword_be(uint8_t *ub) {
    uint16_t u, l;
    u = *ub;
    ub++;
    l = *ub;
    u <<= 8;

    u |= l;
    return u;
}

/**
 * @brief Get 32-bit signed value from 4-byte buffer (big endian)
 * @param[in] ub Pointer to byte buffer
 * @return int32_t value
 */
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

/**
 * @brief Set 16-bit unsigned value to 2-byte buffer (little endian)
 * @param[out] dst Pointer to destination buffer
 * @param[in] src   uint16_t value to set
 * @return None
 */
void put_uword_le(uint8_t *dst, uint16_t src) 
{
    *dst = (uint8_t) src;
    dst++;
    *dst = src >> 8;
}

/**
 * @brief Set 16-bit unsigned value to 2-byte buffer (big endian)
 * @param[out] dst Pointer to destination buffer
 * @param[in] src   uint16_t value to set
 * @return None
 */
void put_uword_be(uint8_t *dst, uint16_t src) 
{
    *dst = src >> 8;
    dst++;
    *dst = (uint8_t) src;
}
