/*
 * tsk_usb.c
 *
 *  Created on: May 14, 2025
 *      Author: ysuga
 *  USB VCP User Interface:
 *  STM32のUSB機能を用いる。UART-USB変換ではないので、シリアルポート設定は不要(なんでもよい)
 *  受信の改行コードは CR+LF または CR または LF
 *  送信の改行コードは CR+LF
 *  Echo backはしないので　ターミナルでエコーバックをONにしてください。
 * 　BSはサポートしています。 矢印キーはサポートしていません。
 * 　ターミナルはTeraTermを推奨します。
 *
 */
#include <ctype.h>
#include "prj.h"
#include "usbd_cdc.h"

extern osMessageQId usbRcvQueue01Handle;
extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

static int usb_putchar( char c );
static int usb_puts( const char *str );
static int analyze_command( char *buf );
static uint16_t usbGetChar( uint32_t timeout );
static void usbUngetChar( uint16_t c);
static void usbEchoBack( char c);
static int cmd_set(  void );
static int cmd_get(  void );
static int cmd_status(  void );
static int cmd_mode(  void );

static int copy_word_to_buf( uint16_t index, char *out, size_t out_size );
static bool parse_u32_token( const char *token, uint32_t *out );
static bool parse_u8_token( const char *token, uint8_t *out );
static bool parse_u16_token( const char *token, uint16_t *out );
static bool parse_ipv4_token( const char *token, uint8_t ip[4] );
static bool is_valid_float_token( const char *token );
static bool parse_float_token( const char *token, float *out );
static void show_setup_param_help( void );
static void print_setup_param( const char *param );
static void print_all_setup_params( void );
static int set_setup_param( const char *param, const char *value );

/* Setup parameter name string constants (one copy shared across all functions) */
static const char * const PSTR_MODBUS_SLAVE_ADDRESS  = "modbus_slave_address";
static const char * const PSTR_RS485_BAUDRATE        = "rs485_baudrate";
static const char * const PSTR_RS485_STOP_BIT        = "rs485_stop_bit";
static const char * const PSTR_RS485_PARITY          = "rs485_parity";
static const char * const PSTR_RS485_BIT_LENGTH      = "rs485_bit_length";
static const char * const PSTR_RESPONSE_DELAY_MS     = "response_delay_ms";
static const char * const PSTR_LEAKAGE_LOW_CUT       = "leakage_low_cut";
static const char * const PSTR_AC_PHASE_WIRE         = "ac_phase_wire";
static const char * const PSTR_CT_TYPE1              = "ct_type1";
static const char * const PSTR_CT_TYPE2              = "ct_type2";
static const char * const PSTR_CT_TYPE3              = "ct_type3";
static const char * const PSTR_CT_TYPE4              = "ct_type4";
static const char * const PSTR_AVARAGE_COUNT         = "avarage_count";
static const char * const PSTR_VOLT_CALIB1_GAIN      = "volt_calib1_gain";
static const char * const PSTR_VOLT_CALIB1_OFFSET    = "volt_calib1_offset";
static const char * const PSTR_VOLT_CALIB2_GAIN      = "volt_calib2_gain";
static const char * const PSTR_VOLT_CALIB2_OFFSET    = "volt_calib2_offset";
static const char * const PSTR_VOLT_CALIB3_GAIN      = "volt_calib3_gain";
static const char * const PSTR_VOLT_CALIB3_OFFSET    = "volt_calib3_offset";
static const char * const PSTR_LEAKAGE_CALIB1_GAIN   = "leakage_calib1_gain";
static const char * const PSTR_LEAKAGE_CALIB1_OFFSET = "leakage_calib1_offset";
static const char * const PSTR_LEAKAGE_CALIB2_GAIN   = "leakage_calib2_gain";
static const char * const PSTR_LEAKAGE_CALIB2_OFFSET = "leakage_calib2_offset";
static const char * const PSTR_LEAKAGE_CALIB3_GAIN   = "leakage_calib3_gain";
static const char * const PSTR_LEAKAGE_CALIB3_OFFSET = "leakage_calib3_offset";
static const char * const PSTR_LEAKAGE_CALIB4_GAIN   = "leakage_calib4_gain";
static const char * const PSTR_LEAKAGE_CALIB4_OFFSET = "leakage_calib4_offset";


