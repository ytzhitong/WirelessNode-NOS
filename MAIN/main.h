
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"
#include "pcf8563.h"

typedef struct _StoreData_
{
	
	_PCF8563_Time_Typedef PCF8563_Time;//时间
	_PCF8563_Date_Typedef PCF8563_Date;//日期
		
	float temp; //温度
	float hum;  //湿度
	float press; //大气压
	float uv;    //紫外线系数
	float lux;   //光照强度
	
} _StoreData;

#endif /* __MAIN_H */
