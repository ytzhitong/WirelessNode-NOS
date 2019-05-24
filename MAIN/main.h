
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"


#define WIFI		  1		//�����Ƿ���wifi,����ΪLORA
#define LPOWER		0		//�����Ƿ��ǵ͹���
#define DEBUG		  0		//�����Ƿ��ǵ���ģʽ

#define WIFI_SEND_REPE 14

#if LPOWER
 #define LORA_SEND_REPE 2
#else
 #define LORA_SEND_REPE 14
#endif
typedef enum
{
	LORA_TYPE = 0,
	WIFI_TYPE = 1,
} _WIRELESS_TYPE;

 

void RST_WIFI(void);

#endif /* __MAIN_H */