#define USB_RCV_BUFSIZE  CDC_DATA_FS_MAX_PACKET_SIZE
#define INPUT_WORD_MAX  10
typedef struct{
  char rcvbuf[USB_RCV_BUFSIZE];
  uint32_t rcvbufp;
  uint32_t rcvtimeout;
  char usbtxbuf[USB_RCV_BUFSIZE];
  uint16_t usbtxbufp;


  char* word_top_ptr[INPUT_WORD_MAX];	// 各wordの先頭ポインタ
  uint16_t word_len[INPUT_WORD_MAX];	// 各wordの長さ
  uint16_t word_num; // word_top_ptr/word_lenのインデックス

  uint16_t ungetdata; // free = USB_RCV_TIMEOUT_CODE,
  bool echo_flg;
} USB_CONTROL_BLOCK;

USB_CONTROL_BLOCK usbcb;


#define USB_RCV_TIMEOUT_CODE 0xFFFF


/// @brief
/// @param timeout  Queue timeout in RTOS ticks
/// @return 0xFFFF:timeout 0x0000-0x00FF:received data
uint16_t usbGetChar( uint32_t timeout )
{
   uint16_t data ;
   if( USB_RCV_TIMEOUT_CODE == usbcb.ungetdata ){
		uint8_t msg_prio;
		uint16_t msg;
		osStatus_t status = osMessageQueueGet(queue_USBHandle,&msg,&msg_prio,timeout);
		switch( status ){
			case osOK:
				data = msg;
				break;
			case osErrorTimeout:
				data = USB_RCV_TIMEOUT_CODE;
				break;
			default:
				data = USB_RCV_TIMEOUT_CODE;
				break;
		}
	}else{
		data = usbcb.ungetdata;
		usbcb.ungetdata = USB_RCV_TIMEOUT_CODE; // reset
	}
    return data;
}


/// @brief usbGetCharで受け取ったデータを戻す
/// @param c
/// @return
static void usbUngetChar( uint16_t c)
{
	usbcb.ungetdata = c;
}

/// @brief
/// @param c
static void usbEchoBack( char c)
{
	if( usbcb.echo_flg ){
		usb_putchar(c);
	}
}

/// @brief usb = VCP USer interface.
/// @param  none
#define UI_TIMEOUT 100000  // msec
#define USB_RCV_TIMEOUT 50 // msec
void tsk_usb( void )
{
	uint16_t rcvdata;
	char key;	//rcvdata のキャラクタ部(下位8bit)
	usbcb.rcvbufp = 0;
	usbcb.usbtxbufp = 0;
	usbcb.rcvtimeout = 0;
	usbcb.ungetdata = USB_RCV_TIMEOUT_CODE;
	usbcb.echo_flg = true;

	for (;;) {
//		HAL_USB_GetState(&hUsbDeviceFS); // USBの状態を確認
        rcvdata = usbGetChar(USB_RCV_TIMEOUT);
        if (rcvdata != USB_RCV_TIMEOUT_CODE) {
            switch(USBMSG_GET_SRC(rcvdata)){
				case USBMSG_SRC_USB:	
					key = (char)USBMSG_GET_CHAR(rcvdata);
					usbcb.rcvtimeout = 0;	// reset timeout
					if (usbcb.rcvbufp < USB_RCV_BUFSIZE) {
						if (key == 0x0D || key == 0x0A) { // CR or LF
							usbEchoBack(key);
							rcvdata = usbGetChar( 1 );
							if( rcvdata == USB_RCV_TIMEOUT_CODE ){
								// timeout, do nothing
							} else if( (USBMSG_GET_SRC(rcvdata) == USBMSG_SRC_USB) &&
									((USBMSG_GET_CHAR(rcvdata) == 0x0D) || (USBMSG_GET_CHAR(rcvdata) == 0x0A)) ){ // CR or LF
								key = (char)USBMSG_GET_CHAR(rcvdata);
								usbEchoBack(key);
							}else{
								usbUngetChar( rcvdata ); // unget
							}
							if (usbcb.rcvbufp) {
								usbcb.rcvbuf[usbcb.rcvbufp] = '\0';
								analyze_command(usbcb.rcvbuf);
							}
							usbcb.rcvbufp = 0;
						}else if( key == 0x8 ){ // BS
							if( usbcb.rcvbufp > 0 ){
								usbcb.rcvbufp--;
							}
							usbEchoBack(key); // echo back;
						} else {
							if( !iscntrl(key) || key == 0x09 ){ // cr/lf/bs以外の制御文字は無視 ,HTは許可
								usbcb.rcvbuf[usbcb.rcvbufp++] = key;
								usbEchoBack(key); // echo back;
							}
						}
					}
					break;
				case USBMSG_SRC_MONITOR:
					char c;	
					c = USBMSG_GET_CHAR(rcvdata);
					usb_putchar(c);
					break;
			}

        } else {

            if (usbcb.usbtxbufp) {
                CDC_Transmit_FS((uint8_t*)usbcb.usbtxbuf, usbcb.usbtxbufp);
                usbcb.usbtxbufp = 0;
            }

            if (usbcb.rcvtimeout < (UI_TIMEOUT/USB_RCV_TIMEOUT)) {
                usbcb.rcvtimeout++;
            }
        }
    }
}


