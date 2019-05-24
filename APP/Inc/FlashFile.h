#ifndef __FlashFile_H
#define __FlashFile_H
#include "sys.h"

#include "pcf8563.h"

typedef struct _FlashData_
{
	u8 flag;
	
	_PCF8563_Time_Typedef PCF8563_Time;//时间
	_PCF8563_Date_Typedef PCF8563_Date;//日期
		
	float temp; //温度
	float hum;  //湿度
	float press; //大气压
	float uv;    //紫外线系数
	float lux;   //光照强度
	
	u8 checksum;
	
} _FlashData;

typedef struct _FlashStat_
{
	u8 flag;
	
  u32 StoreAmount;
	
	u8 checksum;
	
} _FlashStat;


#define FlashStat_BaseAddr 0
#define FlashStat_Num      1000

#define FlashData_BaseAddr (1000*sizeof(_FlashStat)/1024)+1024 //
#define FlashData_Num      10000

void ReadFlashStat_First(void);
void WriteFlashStat(void);
void ReadFlashStat_First(void);

#endif



