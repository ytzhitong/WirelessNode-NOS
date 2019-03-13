#include "adc.h"
#include "delay.h"

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
 *	ADC驱动代码
 *	正点原子@ALIENTEK
 *	技术论坛:www.openedv.com
 *	创建日期:2018/10/27
 *	版本：V1.0
 *	版权所有，盗版必究。
 *	Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	初始版本
 *	******************************************************************************/

ADC_HandleTypeDef ADC1_Handler;//ADC句柄

/**
 * @brief	初始化ADC函数
 *
 * @param   void
 *
 * @return  void
 */
void MY_ADC_Init(void)
{
    ADC1_Handler.Instance = ADC1;
    ADC1_Handler.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4; //2分频，ADCCLK=SYSCLK/2=80/2=40MHZ	
    ADC1_Handler.Init.Resolution = ADC_RESOLUTION_12B;           //12位模式
    ADC1_Handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;           //右对齐
    ADC1_Handler.Init.ScanConvMode = DISABLE;                    //非扫描模式
    ADC1_Handler.Init.EOCSelection = DISABLE;                    //关闭EOC中断
    ADC1_Handler.Init.ContinuousConvMode = DISABLE;              //关闭连续转换
    ADC1_Handler.Init.NbrOfConversion = 1;                       //1个转换在规则序列中 也就是只转换规则序列1
    ADC1_Handler.Init.DiscontinuousConvMode = DISABLE;           //禁止不连续采样模式
    ADC1_Handler.Init.NbrOfDiscConversion = 0;                   //不连续采样通道数为0
    ADC1_Handler.Init.ExternalTrigConv = ADC_SOFTWARE_START;     //软件触发
    ADC1_Handler.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; //使用软件触发

    ADC1_Handler.Init.LowPowerAutoWait = DISABLE;
    ADC1_Handler.Init.DMAContinuousRequests = DISABLE;
//    ADC1_Handler.Init.Overrun = ADC_OVR_DATA_PRESERVED;
//    ADC1_Handler.Init.OversamplingMode = DISABLE;

    ADC1_Handler.Init.DMAContinuousRequests = DISABLE;           //关闭DMA请求
    HAL_ADC_Init(&ADC1_Handler);                                 //初始化
}
/**
 * @brief	ADC底层驱动，引脚配置，时钟使能，此函数会被HAL_ADC_Init()调用
 *
 * @param   hadc	ADC句柄
 *
 * @return  void
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
//    RCC_PeriphCLKInitTypeDef PeriphClkInit;
    GPIO_InitTypeDef GPIO_Initure;

//    //选择ADC时钟源为SYSCLK(80Mhz)
//    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
////    PeriphClkInit.AdcClockSelection = RCC_PERIPHCLK_LCD;
//    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    __HAL_RCC_ADC1_CLK_ENABLE();            //使能ADC1时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟

    GPIO_Initure.Pin = GPIO_PIN_4;          //PC2
    GPIO_Initure.Mode = GPIO_MODE_ANALOG;   //模拟
    GPIO_Initure.Pull = GPIO_NOPULL;        //不带上下拉
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);
}
/**
 * @brief	获得ADC值
 *
 * @param   ch		通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
 *
 * @return  u16		转换结果
 */
u16 Get_Adc(u32 ch)
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;

    ADC1_ChanConf.Channel = ch;                                 //通道
    ADC1_ChanConf.Rank = ADC_REGULAR_RANK_1;					//第1个序列，序列1
    ADC1_ChanConf.SamplingTime = ADC_SAMPLETIME_192CYCLES;    //采样时间
//    ADC1_ChanConf.SingleDiff = ADC_SINGLE_ENDED;				//ADC单端输入
//    ADC1_ChanConf.OffsetNumber = ADC_OFFSET_NONE;				//偏移号选择
//    ADC1_ChanConf.Offset = 0;
    HAL_ADC_ConfigChannel(&ADC1_Handler, &ADC1_ChanConf);       //通道配置

    HAL_ADC_Start(&ADC1_Handler);                               //开启ADC

    HAL_ADC_PollForConversion(&ADC1_Handler, 10);               //轮询转换

    return (u16)HAL_ADC_GetValue(&ADC1_Handler);	        	//返回最近一次ADC1规则组的转换结果
}

/**
 * @brief	获取指定通道的转换值，取times次,然后平均
 *
 * @param   ch		通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
 * @param   times	获取次数
 *
 * @return  u16		通道ch的times次转换结果平均值
 */
u16 Get_Adc_Average(u32 ch, u8 times)
{
    u32 temp_val = 0;
    u8 t;

    for(t = 0; t < times; t++)
    {
        temp_val += Get_Adc(ch);
        delay_ms(5);
    }

    return temp_val / times;
}
