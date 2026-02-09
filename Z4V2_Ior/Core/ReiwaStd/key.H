
/*** NM KEY.H ******/
#define K_ON     (uint8_t)0x20
#define K_RX10   (uint8_t)0x40
#define K_RX20   (uint8_t)0x60
#define	K_OFF	 (uint8_t)0x80

#define	K_PONPOFF		(uint8_t)0x01
#define	K_MODE	    	(uint8_t)0x02
#define	K_TIMRESET		(uint8_t)0x04

#define K_MASK  (uint8_t)0x1F // どのボタンがどういう状態かをORでマスクする
#define K_RMASK  (uint8_t)0xE0

extern uint8_t KEY_get( void );
extern uint8_t KEY_pget( void );
extern void KEY_clr( void );
extern void KEY_init( void );
extern	uint16_t KEY_cnt( void ); //キー入力情報があるかどうか
extern	void	KeyScan( void ); // 割込みから読み込む　1msに一回　ポートを見て確認　ヒステリシス特性あり
extern	uint16_t KEY_Port( void ); // H or L
extern	uint8_t KEY_isPush(void); // キーが押されているかどうか

