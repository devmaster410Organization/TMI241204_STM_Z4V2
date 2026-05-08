/*
 * tsk_modbus.c
 *
 *  Created on: Apr 14, 2026
 *      Author: ysuga
 */

 
#include "prj.h"


static void uart_init(void);




T_UART_MAN tUartRs485;
#define UartRs485RXBUF_SZ 270
static uint8_t UartRs485_rxbuf[UartRs485RXBUF_SZ];

extern uint16_t CRC_calc(uint8_t *nData, uint16_t wLength);


// prottype
void tsk_rs485(void);
static void MX_USART3_UART_Init_MODBUS(void);
static void uart_init(void);
void tsk_modbus_slave(void);
void MODBUS_init(void);
int MODBUSrtu_slave_job(void);
static int analyze_modbus_rtu(uint8_t rcv[], uint16_t len);
static int crc_check( uint8_t *ptop, uint16_t len) ;
static void make_err_code(uint8_t errcode, uint8_t rcv[]);
static void sub_func_read_n(uint8_t rcv[]);
static void sub_func_write_n(uint8_t rcv[]);
static void sub_func_write_1(uint8_t rcv[]);
static void sub_func_echoback(uint8_t rcv[]);
static void sub_error_code(uint8_t errcode);



void tsk_modbus_slave( void )
{
	uart_init();
	MX_USART3_UART_Init_MODBUS();
	osDelay(1500);

	for(;;){
		MODBUS_init();
		MODBUSrtu_slave_job();
	}
}



static void uart_init(void) {
  UART_init();

	tUartRs485.phuart = &huart3;
	tUartRs485.rxbuftop = UartRs485_rxbuf;
	tUartRs485.rxbuf_sz = sizeof(UartRs485_rxbuf);
	UART_create(&tUartRs485);
}



/*
 * modbus slave mode sources
 */

#define MODBUS_UNIT_MAX	99

#define MBTCP_HEADER_SZ	6
#define MBTCP_LEN_POS	4

#define BLOAD_CAST_ID 0x00  //MODBUS

// 通信モード
#define XPMODE_WAIT   0
#define XPMODE_HEADER	1
#define XPMODE_BODY	2
#define	XPMODE_THRUEGH	3
#define XPMODE_ERROR	4

#define XPMODE_RECIEVING 5
#define XPMODE_WAIT_SILENT 6

// MOD BUS ファンクションコード
#define MODBUS_FUNC_READ_N  0x03
#define MODBUS_FUNC_WRITE_N  0x10
#define MODBUS_FUNC_WRITE_1 0x06
#define MODBUS_FUNC_ECOHBACK 0x08

#define MODBUS_FUNC_ECOHBACK_ERR 0x88

// MOD BUS errコード
#define EXCEPTION_CODE_OK 0x00
#define	EXCEPTION_CODE_ILLIGAL_FUNCTION 0x01
#define EXCEPTION_CODE_ILLIGAL_ADDRESS 0x02
#define EXCEPTION_CODE_UNACCEPTABLE_DATA 0x03
#define ERRCODE_04_DOSA_ERR 0x04

// MOD BUS ファンクションコードのパケット長
//#define MODBUS_FUNC_READ_N_LEN  (MBTCP_HEADER_SZ+6)
#define MODBUS_FUNC_WRITE_1_LEN 8
#define MODBUS_FUNC_ECOHBACK_LEN 8

typedef struct {
	// modbus 通信制御
	uint8_t mode;
	uint16_t rcvbufp;
	uint8_t rcvbuf[512];
	uint8_t txbuf[512];
	uint16_t txcnt;

	//
//	uint16_t address;
	uint16_t num;
	uint16_t bytenum;
	uint16_t data[128];

	uint16_t board_sel;
	//

	uint8_t slave_add;
	uint8_t function_code;
	uint16_t add; //レジスタのアドレス
	uint16_t param1; //パラメータ

	//
	uint16_t silent_limmit;	//
} T_MODBUSTCP;

