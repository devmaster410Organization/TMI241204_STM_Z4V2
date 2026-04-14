#include "main.h"
#include "prj.h"
#include "uart_drv.h"
#include <string.h>

//


int errcnt = 0;
uint32_t tobak;


T_UART_MAN *uartman[UARTMAN_SZ];

uint32_t uartmanp ;

uint32_t uart_err_cnt[4] = {0};



void UART_init( void )
{
	__disable_irq();
	for(int i = 0;i<UARTMAN_SZ;i++ ){
		uartman[i] = NULL;
	}
	uartmanp = 0;
	__enable_irq();
}

void UART_create( T_UART_MAN *ptuartman )
{
	__disable_irq();
	uartman[uartmanp] = ptuartman;


	uartmanp++;

	AfifoInit(&ptuartman->rxfifo,ptuartman->rxbuftop,ptuartman->rxbuf_sz );
	AfifoInit(&ptuartman->txfifo,ptuartman->txbuftop,ptuartman->txbuf_sz );


	ptuartman->flg_snd = 0;
	ptuartman->rxp = 0;
	ptuartman->tr485sta_job = NULL;
	ptuartman->tr485fin_job = NULL;
	HAL_UART_Receive_IT(ptuartman->phuart,&ptuartman->rc ,1);	//1文字受信で割り込み
	__enable_irq();
}

void UART_set_485( T_UART_MAN *ptuartman ,  void (*tr485sta_job)(void),void (*tr485fin_job)(void) )
{
	ptuartman->tr485sta_job = tr485sta_job;
	ptuartman->tr485fin_job = tr485fin_job;
}

uint8_t UART_clr_error( T_UART_MAN *ptuartman)
{
	uint8_t ret = UART_OK;
	if ( __HAL_UART_GET_FLAG(ptuartman->phuart, UART_FLAG_ORE) ||
			__HAL_UART_GET_FLAG(ptuartman->phuart, UART_FLAG_NE) ||
			__HAL_UART_GET_FLAG(ptuartman->phuart, UART_FLAG_FE) ||
			__HAL_UART_GET_FLAG(ptuartman->phuart, UART_FLAG_PE) ){
		volatile uint8_t dummy1,dummy2;

		if( __HAL_UART_GET_FLAG(ptuartman->phuart, UART_FLAG_ORE)){
			uart_err_cnt[0]++;
		}
		if( __HAL_UART_GET_FLAG(ptuartman->phuart, UART_FLAG_NE)){
			uart_err_cnt[1]++;
		}
		if( __HAL_UART_GET_FLAG(ptuartman->phuart, UART_FLAG_FE)){
			uart_err_cnt[2]++;
		}
		if( __HAL_UART_GET_FLAG(ptuartman->phuart, UART_FLAG_PE)){
			uart_err_cnt[3]++;
		}
		__disable_irq();
#if 0  // STM32FL0 seriesee
		dummy1 = ptuartman->phuart->Instance->ISR;
		dummy2 = ptuartman->phuart->Instance->RDR;
#else	// STM32F4 seriese
		dummy1 = ptuartman->phuart->Instance->ISR; // <-SR
		dummy2 = ptuartman->phuart->Instance->RDR; // <-DR
#endif
		HAL_UART_Abort(ptuartman->phuart);
		HAL_UART_Receive_IT(ptuartman->phuart,&ptuartman->rc,1);
		__enable_irq();
		 ret = UART_ERR;
	}

	return ret;
}


void UART_rcv_clr( T_UART_MAN *ptuartman )
{
	char dummy;
	int cnt=0;
//	UART_clr_error(ptuartman);
	while(UART_rcv(ptuartman,&dummy)==UART_OK){
		cnt++;
		if(cnt >= ptuartman->rxbuf_sz){
			// error;
			break;
		}
	}
}

uint8_t UART_rcv( T_UART_MAN *ptuartman,char *ch )
{
	bool boo;
	uint8_t ret;
	uint8_t dc;
	UART_clr_error(ptuartman);
	__disable_irq();
	boo = AfifoGet(&ptuartman->rxfifo,&dc);
	__enable_irq();


	if( boo == true ){
		*ch = dc;
		ret = UART_OK;
	}else{
		ret = UART_ERR;
	}
	return ret;
}


