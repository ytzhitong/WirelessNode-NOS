
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include "pcf8563.h"

typedef struct _StoreData_
{
	
	_PCF8563_Time_Typedef PCF8563_Time;//ʱ��
	_PCF8563_Date_Typedef PCF8563_Date;//����
		
	float temp; //�¶�
	float hum;  //ʪ��
	float press; //����ѹ
	float uv;    //������ϵ��
	float lux;   //����ǿ��
	
} _StoreData;

#endif /* __MAIN_H */
