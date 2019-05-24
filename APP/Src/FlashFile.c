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

  if(FlashStat_Temp.flag!=0x55) //flag!=0x55ֱ�ӷ���0
	{
		return 0;
	}	
	FlashTemp=(u8 *)&FlashStat_Temp;
	for(i=0;i<sizeof(_FlashStat)-1;i++)
	{
		checksum=checksum+FlashTemp[i];
	}
	if(checksum==FlashStat_Temp.checksum)//У��ͨ������2
	{
		return 2;
	}
	else                            //У��ʧ�ܷ���1
	{
		return 1;
	}
	
}

void ReadFlashStat_First(void)
{
	FlashStat_Cnt=0;
	while(FlashStat_Cnt<FlashStat_Num)    //  
	{
    if(ReadFlashStat(FlashStat_Cnt)==2) //У��ͨ����˵����ǰ�����Ľṹ��������ڱ�ʹ��
		{			
			break;
		}
		if(ReadFlashStat(FlashStat_Cnt)==1) //flagУ����ȷ��sumУ��ʧ�ܣ�˵���ýṹ��洢�����쳣��
		{			
			break;
		}		
		FlashStat_Cnt++;
	}
	if(FlashStat_Cnt==FlashStat_Num)
		FlashStat_Cnt=0;  //�����������ֵ����0
	  			
}

void WriteFlashStat(void)
{
	u16 i;
	u8 *FlashTemp;
	u32 secpos; //������ַ
	u16 secoff; //����ƫ����
	u16 secremain;//����ʣ��ռ�
	
	u8 FlashBuff[sizeof(_FlashStat)];
	
	FlashStat_Temp.flag=0x00;
	W25QXX_Write_NoCheck((u8*)&FlashStat_Temp,FlashStat_BaseAddr+FlashStat_Cnt*sizeof(_FlashStat),sizeof(_FlashStat));
	
	FlashStat_Cnt++;
	if(FlashStat_Cnt==FlashStat_Num)
		FlashStat_Cnt=0;  //�����������ֵ����0
	
	secpos=FlashStat_Cnt*sizeof(_FlashStat)/4096;
	secoff=FlashStat_Cnt*sizeof(_FlashStat)%4096;
	secremain=4096-secoff;
	
	W25QXX_Read((u8 *)FlashBuff,FlashStat_BaseAddr+FlashStat_Cnt*sizeof(_FlashStat),sizeof(_FlashStat));
  for(i=0;i<sizeof(_FlashStat);i++)
	{
		if(FlashBuff[i]!=0xff)
		{
			W25QXX_Erase_Sector(secpos);		//�����������
			if(secremain<sizeof(_FlashStat))
			{
				W25QXX_Erase_Sector(secpos+1);		//�����¸�����
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

  if(FlashStat_Temp.flag!=0x55) //flag!=0x55ֱ�ӷ���0
	{
		return 0;
	}	
	FlashTemp=(u8 *)&FlashStat_Temp;
	for(i=0;i<sizeof(_FlashStat)-1;i++)
	{
		checksum=checksum+FlashTemp[i];
	}
	if(checksum==FlashStat_Temp.checksum)//У��ͨ������2
	{
		return 2;
	}
	else                            //У��ʧ�ܷ���1
	{
		return 1;
	}	
}