/// @brief
/// @param c :push character to usb transmit buffer
/// @return 1:success 0:buffer full
static int usb_putchar( char c )
{
  int ret;
  if( usbcb.usbtxbufp < USB_RCV_BUFSIZE){
    usbcb.usbtxbuf[usbcb.usbtxbufp++] = c;
  	ret = 1;
  }else{
    ret = 0;
  }
  if( (usbcb.usbtxbufp >= USB_RCV_BUFSIZE) || (c == 0x00)){
    CDC_Transmit_FS((uint8_t*)usbcb.usbtxbuf, usbcb.usbtxbufp);
	usbcb.usbtxbufp = 0; // buffer full, reset buffer
	osDelay(2); // wait for buffer flush
  }

  return ret;
}



/// @brief
/// @param str
/// @return
static int usb_puts( const char *str )
{
  int ret = 1;
  while(*str){
	ret = usb_putchar(*str);
		if(ret != 1 ){
		break; // buffer full
	}
	str++;
  }
  if( ret == 1 )ret = usb_putchar(0x0D); // CR
  if( ret == 1 )ret = usb_putchar(0x0A); // LF
  return ret;
}

/// @brief 入力文字列を単語ごとに分割し、各単語の先頭ポインタと長さをusbcbに格納する
/// @param command_strings 入力文字列（\0終端）
/// @return 取得した単語数
int parse_input_words(char *command_strings)
{
    int word_count = 0;
    char *p = command_strings;

    while (*p != '\0' && word_count < INPUT_WORD_MAX) {
        // 空白・制御文字をスキップ
        while (*p != '\0' && (isspace((int)*p) || iscntrl((int)*p))) {
            p++;
        }
        if (*p == '\0') break;

        // 単語の先頭を記録
        usbcb.word_top_ptr[word_count] = p;

        // 単語の長さをカウント
        int len = 0;
        while (p[len] != '\0' && !isspace((int)p[len]) && !iscntrl((int)p[len])) {
            len++;
        }
        usbcb.word_len[word_count] = len;
        word_count++;

        // 次の単語へ
        p += len;
    }
    usbcb.word_num = word_count;
    return word_count;
}

typedef enum{
  KWD_NONE = 0,
  KWD_VERSION,
  KWD_HELP,
  KWD_RESET,
  KWD_MODE,
  KWD_SET,
  KWD_GET,
  KWD_STATUS,
  KWD_POWER,
  KWD_MON,
  KWD_MAX
} E_KEYWORD;

typedef struct{
  E_KEYWORD kwd_no;
  const char *kwd_str;
  const char *cmd_help;
}T_KEYWORD;

const T_KEYWORD t_command[]={
	{KWD_VERSION, "version", "Show Version"},
	{KWD_HELP, "help", "Show Help"},
	{KWD_HELP, "?", "Show Help"},
	{KWD_RESET, "reset", "Reset System"},
	{KWD_SET, "set", "Set Parameter"},
	{KWD_GET, "get", "Get Parameter"},
	{KWD_MODE, "mode", "Set Mode"},
	{KWD_POWER,"power","Drive Relay ON"},
	{KWD_STATUS, "status", "Show Status"},
	{KWD_MON, "mon", "Monitor Mode"},
	{KWD_MAX, "", ""}
};

static int copy_word_to_buf( uint16_t index, char *out, size_t out_size )
{
	uint16_t len;

	if( (out == NULL) || (out_size == 0) ){
		return 0;
	}
	if( index >= usbcb.word_num ){
		return 0;
	}

	len = usbcb.word_len[index];
	if( (len == 0) || (len >= out_size) ){
		return 0;
	}

	memcpy(out, usbcb.word_top_ptr[index], len);
	out[len] = '\0';
	return 1;
}

