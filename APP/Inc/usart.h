#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"


typedef struct _uart_rx_ {
 
	u8 rx_buf[128];
	u16 rx_cnt;
	u8 rx_flag;
	u8 con_flag;
	
	u16 check;
	u16 check2;
} UART_RX;



#endif


