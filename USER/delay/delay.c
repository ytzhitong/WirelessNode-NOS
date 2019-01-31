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
 *	本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *	ALIENTEK Pandora STM32L475 IOT开发板
 *	使用SysTick的普通计数模式对延迟进行管理
 *	包括delay_us,delay_ms
 *	正点原子@ALIENTEK
 *	技术论坛:www.openedv.com
 *	创建日期:2015/6/10
 *	版本：V1.1
 *	版权所有，盗版必究。
 *	Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	修改说明
 *	******************************************************************************/


static u32 fac_us = 0;							//us延时倍乘数
//static u16 fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数



/**
 * @brief	初始化延迟函数,SYSTICK的时钟固定为AHB时钟
 *
 * @param   SYSCLK	系统时钟频率
 *
 * @return  void
 */
void delay_init(void)
{
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//选择外部时钟  HCLK
	
	fac_us=SystemCoreClock/1000000;				//不论是否使用OS,fac_us都需要使用
//  fac_ms=(u16)fac_us*1000;					//非OS下,代表每个ms需要的systick时钟数  
}


/**
 * @brief	延时微秒(us)函数
 *
 * @remark	nus:0~190887435(最大值即2^32/fac_us@fac_us=22.5)
 *
 * @param   nus		需要延时多少微秒
 *
 * @return  void
 */
void delay_us(u32 nus)
{
    u32 ticks;
    u32 told, tnow, tcnt = 0;
    u32 reload = SysTick->LOAD;				//LOAD的值
    ticks = nus * fac_us; 						//需要的节拍数
    told = SysTick->VAL;        				//刚进入时的计数器值

    while(1)
    {
        tnow = SysTick->VAL;

        if(tnow != told)
        {
            if(tnow < told)tcnt += told - tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
            else tcnt += reload - tnow + told;
			
            told = tnow;
            if(tcnt >= ticks)break;			//时间超过/等于要延迟的时间,则退出.
        }
    }
}


/**
 * @brief	延时毫秒(ms)函数
 *
 * @param   nms		需要延时多少毫秒
 *
 * @return  void
 */
void delay_ms(u16 nms)
{
    u32 i;

    for(i = 0; i < nms; i++) delay_us(1000);
}