static bool parse_u32_token( const char *token, uint32_t *out )
{
	uint32_t value = 0;

	if( (token == NULL) || (out == NULL) || (*token == '\0') ){
		return false;
	}

	for( ; *token != '\0'; token++ ){
		if( !isdigit((int)(unsigned char)*token) ){
			return false;
		}
		value = (value * 10U) + (uint32_t)(*token - '0');
	}

	*out = value;
	return true;
}

static bool parse_u8_token( const char *token, uint8_t *out )
{
	uint32_t value;

	if( !parse_u32_token(token, &value) ){
		return false;
	}
	if( value > 255U ){
		return false;
	}
	*out = (uint8_t)value;
	return true;
}

static bool parse_u16_token( const char *token, uint16_t *out )
{
	uint32_t value;

	if( !parse_u32_token(token, &value) ){
		return false;
	}
	if( value > 65535U ){
		return false;
	}
	*out = (uint16_t)value;
	return true;
}

static bool parse_ipv4_token( const char *token, uint8_t ip[4] )
{
	uint32_t value = 0;
	int part = 0;
	bool has_digit = false;

	if( (token == NULL) || (*token == '\0') ){
		return false;
	}

	for( ; ; token++ ){
		char c = *token;
		if( isdigit((int)(unsigned char)c) ){
			has_digit = true;
			value = (value * 10U) + (uint32_t)(c - '0');
			if( value > 255U ){
				return false;
			}
			continue;
		}

		if( (c == '.') || (c == '\0') ){
			if( !has_digit || (part >= 4) ){
				return false;
			}
			ip[part++] = (uint8_t)value;
			value = 0;
			has_digit = false;
			if( c == '\0' ){
				break;
			}
			continue;
		}

		return false;
	}

	return (part == 4);
}

static bool is_valid_float_token( const char *token )
{
	bool has_digit = false;
	int dot_count = 0;

	if( (token == NULL) || (*token == '\0') ){
		return false;
	}

	if( (*token == '+') || (*token == '-') ){
		token++;
		if( *token == '\0' ){
			return false;
		}
	}

	for( ; *token != '\0'; token++ ){
		if( isdigit((int)(unsigned char)*token) ){
			has_digit = true;
			continue;
		}
		if( *token == '.' ){
			dot_count++;
			if( dot_count > 1 ){
				return false;
			}
			continue;
		}
		return false;
	}

	return has_digit;
}

static bool parse_float_token( const char *token, float *out )
{
	char work[32];
	size_t len;

	if( (out == NULL) || !is_valid_float_token(token) ){
		return false;
	}

	len = strlen(token);
	if( len >= sizeof(work) ){
		return false;
	}

	memcpy(work, token, len + 1U);
	*out = Aatof(work);

	return true;
}

static void show_setup_param_help( void )
{
	usb_puts("Usage: set <param> <value>");
	usb_puts("Usage: get <param> | get all");
	usb_puts("params:");
	usb_puts("modbus_slave_address rs485_baudrate rs485_stop_bit rs485_parity rs485_bit_length response_delay_ms");
	usb_puts("leakage_low_cut ac_phase_wire ct_type1 ct_type2 ct_type3 ct_type4 avarage_count");
	usb_puts("volt_calib1_gain volt_calib1_offset volt_calib2_gain volt_calib2_offset");
	usb_puts("leakage_calib1_gain leakage_calib1_offset ... leakage_calib4_gain leakage_calib4_offset");
}

