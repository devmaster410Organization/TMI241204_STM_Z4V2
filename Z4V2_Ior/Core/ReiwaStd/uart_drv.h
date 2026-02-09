/*
 * uart2_drv.h Ver.2.0.0
 *
 *  Created on: 2020/07/05
 *      Author: ysugawara
 */


/*
 * オーバーランエラー等対策のために筆つようなときにHAL_UART_Abort 関数を追加する。
 *
 */
#include "fifo.h"


#ifndef SRC_UART_DRV_H_
#define SRC_UART_DRV_H_

#define UARTMAN_SZ	2

#define UART_OK	0
#define UART_ERR	1

typedef struct {
	UART_HandleTypeDef *phuart;

	uint8_t *rxbuftop;
	uint32_t rxbuf_sz;

	uint8_t *txbuftop;
	uint32_t txbuf_sz;

	//---------- work memory
	FIFO txfifo;
	FIFO rxfifo;
	uint8_t rc;
	uint8_t tc;
	uint32_t rxp;
	volatile	uint8_t flg_snd;

    void (*tr485sta_job)(void);
    void (*tr485fin_job)(void);

}T_UART_MAN;


void UART_init( void );
void UART_create( T_UART_MAN *ptuartman );
void UART_rcv_clr( T_UART_MAN *ptuartman );
void UART_set_485( T_UART_MAN *ptuartman ,  void (*tr485sta_job)(void),void (*tr485fin_job)(void) );
uint8_t UART_clr_error( T_UART_MAN *ptuartman);
uint8_t UART_rcv( T_UART_MAN *ptuartman,char *ch );
uint8_t UART_putc( T_UART_MAN *ptuartman,char d);
uint8_t UART_puts( T_UART_MAN *ptuartman,char * str ,uint32_t timeout);
uint8_t UART_nputs( T_UART_MAN *ptuartman,char *data , uint32_t len ,uint32_t timeout);
uint8_t UART_isSending( T_UART_MAN *ptuartman);


#endif /* SRC_UART2_DRV_H_ */
