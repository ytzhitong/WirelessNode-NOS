#include "FlashFile.h"
#include "w25qxx.h" 

u16 FlashStat_Cnt;

_FlashStat FlashStat_Temp,FlashStat;
_FlashData FlashData_Temp,FlashData_Write;

//
u8 ReadFlashStat(u16 cnt)
{
	u8 *FlashTemp;
	u8 checksum=0;
	u8 i;
	
  W25QXX_Read((u8 *)&FlashStat_Temp,FlashStat_BaseAddr+FlashStat_Cnt*sizeof(_FlashStat),sizeof(_FlashStat));

  if(FlashStat_Temp.flag!=0x55) //flag!=0x55直接返回0
	{
		return 0;
	}	
	FlashTemp=(u8 *)&FlashStat_Temp;
	for(i=0;i<sizeof(_FlashStat)-1;i++)
	{
		checksum=checksum+FlashTemp[i];
	}
	if(checksum==FlashStat_Temp.checksum)//校验通过返回2
	{
		return 2;
	}
	else                            //校验失败返回1
	{
		return 1;
	}
	
}

void ReadFlashStat_First(void)
{
	FlashStat_Cnt=0;
	while(FlashStat_Cnt<FlashStat_Num)    //  
	{
    if(ReadFlashStat(FlashStat_Cnt)==2) //校验通过，说明当前计数的结构体变量正在被使用
		{			
			break;
		}
		if(ReadFlashStat(FlashStat_Cnt)==1) //flag校验正确，sum校验失败，说明该结构体存储存在异常。
		{			
			break;
		}		
		FlashStat_Cnt++;
	}
	if(FlashStat_Cnt==FlashStat_Num)
		FlashStat_Cnt=0;  //超过计数最大值返回0
	  			
}

void WriteFlashStat(void)
{
	u16 i;
	u8 *FlashTemp;
	u32 secpos; //扇区地址
	u16 secoff; //扇区偏移量
	u16 secremain;//扇区剩余空间
	
	u8 FlashBuff[sizeof(_FlashStat)];
	
	FlashStat_Temp.flag=0x00;
	W25QXX_Write_NoCheck((u8*)&FlashStat_Temp,FlashStat_BaseAddr+FlashStat_Cnt*sizeof(_FlashStat),sizeof(_FlashStat));
	
	FlashStat_Cnt++;
	if(FlashStat_Cnt==FlashStat_Num)
		FlashStat_Cnt=0;  //超过计数最大值返回0
	
	secpos=FlashStat_Cnt*sizeof(_FlashStat)/4096;
	secoff=FlashStat_Cnt*sizeof(_FlashStat)%4096;
	secremain=4096-secoff;
	
	W25QXX_Read((u8 *)FlashBuff,FlashStat_BaseAddr+FlashStat_Cnt*sizeof(_FlashStat),sizeof(_FlashStat));
  for(i=0;i<sizeof(_FlashStat);i++)
	{
		if(FlashBuff[i]!=0xff)
		{
			W25QXX_Erase_Sector(secpos);		//擦除这个扇区
			if(secremain<sizeof(_FlashStat))
			{
				W25QXX_Erase_Sector(secpos+1);		//擦除下个扇区
			}
			break;
		}
	}

  FlashStat_Temp.flag=0x55;
	FlashStat_Temp.StoreAmount=FlashStat_Cnt;
	FlashStat_Temp.checksum=0;
	FlashTemp=(u8 *)&FlashStat_Temp;
	for(i=0;i<sizeof(_FlashStat)-1;i++)
	{
		FlashStat_Temp.checksum=FlashStat_Temp.checksum+FlashTemp[i];
	}
	W25QXX_Write_NoCheck((u8*)&FlashStat_Temp,FlashStat_BaseAddr+FlashStat_Cnt*sizeof(_FlashStat),sizeof(_FlashStat));		
}

u8 ReadFlashData(u32 cnt)
{
	u8 *FlashTemp;
	u8 checksum=0;
	u8 i;
	
  W25QXX_Read((u8 *)&FlashStat_Temp,FlashStat_BaseAddr+FlashStat_Cnt*sizeof(_FlashStat),sizeof(_FlashStat));

  if(FlashStat_Temp.flag!=0x55) //flag!=0x55直接返回0
	{
		return 0;
	}	
	FlashTemp=(u8 *)&FlashStat_Temp;
	for(i=0;i<sizeof(_FlashStat)-1;i++)
	{
		checksum=checksum+FlashTemp[i];
	}
	if(checksum==FlashStat_Temp.checksum)//校验通过返回2
	{
		return 2;
	}
	else                            //校验失败返回1
	{
		return 1;
	}	
}