static void print_setup_param( const char *param )
{
	char str[96];

	if( strcmp(param, PSTR_MODBUS_SLAVE_ADDRESS) == 0 ){
		snprintf(str, sizeof(str), "%s:%u", PSTR_MODBUS_SLAVE_ADDRESS, g_setup.modbus_slave_address);
	} else if( strcmp(param, PSTR_RS485_BAUDRATE) == 0 ){
		snprintf(str, sizeof(str), "%s:%u", PSTR_RS485_BAUDRATE, g_setup.rs485_baudrate);
	} else if( strcmp(param, PSTR_RS485_STOP_BIT) == 0 ){
		snprintf(str, sizeof(str), "%s:%u", PSTR_RS485_STOP_BIT, g_setup.rs485_stop_bit);
	} else if( strcmp(param, PSTR_RS485_PARITY) == 0 ){
		snprintf(str, sizeof(str), "%s:%u", PSTR_RS485_PARITY, g_setup.rs485_parity);
	} else if( strcmp(param, PSTR_RS485_BIT_LENGTH) == 0 ){
		snprintf(str, sizeof(str), "%s:%u", PSTR_RS485_BIT_LENGTH, g_setup.rs485_bit_length);
	} else if( strcmp(param, PSTR_RESPONSE_DELAY_MS) == 0 ){
		snprintf(str, sizeof(str), "%s:%u", PSTR_RESPONSE_DELAY_MS, g_setup.response_delay_ms);
	} else if( strcmp(param, PSTR_LEAKAGE_LOW_CUT) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_LEAKAGE_LOW_CUT, (double)g_setup.leakage_low_cut);
	} else if( strcmp(param, PSTR_AC_PHASE_WIRE) == 0 ){
		snprintf(str, sizeof(str), "%s:%u", PSTR_AC_PHASE_WIRE, g_setup.ac_phase_wire);
	} else if( strcmp(param, PSTR_CT_TYPE1) == 0 ){
		snprintf(str, sizeof(str), "%s:%u", PSTR_CT_TYPE1, g_setup.ct_type[0]);
	} else if( strcmp(param, PSTR_CT_TYPE2) == 0 ){
		snprintf(str, sizeof(str), "%s:%u", PSTR_CT_TYPE2, g_setup.ct_type[1]);
	} else if( strcmp(param, PSTR_CT_TYPE3) == 0 ){
		snprintf(str, sizeof(str), "%s:%u", PSTR_CT_TYPE3, g_setup.ct_type[2]);
	} else if( strcmp(param, PSTR_CT_TYPE4) == 0 ){
		snprintf(str, sizeof(str), "%s:%u", PSTR_CT_TYPE4, g_setup.ct_type[3]);
	} else if( strcmp(param, PSTR_AVARAGE_COUNT) == 0 ){
		snprintf(str, sizeof(str), "%s:%u", PSTR_AVARAGE_COUNT, g_setup.avarage_count);
	} else if( strcmp(param, PSTR_VOLT_CALIB1_GAIN) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_VOLT_CALIB1_GAIN, (double)g_setup.volt_calib[0].gain);
	} else if( strcmp(param, PSTR_VOLT_CALIB1_OFFSET) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_VOLT_CALIB1_OFFSET, (double)g_setup.volt_calib[0].offset);
	} else if( strcmp(param, PSTR_VOLT_CALIB2_GAIN) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_VOLT_CALIB2_GAIN, (double)g_setup.volt_calib[1].gain);
	} else if( strcmp(param, PSTR_VOLT_CALIB2_OFFSET) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_VOLT_CALIB2_OFFSET, (double)g_setup.volt_calib[1].offset);
	} else if( strcmp(param, PSTR_VOLT_CALIB3_GAIN) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_VOLT_CALIB3_GAIN, (double)g_setup.volt_calib[2].gain);
	} else if( strcmp(param, PSTR_VOLT_CALIB3_OFFSET) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_VOLT_CALIB3_OFFSET, (double)g_setup.volt_calib[2].offset);
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB1_GAIN) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_LEAKAGE_CALIB1_GAIN, (double)g_setup.leakage_calib[0].gain);
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB1_OFFSET) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_LEAKAGE_CALIB1_OFFSET, (double)g_setup.leakage_calib[0].offset);
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB2_GAIN) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_LEAKAGE_CALIB2_GAIN, (double)g_setup.leakage_calib[1].gain);
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB2_OFFSET) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_LEAKAGE_CALIB2_OFFSET, (double)g_setup.leakage_calib[1].offset);
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB3_GAIN) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_LEAKAGE_CALIB3_GAIN, (double)g_setup.leakage_calib[2].gain);
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB3_OFFSET) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_LEAKAGE_CALIB3_OFFSET, (double)g_setup.leakage_calib[2].offset);
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB4_GAIN) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_LEAKAGE_CALIB4_GAIN, (double)g_setup.leakage_calib[3].gain);
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB4_OFFSET) == 0 ){
		snprintf(str, sizeof(str), "%s:%.6g", PSTR_LEAKAGE_CALIB4_OFFSET, (double)g_setup.leakage_calib[3].offset);
	} else {
		snprintf(str, sizeof(str), "Unknown parameter:%s", param);
	}

	usb_puts(str);
}

