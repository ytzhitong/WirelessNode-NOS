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
 *	系统时钟初始化
 *	包括时钟设置/中断管理/GPIO设置等
 *	正点原子@ALIENTEK
 *	技术论坛:www.openedv.com
 *	创建日期:2016/1/5
 *	版本：V1.0
 *	版权所有，盗版必究。
 *	Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	版本修改说明
 *	无
 *	******************************************************************************/

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

/**
 * @brief	时钟系统配置函数
 *
 * @remark	SYSCLK = HSE / PLLM * PLLN / PLLR
 * 			SYSCLK = 8M  /   1  *  20  /2		= 	80M
 *
 * @param   void
 *
 * @return  void
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
//  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
//  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
//  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
//  {
//    Error_Handler();
//  }
}

/**
 * @brief	THUMB指令不支持汇编内联、
 *			采用如下方法实现执行汇编指令WFI 
 *
 * @param   void
 *
 * @return  __asm
 */
__asm void WFI_SET(void)
{
	WFI;		  
}
/**
 * @brief	关闭所有中断(但是不包括fault和NMI中断)
 *
 * @param   void
 *
 * @return  __asm
 */
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
/**
 * @brief	开启所有中断
 *
 * @param   void
 *
 * @return  __asm
 */
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
/**
 * @brief	设置栈顶地址
 *
 * @param   addr	栈顶地址
 *
 * @return  __asm
 */
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}

