// (C) 2016 FUJI Technology Inc.
/***********************************************************************/
/*																	   */
/*	FILE		:strcnv.H											   */
/*	DATE		:Wed, Oct 10, 2007									   */
/*	DESCRIPTION :Project Header 									   */
/*	CPU GROUP	:Rx621 												   */
/*																	   */
/*																	   */
/***********************************************************************/


extern uint8_t	AcalBCC( uint8_t *pstr , uint16_t len ) ;
extern uint16_t	AcalSUM( char *pstr , uint16_t len ) ;
extern uint16_t	AcalCRC( uint8_t *pstr , uint16_t len ) ;
extern uint16_t	Aatoi( char *pstr , uint16_t len ) ;
extern void	Aitoa( char *pstr, uint16_t data, uint16_t keta , uint8_t zs ) ;
extern uint16_t	Astoi( uint8_t *pstr, uint16_t len ) ;
extern void	Aitos( uint8_t *pstr, uint16_t data, uint16_t keta , uint8_t zs ) ;
extern	void	AitoDec( char *pstr, short data, uint16_t keta , uint8_t zs );
extern uint32_t	Aatol( char *pstr , uint16_t len ) ;
extern float	Aatof( char *pstr  ) ;
extern void	Altoa( char *pstr, uint32_t data, uint16_t keta , uint8_t zs ) ;
extern uint32_t	Astol( uint8_t *pstr, uint16_t len ) ;
extern void	Altos( uint8_t *pstr, uint32_t data, uint16_t keta , uint8_t zs ) ;
extern	void	Aftoa( char *pstr, float f );
extern uint8_t	AhextoAsc( uint8_t hexdata ) ;
extern uint8_t	AasctoHex( uint8_t ascdata ) ;
extern uint16_t	StrToUh( uint8_t *p ) ;
extern void	UhToStr( uint8_t *p , uint16_t d ) ;
extern uint16_t Bcd2Bin(uint16_t bcd );
extern uint16_t Bin2Bcd(uint16_t bin );
extern uint16_t get_uword_le(uint8_t *ub);
extern uint16_t get_uword_be(uint8_t *ub);
extern int32_t get_long_be(uint8_t *ub);

extern void put_uword_le(uint8_t *dst, uint16_t src);
extern void put_uword_be(uint8_t *dst, uint16_t src);