static void print_all_setup_params( void )
{
	print_setup_param(PSTR_MODBUS_SLAVE_ADDRESS);
	print_setup_param(PSTR_RS485_BAUDRATE);
	print_setup_param(PSTR_RS485_STOP_BIT);
	print_setup_param(PSTR_RS485_PARITY);
	print_setup_param(PSTR_RS485_BIT_LENGTH);
	print_setup_param(PSTR_RESPONSE_DELAY_MS);
	print_setup_param(PSTR_LEAKAGE_LOW_CUT);
	print_setup_param(PSTR_AC_PHASE_WIRE);
	print_setup_param(PSTR_CT_TYPE1);
	print_setup_param(PSTR_CT_TYPE2);
	print_setup_param(PSTR_CT_TYPE3);
	print_setup_param(PSTR_CT_TYPE4);
	print_setup_param(PSTR_AVARAGE_COUNT);
	print_setup_param(PSTR_VOLT_CALIB1_GAIN);
	print_setup_param(PSTR_VOLT_CALIB1_OFFSET);
	print_setup_param(PSTR_VOLT_CALIB2_GAIN);
	print_setup_param(PSTR_VOLT_CALIB2_OFFSET);
	print_setup_param(PSTR_VOLT_CALIB3_GAIN);
	print_setup_param(PSTR_VOLT_CALIB3_OFFSET);
	print_setup_param(PSTR_LEAKAGE_CALIB1_GAIN);
	print_setup_param(PSTR_LEAKAGE_CALIB1_OFFSET);
	print_setup_param(PSTR_LEAKAGE_CALIB2_GAIN);
	print_setup_param(PSTR_LEAKAGE_CALIB2_OFFSET);
	print_setup_param(PSTR_LEAKAGE_CALIB3_GAIN);
	print_setup_param(PSTR_LEAKAGE_CALIB3_OFFSET);
	print_setup_param(PSTR_LEAKAGE_CALIB4_GAIN);
	print_setup_param(PSTR_LEAKAGE_CALIB4_OFFSET);
}