T_MODBUSTCP tModBus;


int16_t MDBS_set_reg_com[16];
int16_t MDBS_set_reg_rcv[16][8];
int16_t MDBS_set_reg_rsv[16];



/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init_MODBUS(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  switch(g_setup.baudrate){
	case PRM_BAUDRATE_115200:		huart3.Init.BaudRate = 115200;		break;
	case PRM_BAUDRATE_57600:		huart3.Init.BaudRate = 57600;		break;
	case PRM_BAUDRATE_38400:		huart3.Init.BaudRate = 38400;		break;
	case PRM_BAUDRATE_19200:		huart3.Init.BaudRate = 19200;		break;
	case PRM_BAUDRATE_9600:			huart3.Init.BaudRate = 9600;		break;
	default:						huart3.Init.BaudRate = 38400;		break;
  }
  switch(g_setup.bit_length){
	case PRM_DATA_BIT_7:			huart3.Init.WordLength = UART_WORDLENGTH_7B;	break;	
	case PRM_DATABITS_8:			huart3.Init.WordLength = UART_WORDLENGTH_8B;	break;
	default:						huart3.Init.WordLength = UART_WORDLENGTH_8B;	break;
  }
  switch(g_setup.stop_bit){
	case PRM_STOP_BIT_1:			huart3.Init.StopBits = UART_STOPBITS_1;		break;
	case PRM_STOP_BIT_2:			huart3.Init.StopBits = UART_STOPBITS_2;		break;
	default:						huart3.Init.StopBits = UART_STOPBITS_1;		break;
  }
  switch(g_setup.parparityity){
	case PRM_PARITY_NONE:			huart3.Init.Parity = UART_PARITY_NONE;		break;
	case PRM_PARITY_EVEN:			huart3.Init.Parity = UART_PARITY_EVEN;		break;
	case PRM_PARITY_ODD:			huart3.Init.Parity = UART_PARITY_ODD;		break;
	default:						huart3.Init.Parity = UART_PARITY_NONE;		break;
  }

  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  if (HAL_RS485Ex_Init(&huart3, UART_DE_POLARITY_HIGH, 0, 0) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_EnableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

void MODBUS_init(void) {
	tModBus.mode = XPMODE_WAIT;
	tModBus.rcvbufp = 0;


	memset(MDBS_set_reg_com,0,sizeof(MDBS_set_reg_com));
	memset(MDBS_set_reg_rcv,0,sizeof(MDBS_set_reg_rcv));
	memset(MDBS_set_reg_rsv,0,sizeof(MDBS_set_reg_rsv));

	tModBus.silent_limmit = 10;
	g_setup.modbus_slave_address = 10;
}



/// @brief MODBUS RTUスレーブのメインループ。MODBUS RTUパケットの受信と解析を行う。
/// @param  なし
/// @return なし:無限ループ関数
int MODBUSrtu_slave_job(void) 
{
	uint32_t interval = 0;
	int ret = 0;
	char c;

	for (;;) {
 
		if (UART_rcv(&tUartRs485, &c) == UART_OK) {
PORT_TGL(TP8);
			interval = 0;

			switch (tModBus.mode) {
			case XPMODE_WAIT:
				tModBus.mode = XPMODE_RECIEVING;
				tModBus.rcvbufp = 0;
				tModBus.rcvbuf[tModBus.rcvbufp] = c;
				tModBus.rcvbufp++;
				break;
			case XPMODE_RECIEVING:
				tModBus.rcvbuf[tModBus.rcvbufp] = c;
				tModBus.rcvbufp++;
				if (tModBus.rcvbufp >= sizeof(tModBus.rcvbuf)) {
					tModBus.mode = XPMODE_WAIT_SILENT;
					tModBus.rcvbufp = 0;
				}
				break;
			}

		} else {
			osDelay(1); //最大 1mSec待ち。実際はUART受信待ちでほとんどここで待つことになる。
			if (interval < 1000) {
				interval++;
			}
			switch (tModBus.mode) {
			case XPMODE_WAIT:
				break;
			case XPMODE_RECIEVING:
				if (interval >= 2) { //受信完了
					ret = analyze_modbus_rtu(tModBus.rcvbuf, tModBus.rcvbufp);
					tModBus.mode = XPMODE_WAIT;
					tModBus.rcvbufp = 0;
					;
				}
				break;

			case XPMODE_WAIT_SILENT:
				if (interval >= 3) { //受信終了。新規コマンド入力に移行する。
					tModBus.mode = XPMODE_WAIT;
					tModBus.rcvbufp = 0;
					;
				}
				break;
			default:
				break;
			}
		}
	}

	return ret;
}



 /// @brief MODBUS RTUパケットの解析
 /// @param pub 受信チャンクの先頭アドレス
 /// @param len 受信チャンクの文字数
 /// @return  0:パケット受信なし 1:パケット受信成功
 static int analyze_modbus_rtu(uint8_t *pub, uint16_t len) {
	uint16_t crc1, crc2;
	uint16_t bytenum;
	uint8_t *ptop;
	uint8_t rspflg = 0;

	ptop = pub;
	// Slave アドレス

	if(len < 5) {
		goto err;
	}
	tModBus.slave_add = *pub;
	if ((tModBus.slave_add != 0) && (tModBus.slave_add != g_setup.modbus_slave_address)) {	//自分と関係ないパケット
		goto err;
	}
	pub++;

	tModBus.function_code = *pub;
	// ファンクションコード
	switch (*pub) {
	case MODBUS_FUNC_READ_N: // 03 変数読み出し
//            sub_error_code(EXCEPTION_CODE_ILLIGAL_FUNCTION);
		rspflg = 1;

		pub++;
		tModBus.add = get_uword_be(pub);
		pub += 2;
		tModBus.param1 = get_uword_be(pub);
		if (tModBus.param1 == 0)
			goto err;
		//あり得ない数値
		if (tModBus.param1 > 106)
			goto err;
		//あり得ない数値
		pub += 2;
		crc1 = CRC_calc(ptop, pub - ptop);
		crc2 = get_uword_le(pub);
		if (crc1 == crc2) {
			if (tModBus.slave_add == g_setup.modbus_slave_address) { //自IDの時のみ
				sub_func_read_n(ptop);
				rspflg = 1;
			}
			// 送信する。
		} else { //CRCエラー
			goto err;
		}

		break;

	case MODBUS_FUNC_WRITE_N: // 16 変数書き出し
		pub++;
		tModBus.add = get_uword_be(pub);
		pub += 2;

		tModBus.param1 = get_uword_be(pub);
		if (tModBus.param1 == 0)
			goto err;
		//あり得ない数値
		if (tModBus.param1 > 106)
			goto err;
		//あり得ない数値
		pub += 2;
		bytenum = *pub;
		pub++;
		pub += bytenum;

		crc1 = CRC_calc(ptop, pub - ptop);
		crc2 = get_uword_le(pub);
		if (crc1 == crc2) {
			sub_func_write_n(ptop);
			if (tModBus.slave_add == g_setup.modbus_slave_address) {
				rspflg = 1;
			}
			// 送信する。
		} else { //CRCエラー
			goto err;
		}
		break;
	case MODBUS_FUNC_WRITE_1: // 06 変数書き出し
		pub++;
		tModBus.add = get_uword_be(pub);
		pub += 2;
		tModBus.param1 = get_uword_be(pub);
		pub += 2;

		crc1 = CRC_calc(ptop, pub - ptop);
		crc2 = get_uword_le(pub);

		if (crc1 == crc2) {
			if ((tModBus.slave_add == g_setup.modbus_slave_address) || (tModBus.slave_add == 0)) {
				sub_func_write_1(ptop);
				if (tModBus.slave_add == g_setup.modbus_slave_address) {
					rspflg = 1;
				}
			}
			// 送信する。
		} else { //CRCエラー
			goto err;
		}
		break;
	case MODBUS_FUNC_ECOHBACK: // 08 エコーバックテスト
	    if( crc_check( ptop,len ) ){
			sub_error_code(EXCEPTION_CODE_ILLIGAL_FUNCTION);
			rspflg = 1;
		} else{
			rspflg = 0;
		}
		break;
	default:	//サポートしていないパケット　または　ノイズのパケット
	    if( crc_check( ptop,len ) ){	
			sub_error_code(EXCEPTION_CODE_ILLIGAL_FUNCTION);
			rspflg = 1;
		} else{
			rspflg = 0;
		}
		break;
	}
	if ((tModBus.txcnt != 0) && rspflg) {
		crc1 = CRC_calc(&tModBus.txbuf[0], tModBus.txcnt);
		put_uword_le(&tModBus.txbuf[tModBus.txcnt], crc1);
		tModBus.txcnt += 2;
		/*		if( tRemote.respdelay ){
		 tslp_tsk( tRemote.respdelay*10 );
		 }
		 */
		osDelay(g_setup.response_delay_ms);

		UART_nputs(&tUartRs485, (char*) tModBus.txbuf, tModBus.txcnt);
		while( UART_isSending(&tUartRs485)  ){
			osDelay(1);
		}
	}
	return 1;
	err:

	return 0;
}


static int crc_check( uint8_t *ptop, uint16_t len) 
{
	int ret;
	uint16_t crc1, crc2;
	crc1 = CRC_calc(ptop, len-2);
	crc2 = get_uword_le(ptop+len-2);
	if(crc1==crc2){
		ret = 1;
	}else{
		ret = 0;
	}
	return ret;

}




static void make_err_code(uint8_t errcode, uint8_t rcv[]) {
	uint16_t ptr;
	ptr = 0;

	tModBus.txbuf[ptr] = rcv[ptr];
	ptr++; //slave address
	tModBus.txbuf[ptr] = rcv[ptr] | 0x80;
	ptr++; // function のMSBを立てる
	tModBus.txbuf[ptr] = errcode;
	ptr++;

	tModBus.txcnt = ptr;
}



static void sub_func_read_n(uint8_t rcv[]) {
	int i;
	uint16_t ptr;
	uint16_t add, num;
	int16_t val;
	int rslt;
	uint16_t sz;

	ptr = 0;
	sz = 0;
	tModBus.slave_add = get_uword_be(&rcv[sz + 2]);
	tModBus.num = get_uword_be(&rcv[sz + 4]);

	if (tModBus.num >= 255)
		goto err;
	add = tModBus.slave_add;
	num = tModBus.num;

	tModBus.txbuf[ptr] = rcv[ptr];
	ptr++; //slave address
	tModBus.txbuf[ptr] = rcv[ptr];
	ptr++; //function code
	tModBus.txbuf[ptr] = num * 2;
	ptr++; //

	for (i = 0; i < num; i++) {
		rslt = MODBUS_get_reg(add, &val);
		if (rslt == EXCEPTION_CODE_OK) {
			put_uword_be(&tModBus.txbuf[ptr], val);
			ptr += 2;
			add++;
		} else {	//エラー時　(アドレスが違うなど)
			goto err;
		}
	}
	tModBus.txcnt = ptr;
	return;
	err: make_err_code(EXCEPTION_CODE_UNACCEPTABLE_DATA, rcv);
	return;

}

static void sub_func_write_n(uint8_t rcv[]) {
	int ret;
	int i;
	uint16_t add;
	uint16_t ptr;

	ptr = 0;
	tModBus.txbuf[ptr] = rcv[ptr];
	ptr++;	//slave address
	tModBus.txbuf[ptr] = rcv[ptr];
	ptr++;	//function code

	tModBus.slave_add = get_uword_be(&rcv[ptr]);
	tModBus.txbuf[ptr] = rcv[ptr];
	ptr++;	//start address u
	tModBus.txbuf[ptr] = rcv[ptr];
	ptr++;	//start address l
	tModBus.num = get_uword_be(&rcv[ptr]);
	tModBus.txbuf[ptr] = rcv[ptr];
	ptr++;	//start address u
	tModBus.txbuf[ptr] = rcv[ptr];
	ptr++;	//start address l
	tModBus.txcnt = ptr;

	tModBus.bytenum = rcv[ptr];
	ptr++;
	if (tModBus.num * 2 != tModBus.bytenum) {
//			make_err_code(EXCEPTION_CODE_ILLIGAL_ADDRESS);
		make_err_code(EXCEPTION_CODE_UNACCEPTABLE_DATA, rcv);
		goto err;
	}
	add = tModBus.slave_add;
	for (i = 0; i < tModBus.num; i++) {
		tModBus.data[i] = get_uword_be(&rcv[ptr]);
		ptr += 2;
		ret = MODBUS_set_reg(add, tModBus.data[i]);
		add++;
	}
	err: return;
}

static void sub_func_write_1(uint8_t rcv[]) {
	int16_t val;
	uint16_t ptr;
	uint16_t rslt;

	ptr = 0;
	tModBus.txbuf[ptr] = rcv[ptr];
	ptr++;	//slave address
	tModBus.txbuf[ptr] = rcv[ptr];
	ptr++;	//function code
	tModBus.slave_add = get_uword_be(&rcv[ptr]);
	tModBus.txbuf[ptr] = rcv[ptr];
	ptr++;	//start address u
	tModBus.txbuf[ptr] = rcv[ptr];
	ptr++;	//start address l

	tModBus.data[0] = get_uword_be(&rcv[ptr]);
	MODBUS_set_reg(tModBus.slave_add, tModBus.data[0]);
	rslt = MODBUS_get_reg(tModBus.slave_add, &val);
	if (rslt == EXCEPTION_CODE_OK) {
		put_uword_be(&tModBus.txbuf[ptr], val);
		ptr += 2;
		tModBus.txcnt = ptr;
	} else {	//エラー時　(アドレスが違うなど)
		make_err_code(EXCEPTION_CODE_ILLIGAL_ADDRESS, rcv);
	}

}

static void sub_func_echoback(uint8_t rcv[]) {
	uint16_t add, val;
	uint16_t header_sz;

	header_sz = 0;
	add = get_uword_be(&rcv[header_sz + 2]);
	val = get_uword_be(&rcv[header_sz + 4]);
	if (add != 0) {
		tModBus.txbuf[header_sz + 1] = MODBUS_FUNC_ECOHBACK_ERR;
	} else {
		tModBus.txbuf[header_sz + 1] = MODBUS_FUNC_ECOHBACK;
	}
	put_uword_be(&tModBus.txbuf[header_sz + 2], 0);
	put_uword_be(&tModBus.txbuf[header_sz + 4], val);
	tModBus.txcnt = 6;
}

/**********************************************************
 エラーコードの伝文を作成する。


 引数
 _UBYTE errcode	:エラーコード
 詳細はMODBUSの仕様を参照


 返り値
 なし
 **********************************************************/
static void sub_error_code(uint8_t errcode) {
	uint8_t *pub;
	tModBus.txcnt = 0;
	if (tModBus.slave_add == g_setup.modbus_slave_address) {
		pub = &tModBus.txbuf[0];
		*pub = tModBus.slave_add;
		pub++;
		*pub = tModBus.function_code;
		pub++;
		*pub = errcode;
		pub++;
		tModBus.txcnt = pub - tModBus.txbuf;
	} else {
		//　違う

	}
}