// send 1char from fifo buffer
uint8_t tx1char(T_UART_MAN *ptuartman )
{
	HAL_StatusTypeDef halstat;
	uint8_t ret = UART_OK;
	bool boo;  
	boo = AfifoGet(&ptuartman->txfifo,&ptuartman->tc);
	if(boo == true){
	  	halstat = HAL_UART_Transmit_IT( ptuartman->phuart,&ptuartman->tc,1);
		if(halstat == HAL_OK){
			ptuartman->flg_snd = 1;
			ret = UART_OK;
		}else{
			errcnt++;
			ret = UART_ERR;
		}
	}
	return ret;
}

uint8_t UART_putc(T_UART_MAN *ptuartman,char d )
{
	bool boo;
	uint8_t ret;
	__disable_irq();

	if(UART_isSending(ptuartman) == 0 ){	//送信中でなければ
		boo = AfifoPut(&ptuartman->txfifo,d );
		if(	ptuartman->tr485sta_job != NULL ){
			ptuartman->tr485sta_job();
		}

		tx1char(ptuartman);
		ret = UART_OK;
	}else{
		boo = AfifoPut(&ptuartman->txfifo,d );
		if(boo==true){
			ret = UART_OK;
		}else{
			ret = UART_ERR;
		}
	}
	__enable_irq();
	return ret;
}

/*
 * 文字列の送信
 * char *str 送信文字列
 *
 */
uint8_t UART_puts( T_UART_MAN *ptuartman,char * str ,uint32_t timeout)
{
	uint8_t ret;
	while(*str){
		ret = UART_putc(ptuartman,*str );
		if(ret == UART_OK){
			str++;
		}else{
#ifdef _CMSIS_OS_H
			osDelay(1);
#else
			HAL_Delay(1);
#endif
			timeout--;
			if(timeout==0){
				ret = UART_ERR;
				break;
			}
		}

	}

	return ret;
}
/*
 * 指定文字数の送信
 * uint8_t *data  送信文字数 (バイト)
 * uint32_t len
 *
 */
uint8_t UART_nputs( T_UART_MAN *ptuartman,char *data , uint32_t len ,uint32_t timeout)
{
	uint8_t ret;

	while(len){
		ret = UART_putc(ptuartman,*data );
		if(ret == UART_OK){
			data++;
			len--;
		}else{
#ifdef _CMSIS_OS_H
			osDelay(1);
#else
			HAL_Delay(1);
#endif
			timeout--;
			if(timeout==0){
				ret = UART_ERR;
				break;
			}
		}
	}
	return ret;
}

uint8_t UART_isSending(  T_UART_MAN *ptuartman )
{
	return 		ptuartman->flg_snd ;
}



/**
  * @brief  Tx Half Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */


void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	for(int i=0;i<uartmanp;i++){
		if(huart == uartman[i]->phuart ){
			break;	//一度ヒットしたらループから抜ける
		}
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	for(int i=0;i<uartmanp;i++){
		if(huart == uartman[i]->phuart ){
			if(AfifoCount(&uartman[i]->txfifo)){
				tx1char(uartman[i]);
			}else{
				uartman[i]->flg_snd = 0;
				if(	uartman[i]->tr485fin_job != NULL ){
					uartman[i]->tr485fin_job();
				}
			}
			break;	//一度ヒットしたらループから抜ける
		}
	}
}

/**
  * @brief  Rx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
	for(int i=0;i<uartmanp;i++){
		if(huart == uartman[i]->phuart ){
			AfifoPut(&uartman[i]->rxfifo,uartman[i]->rc);
			HAL_UART_Receive_IT(huart,&uartman[i]->rc, 1 );
			break;	//一度ヒットしたらループから抜ける
		}
	}
}


void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	for(int i=0;i<uartmanp;i++){
		if(huart == uartman[i]->phuart ){
			break;	//一度ヒットしたらループから抜ける
		}
	}
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	for(int i=0;i<uartmanp;i++){
		if(huart == uartman[i]->phuart ){
			break;	//一度ヒットしたらループから抜ける
		}
	}
}
void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart)
{
	for(int i=0;i<uartmanp;i++){
		if(huart == uartman[i]->phuart ){
			break;	//一度ヒットしたらループから抜ける
		}
	}
}
void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef *huart)
{
	for(int i=0;i<uartmanp;i++){
		if(huart == uartman[i]->phuart ){
			break;	//一度ヒットしたらループから抜ける
		}
	}
}
void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{
	for(int i=0;i<uartmanp;i++){
		if(huart == uartman[i]->phuart ){
			break;	//一度ヒットしたらループから抜ける
		}
	}
}