static int set_setup_param( const char *param, const char *value )
{
	char str[96];
	uint8_t u8v;
	uint16_t u16v;
	uint8_t ip[4];
	float fv;

	if( strcmp(param, PSTR_MODBUS_SLAVE_ADDRESS) == 0 ){
		if( !parse_u8_token(value, &u8v) ) return 0;
		g_setup.modbus_slave_address = u8v;
	} else if( strcmp(param, PSTR_RS485_BAUDRATE) == 0 ){
		if( !parse_u8_token(value, &u8v) ) return 0;
		g_setup.rs485_baudrate = u8v;
	} else if( strcmp(param, PSTR_RS485_STOP_BIT) == 0 ){
		if( !parse_u8_token(value, &u8v) ) return 0;
		g_setup.rs485_stop_bit = u8v;
	} else if( strcmp(param, PSTR_RS485_PARITY) == 0 ){
		if( !parse_u8_token(value, &u8v) ) return 0;
		g_setup.rs485_parity = u8v;
	} else if( strcmp(param, PSTR_RS485_BIT_LENGTH) == 0 ){
		if( !parse_u8_token(value, &u8v) ) return 0;
		g_setup.rs485_bit_length = u8v;
	} else if( strcmp(param, PSTR_RESPONSE_DELAY_MS) == 0 ){
		if( !parse_u16_token(value, &u16v) ) return 0;
		g_setup.response_delay_ms = u16v;
	} else if( strcmp(param, PSTR_LEAKAGE_LOW_CUT) == 0 ){
		if( !parse_float_token(value, &fv) ) return 0;
		g_setup.leakage_low_cut = fv;
	} else if( strcmp(param, PSTR_AC_PHASE_WIRE) == 0 ){
		if( !parse_u8_token(value, &u8v) ) return 0;
		g_setup.ac_phase_wire = u8v;
	} else if( strcmp(param, PSTR_CT_TYPE1) == 0 ){
		if( !parse_u8_token(value, &u8v) ) return 0;
		g_setup.ct_type[0] = u8v;
	} else if( strcmp(param, PSTR_CT_TYPE2) == 0 ){
		if( !parse_u8_token(value, &u8v) ) return 0;
		g_setup.ct_type[1] = u8v;
	} else if( strcmp(param, PSTR_CT_TYPE3) == 0 ){
		if( !parse_u8_token(value, &u8v) ) return 0;
		g_setup.ct_type[2] = u8v;
	} else if( strcmp(param, PSTR_CT_TYPE4) == 0 ){
		if( !parse_u8_token(value, &u8v) ) return 0;
		g_setup.ct_type[3] = u8v;
	} else if( strcmp(param, PSTR_AVARAGE_COUNT) == 0 ){
		if( !parse_u16_token(value, &u16v) ) return 0;
		g_setup.avarage_count = u16v;
	} else if( strcmp(param, PSTR_VOLT_CALIB1_GAIN) == 0 ){
		if( !parse_float_token(value, &fv) ) return 0;
		g_setup.volt_calib[0].gain = fv;
	} else if( strcmp(param, PSTR_VOLT_CALIB1_OFFSET) == 0 ){
		if( !parse_float_token(value, &fv) ) return 0;
		g_setup.volt_calib[0].offset = fv;
	} else if( strcmp(param, PSTR_VOLT_CALIB2_GAIN) == 0 ){
		if( !parse_float_token(value, &fv) ) return 0;
		g_setup.volt_calib[1].gain = fv;
	} else if( strcmp(param, PSTR_VOLT_CALIB2_OFFSET) == 0 ){
		if( !parse_float_token(value, &fv) ) return 0;
		g_setup.volt_calib[1].offset = fv;
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB1_GAIN) == 0 ){
		if( !parse_float_token(value, &fv) ) return 0;
		g_setup.leakage_calib[0].gain = fv;
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB1_OFFSET) == 0 ){
		if( !parse_float_token(value, &fv) ) return 0;
		g_setup.leakage_calib[0].offset = fv;
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB2_GAIN) == 0 ){
		if( !parse_float_token(value, &fv) ) return 0;
		g_setup.leakage_calib[1].gain = fv;
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB2_OFFSET) == 0 ){
		if( !parse_float_token(value, &fv) ) return 0;
		g_setup.leakage_calib[1].offset = fv;
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB3_GAIN) == 0 ){
		if( !parse_float_token(value, &fv) ) return 0;
		g_setup.leakage_calib[2].gain = fv;
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB3_OFFSET) == 0 ){
		if( !parse_float_token(value, &fv) ) return 0;
		g_setup.leakage_calib[2].offset = fv;
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB4_GAIN) == 0 ){
		if( !parse_float_token(value, &fv) ) return 0;
		g_setup.leakage_calib[3].gain = fv;
	} else if( strcmp(param, PSTR_LEAKAGE_CALIB4_OFFSET) == 0 ){
		if( !parse_float_token(value, &fv) ) return 0;
		g_setup.leakage_calib[3].offset = fv;
	} else {
		return 0;
	}

	g_sys.setup_update = 1;
	g_sys.setup_update_time = HAL_GetTick();

	snprintf(str, sizeof(str), "set ok: %s", param);
	usb_puts(str);
	print_setup_param(param);
	return 1;
}


void usb_debug_hex( char *str )
{
  char debugst[10];
  while(*str){
	usb_putchar(':');
	sprintf(debugst,"%02X", (unsigned char)*str);
	usb_putchar(debugst[0]);
	usb_putchar(debugst[1]);
	str++;
  }
  usb_puts ("<");
}

/// @brief コマンドを検索する。
/// @param buf command string
/// @param buf commans string length
/// @return
E_KEYWORD search_keyword( const T_KEYWORD *ptk,char *buf , uint16_t word_len)
{
  E_KEYWORD kwd_no = KWD_NONE;
  uint16_t cmd_len;
  for(int i = 0; ptk->kwd_no != KWD_MAX; i++,ptk++){
	cmd_len = strlen(ptk->kwd_str);
	if( cmd_len == word_len){
		if( strncmp(buf, ptk->kwd_str, strlen(ptk->kwd_str)) == 0 ){
			kwd_no = ptk->kwd_no;
			break;
		}
	}
  }
  return kwd_no;
}



char command_buf[USB_RCV_BUFSIZE];

