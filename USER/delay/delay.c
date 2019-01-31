#include "delay.h"
#include "sys.h"

/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 *	ALIENTEK Pandora STM32L475 IOT������
 *	ʹ��SysTick����ͨ����ģʽ���ӳٽ��й���
 *	����delay_us,delay_ms
 *	����ԭ��@ALIENTEK
 *	������̳:www.openedv.com
 *	��������:2015/6/10
 *	�汾��V1.1
 *	��Ȩ���У�����ؾ���
 *	Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	�޸�˵��
 *	******************************************************************************/


static u32 fac_us = 0;							//us��ʱ������
//static u16 fac_ms=0;							//ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��



/**
 * @brief	��ʼ���ӳٺ���,SYSTICK��ʱ�ӹ̶�ΪAHBʱ��
 *
 * @param   SYSCLK	ϵͳʱ��Ƶ��
 *
 * @return  void
 */
void delay_init(void)
{
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//ѡ���ⲿʱ��  HCLK
	
	fac_us=SystemCoreClock/1000000;				//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
//  fac_ms=(u16)fac_us*1000;					//��OS��,����ÿ��ms��Ҫ��systickʱ����  
}


/**
 * @brief	��ʱ΢��(us)����
 *
 * @remark	nus:0~190887435(���ֵ��2^32/fac_us@fac_us=22.5)
 *
 * @param   nus		��Ҫ��ʱ����΢��
 *
 * @return  void
 */
void delay_us(u32 nus)
{
    u32 ticks;
    u32 told, tnow, tcnt = 0;
    u32 reload = SysTick->LOAD;				//LOAD��ֵ
    ticks = nus * fac_us; 						//��Ҫ�Ľ�����
    told = SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ

    while(1)
    {
        tnow = SysTick->VAL;

        if(tnow != told)
        {
            if(tnow < told)tcnt += told - tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
            else tcnt += reload - tnow + told;
			
            told = tnow;
            if(tcnt >= ticks)break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
        }
    }
}


/**
 * @brief	��ʱ����(ms)����
 *
 * @param   nms		��Ҫ��ʱ���ٺ���
 *
 * @return  void
 */
void delay_ms(u16 nms)
{
    u32 i;

    for(i = 0; i < nms; i++) delay_us(1000);
}





