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

extern osMessageQId usbRcvQueue01Handle;
extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

static int usb_putchar( char c );
static int usb_puts( const char *str );
static int analyze_command( char *buf );
static uint16_t usbGetChar( uint32_t timeout );
static void usbUngetChar( uint16_t c);
static void usbEchoBack( char c);
static int cmd_set(  void );
static int cmd_status(  void );


#define USB_RCV_BUFSIZE  128
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
            if (USBMSG_GET_SRC(rcvdata) != USBMSG_SRC_USB) {
                continue;
            }
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

/// @brief
/// @param str
/// @return
static int usb_puts_k( const char *str )
{
  int ret = 1;
  usb_putchar('[');
  while(*str){
	ret = usb_putchar(*str);
		if(ret != 1 ){
		break; // buffer full
	}
	str++;
  }

  usb_putchar(']');
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
  KWD_SET,
  KWD_GET,
  KWD_STATUS,
  KWD_IP,
  KWD_PORT,
  KWD_POWER,
  KWD_CONNECT_TIMEOUT,
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
	{KWD_POWER,"power","Drive Relay ON"},
	{KWD_STATUS, "status", "Show Status"},
	{KWD_MAX, "", ""}
};
const T_KEYWORD t_set_param[] =
{
	{KWD_IP,"ip", "Set IP Address"},
	{KWD_PORT,"port", "Set Port Number"},
	{KWD_CONNECT_TIMEOUT,"timeout", "Set Connect Timeout"},
	{KWD_MAX, "", ""}
};


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
E_KEYWORD search_keyword( T_KEYWORD *ptk,char *buf , uint16_t word_len)
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
	int bufp;
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
				break;
			case KWD_GET:
				usb_puts("Get Command Received.");
				break;
			case KWD_POWER:
				break;
			case KWD_STATUS:
				cmd_status();
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
	E_KEYWORD kwd_no = KWD_NONE;
	char str[40];
	int scanf_result;
	int i32;
	int ip[4] = {0, 0, 0, 0};

	switch( usbcb.word_num ){
		case 1: // set command
			usb_puts("Set parameters");
			usb_puts("Ex set ip xx.xx.xx.xx");
			usb_puts("ip/port/timeout");
		break;
		case 2: // set command with parameter
			kwd_no = search_keyword(t_set_param ,usbcb.word_top_ptr[1],usbcb.word_len[1]); // search command number
			switch(kwd_no){
				case KWD_IP:
					snprintf(str,sizeof(str), "ip addr:%d.%d.%d.%d",g_setup.tcpDesconip[0], g_setup.tcpDesconip[1], g_setup.tcpDesconip[2], g_setup.tcpDesconip[3]);
					usb_puts(str);
					break;
				case KWD_PORT:
					snprintf(str,sizeof(str), "port:%d",g_setup.tcpDesconPort);
					usb_puts(str);
					break;
				case KWD_CONNECT_TIMEOUT:
					snprintf(str,sizeof(str), "TCP disconnect timeout:%d",g_setup.tcpDescon_silent_timeout);
					usb_puts(str);
					break;

				default:
					strncpy(str, usbcb.word_top_ptr[1], usbcb.word_len[1]);
					str[usbcb.word_len[1]] = '\0'; // null terminate
					usb_puts(str);
					return 0;
			}
			break;
		case 3:
			// set command with parameter and value
			kwd_no = search_keyword(t_set_param ,usbcb.word_top_ptr[1],usbcb.word_len[1]); // search command number
			switch(kwd_no){
				case KWD_IP:
				scanf_result =sscanf(usbcb.word_top_ptr[2], "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
					// sscanfでIPアドレスを分解
					if( scanf_result == 4 ){
						if( ip[0] < 0 || ip[0] > 255 ||
							ip[1] < 0 || ip[1] > 255 ||
							ip[2] < 0 || ip[2] > 255 ||
							ip[3] < 0 || ip[3] > 255 ){
							usb_puts("Invalid IP Address Format.");
						}else{
							g_setup.tcpDesconip[0] = (uint8_t)ip[0];
							g_setup.tcpDesconip[1] = (uint8_t)ip[1];
							g_setup.tcpDesconip[2] = (uint8_t)ip[2];
							g_setup.tcpDesconip[3] = (uint8_t)ip[3];
							g_sys.setup_update = 1; // indicate setup update
							snprintf(str,sizeof(str), "ip addr:%d.%d.%d.%d",g_setup.tcpDesconip[0], g_setup.tcpDesconip[1], g_setup.tcpDesconip[2], g_setup.tcpDesconip[3]);
							usb_puts(str);
						}
					}else{
						usb_puts("Invalid IP Address Format. Expected format: x.x.x.x");
					}
					break;
				case KWD_PORT:
					scanf_result = sscanf(usbcb.word_top_ptr[2], "%d ", &i32);
					if(  (scanf_result == 1 ) && (i32 < 65535 ) && (i32 > 1) ){
						g_setup.tcpDesconPort = (uint16_t)i32;
						g_sys.setup_update = 1; // indicate setup update
						snprintf(str,sizeof(str), "port:%d",g_setup.tcpDesconPort);
						usb_puts(str);
					}else{
						usb_puts("Invalid Port Number Format.");
					}
					break;
				case KWD_CONNECT_TIMEOUT:

					scanf_result = sscanf(usbcb.word_top_ptr[2], "%d ", &i32);
					if(  (scanf_result == 1 ) &&  (i32 >= 0) ){
						g_setup.tcpDescon_silent_timeout = i32;
						g_sys.setup_update = 1; // indicate setup update
						snprintf(str,sizeof(str), "TCP disconnect timeout:%ld",g_setup.tcpDescon_silent_timeout);
						usb_puts(str);
					}else{
						usb_puts("Invalid Connect Timeout Format.");
					}
					break;

				default:
					usb_puts("parameter: ip | port | timeout");
					return 0;
			}
			break;
		default:
			break;

	}
	return 0;
}

extern float sysvdda,systemp,sysvbat;

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