/// @brief analyze and response command
/// @param buf  command line terminal by \0
/// @return
static int analyze_command( char *buf )
{
	int ret = 0;
	int word_num;
	E_KEYWORD kwd_no = KWD_NONE;
	word_num = parse_input_words(buf); // parse input words

  	if( word_num != 0 ){
		kwd_no = search_keyword(t_command ,usbcb.word_top_ptr[0],usbcb.word_len[0]); // search command number
		switch(kwd_no){
			case KWD_VERSION:
				usb_puts("Version: 1.0.0");
				break;
			case KWD_HELP:
				usb_puts("Available Commands:");
				for(int i = 0; t_command[i].kwd_no != KWD_MAX; i++){
					usb_puts(t_command[i].kwd_str);
//					usb_puts(t_command[i].cmd_help);
				}
				break;
			case KWD_RESET:
				usb_puts("System Reset Command Received.");
				ret = 1; // indicate reset command
				break;
			case KWD_SET:
				cmd_set();
				break;
			case KWD_GET:
				cmd_get();
				break;
			case KWD_MODE:
				cmd_mode(); // for now, just show current mode
				break;
			case KWD_POWER:
				break;
			case KWD_STATUS:
				cmd_status();
				break;
			case KWD_MON:
				g_sys.monz0_count = 360;
				break;
			default:
				usb_puts("Unknown Command.");
				usb_puts(buf);
				break;
		}
	}
/*
	bufp = 0;
	sscanf(buf, "%s", command_buf); // get command number
	strcpy(command_buf, "Command: ");


	strcpy(command_buf, "Command: ");
	bufp = strlen(command_buf); // start position
	while(bufp < USB_RCV_BUFSIZE && *buf && !isspace((int)*buf) && *buf != '\0'){
		command_buf[bufp++] = *buf++;
	}
	command_buf[bufp] = '\0'; // null terminate
	usb_puts(command_buf);
*/
	return ret;
}


static int cmd_set(  void )
{
	char param[40];
	char value[40];

	if( usbcb.word_num == 1 ){
		show_setup_param_help();
		return 0;
	}

	if( !copy_word_to_buf(1, param, sizeof(param)) ){
		usb_puts("Invalid parameter token.");
		return 0;
	}

	if( usbcb.word_num == 2 ){
		print_setup_param(param);
		return 0;
	}

	if( !copy_word_to_buf(2, value, sizeof(value)) ){
		usb_puts("Invalid value token.");
		return 0;
	}

	if( g_sys.mode == MODE_MEASURE ){
		usb_puts("Cannot set parameter in MEAS mode");
		return 0;
	}

	if( set_setup_param(param, value) == 0 ){
		usb_puts("Set failed. Check parameter name and value format.");
		show_setup_param_help();
	}

	return 0;
}

static int cmd_get(  void )
{
	char param[40];

	if( usbcb.word_num == 1 ){
		show_setup_param_help();
		return 0;
	}

	if( !copy_word_to_buf(1, param, sizeof(param)) ){
		usb_puts("Invalid parameter token.");
		return 0;
	}

	if( strcmp(param, "all") == 0 ){
		print_all_setup_params();
		return 0;
	}

	print_setup_param(param);
	return 0;
}

static int cmd_mode(  void )
{
	char str[40];
	char param[40];
	char value[40];
	uint32_t u32;
	if( usbcb.word_num == 1 ){ // mode command
		snprintf(str, sizeof(str), "Current Mode: %s", (g_sys.mode == MODE_MEASURE) ? "RUN" : "SETUP");
		usb_puts(str);
	}else if (usbcb.word_num == 2){
		if( !copy_word_to_buf(1, value, sizeof(value)) ){
			usb_puts("Invalid value token.");
			return 0;
		}
		if( !parse_u32_token(value, &u32) ){
			return 0;
		}
		switch( u32 ){
		case 0:
				g_sys.mode_next = MODE_MEASURE;
				usb_puts("** RUN **");
				break;
		case 1:
				g_sys.mode_next = MODE_SETUP;
				usb_puts("## SETUP MODE ##");

				break;
		}
	}
	return 0;
}

extern float sysvdda;

static int cmd_status(  void )
{
	char str[40];

	if( usbcb.word_num == 1 ){ // status command
		usb_puts("Status:");
		snprintf(str,sizeof(str), "Ver:%s",pVersionString);
		usb_puts(str);
		snprintf(str,sizeof(str), "dsw:0x%02X", GetDsw());
		usb_puts(str);
		snprintf(str,sizeof(str), "vdda:%5.2f V", sysvdda);
		usb_puts(str);
	}
	return 0;
}
