#include "rtc.h"
#include "delay.h"
#include "usart.h"

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
 *	RTC驱动代码
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

RTC_HandleTypeDef RTC_Handler;  //RTC句柄

void system_power_config(void);

/**
 * @brief	RTC时间设置函数
 *
 * @param   hour	小时
 * @param   min		分钟
 * @param   sec		秒钟
 * @param   ampm	@RTC_AM_PM_Definitions:RTC_HOURFORMAT12_AM/RTC_HOURFORMAT12_PM
 *
 * @return  HAL_StatusTypeDef	SUCEE(1),成功
 *								ERROR(0),进入初始化模式失败
 */
HAL_StatusTypeDef RTC_Set_Time(u8 hour, u8 min, u8 sec, u8 ampm)
{
    RTC_TimeTypeDef RTC_TimeStructure;

    RTC_TimeStructure.Hours = hour;
    RTC_TimeStructure.Minutes = min;
    RTC_TimeStructure.Seconds = sec;
    RTC_TimeStructure.TimeFormat = ampm;
    RTC_TimeStructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    RTC_TimeStructure.StoreOperation = RTC_STOREOPERATION_RESET;
    return HAL_RTC_SetTime(&RTC_Handler, &RTC_TimeStructure, RTC_FORMAT_BIN);
}
/**
 * @brief	RTC日期设置函数
 *
 * @param   year	年
 * @param   month	月
 * @param   date	日
 * @param   week	星期(1~7,0,非法!)
 *
 * @return  HAL_StatusTypeDef	SUCEE(1),成功
 *								ERROR(0),进入初始化模式失败
 */
HAL_StatusTypeDef RTC_Set_Date(u8 year, u8 month, u8 date, u8 week)
{
    RTC_DateTypeDef RTC_DateStructure;

    RTC_DateStructure.Date = date;
    RTC_DateStructure.Month = month;
    RTC_DateStructure.WeekDay = week;
    RTC_DateStructure.Year = year;
    return HAL_RTC_SetDate(&RTC_Handler, &RTC_DateStructure, RTC_FORMAT_BIN);
}
/**
 * @brief	RTC初始化
 *
 * @param   void
 *
 * @return  u8		0,初始化成功;
 *					2,进入初始化模式失败;
 */
u8 RTC_Init(void)
{
    RTC_Handler.Instance = RTC;
    RTC_Handler.Init.HourFormat = RTC_HOURFORMAT_24; //RTC设置为24小时格式
    RTC_Handler.Init.AsynchPrediv = 0X7F;         //RTC异步分频系数(1~0X7F)
    RTC_Handler.Init.SynchPrediv = 0XFF;          //RTC同步分频系数(0~7FFF)
    RTC_Handler.Init.OutPut = RTC_OUTPUT_DISABLE;
    RTC_Handler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTC_Handler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if(HAL_RTC_Init(&RTC_Handler) != HAL_OK) return 2;

    if(HAL_RTCEx_BKUPRead(&RTC_Handler, RTC_BKP_DR0) != 0x32F2) //是否第一次配置
    {
        RTC_Set_Time(10, 00, 00, RTC_HOURFORMAT12_PM);	     //设置时间 ,根据实际时间修改
        RTC_Set_Date(19, 1, 29, 2);		                 //设置日期
        HAL_RTCEx_BKUPWrite(&RTC_Handler, RTC_BKP_DR0, 0x32F2); //标记已经初始化过了
    }

    return 0;
}

/**
 * @brief	RTC底层驱动，时钟配置，此函数会被HAL_RTC_Init()调用
 *
 * @param   hrtc	RTC句柄
 *
 * @return  void
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();//使能电源时钟PWR
    HAL_PWR_EnableBkUpAccess();//取消备份区域写保护

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE; //LSE配置
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;                //RTC使用LSE
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC; //外设为RTC
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE; //RTC时钟源为LSE
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    __HAL_RCC_RTC_ENABLE();//RTC时钟使能
}

/**
 * @brief	设置闹钟时间(按星期闹铃,24小时制)
 *
 * @param   week	星期几(1~7) @ref  RTC_WeekDay_Definitions
 * @param   hour	小时
 * @param   min		分钟
 * @param   sec		秒钟
 *
 * @return  void
 */
void RTC_Set_AlarmA(u8 week, u8 hour, u8 min, u8 sec)
{
    RTC_AlarmTypeDef RTC_AlarmSturuct;

    RTC_AlarmSturuct.AlarmTime.Hours = hour; //小时
    RTC_AlarmSturuct.AlarmTime.Minutes = min; //分钟
    RTC_AlarmSturuct.AlarmTime.Seconds = sec; //秒
    RTC_AlarmSturuct.AlarmTime.SubSeconds = 0;
    RTC_AlarmSturuct.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;

    RTC_AlarmSturuct.AlarmMask = RTC_ALARMMASK_NONE; //精确匹配星期，时分秒
    RTC_AlarmSturuct.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    RTC_AlarmSturuct.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY; //按星期
    RTC_AlarmSturuct.AlarmDateWeekDay = week; //星期
    RTC_AlarmSturuct.Alarm = RTC_ALARM_A;   //闹钟A
    HAL_RTC_SetAlarm_IT(&RTC_Handler, &RTC_AlarmSturuct, RTC_FORMAT_BIN);

    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0x01, 0x02); //抢占优先级1,子优先级2
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}
/**
 * @brief	周期性唤醒定时器设置
 *
 * @param   wksel	@ref RTCEx_Wakeup_Timer_Definitions
 *				#define RTC_WAKEUPCLOCK_RTCCLK_DIV16        ((uint32_t)0x00000000)
 *				#define RTC_WAKEUPCLOCK_RTCCLK_DIV8         ((uint32_t)0x00000001)
 *				#define RTC_WAKEUPCLOCK_RTCCLK_DIV4         ((uint32_t)0x00000002)
 *				#define RTC_WAKEUPCLOCK_RTCCLK_DIV2         ((uint32_t)0x00000003)
 *				#define RTC_WAKEUPCLOCK_CK_SPRE_16BITS      ((uint32_t)0x00000004)
 *				#define RTC_WAKEUPCLOCK_CK_SPRE_17BITS      ((uint32_t)0x00000006)
 * @param   cnt		自动重装载值.减到0,产生中断
 *
 * @return  void
 */
//
void RTC_Set_WakeUp(u32 wksel, u16 cnt)
{
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTC_Handler, RTC_FLAG_WUTF);//清除RTC WAKE UP的标志

    /*
    特殊说明：由于本例程使用的是最新HAL库(V1.9.0 / 27-July-2018)，该版本中此函数底层没有开启WakeUpTimer中断
    		自己手动移植程序的时候需要注意修改HAL底层函数，此处已经进行了修改！！！
    */
    HAL_RTCEx_SetWakeUpTimer_IT(&RTC_Handler, cnt, wksel);        //设置重装载值和时钟

    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x02, 0x02); //抢占优先级1,子优先级2
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}
/**
 * @brief	RTC闹钟中断服务函数
 *
 * @param   void
 *
 * @return  void
 */
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&RTC_Handler);
}

/**
 * @brief	RTC闹钟A中断处理回调函数
 *
 * @param   hrtc	RTC句柄
 *
 * @return  void
 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
//    printf("ALARM A!\r\n");
//	BEEP(1);
}

/**
 * @brief	RTC WAKE UP中断服务函数
 *
 * @param   void
 *
 * @return  void
 */
void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&RTC_Handler);
}

/**
 * @brief	RTC WAKE UP中断处理
 *
 * @param   hrtc	RTC句柄
 *
 * @return  void
 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
//    LED_B_TogglePin;	//每1s,翻转一次LED_B
}

//进入STANDBY模式低功耗，使用RTC功能唤醒，其中standbytime单位为S，如设置1，低功耗1秒后唤醒
void enter_standby_rtc(u16 standbytime)
{
//	uint32_t i;		//局部变量，用于计算低功耗时长
	system_power_config();
	
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
	{
		/* Clear Standby flag */
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB); 
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                  //清除Wake_UP标志
	}
  
    /* Disable Wakeup Counter */
    HAL_RTCEx_DeactivateWakeUpTimer(&RTC_Handler);
	
	/*To configure the wake up timer to 4s the WakeUpCounter is set to 0x242B:
	RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
	Wakeup Time Base = 16 /(~37KHz) = ~0,432 ms
	Wakeup Time = ~5s = 0,432ms  * WakeUpCounter
	==> WakeUpCounter = ~5s/0,432ms = 11562 */
//	i = standbytime*2048;
    HAL_RTCEx_SetWakeUpTimer_IT(&RTC_Handler, standbytime*2048, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    
	/* Clear all related wakeup flags */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	 /* Enter the Standby mode */
	HAL_PWR_EnterSTANDBYMode();
}

//进入STOP模式低功耗，使用RTC功能唤醒，其中stoptime单位为S，如设置1，低功耗1秒后唤醒
void enter_stop_rtc(float stoptime)
{
	uint32_t i;		//局部变量，用于计算低功耗时长
	system_power_config();
	
    /* Disable Wakeup Counter */
    HAL_RTCEx_DeactivateWakeUpTimer(&RTC_Handler);
	
	/*To configure the wake up timer to 4s the WakeUpCounter is set to 0x242B:
	RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
	Wakeup Time Base = 16 /(~37KHz) = ~0,432 ms
	Wakeup Time = ~5s = 0,432ms  * WakeUpCounter
	==> WakeUpCounter = ~5s/0,432ms = 11562 */
	i = stoptime*2396;
    HAL_RTCEx_SetWakeUpTimer_IT(&RTC_Handler, i, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    
    /* Enter Stop Mode */
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	SystemClock_Config();
}

//进入SLEEP模式低功耗，使用RTC功能唤醒，其中sleeptime单位为S，如设置1，低功耗1秒后唤醒
void enter_sleep_rtc(float sleeptime)
{
	uint32_t i;		//局部变量，用于计算低功耗时长
	

	system_power_config();
	
    /* Disable Wakeup Counter */
    HAL_RTCEx_DeactivateWakeUpTimer(&RTC_Handler);
	
	/*To configure the wake up timer to 4s the WakeUpCounter is set to 0x242B:
	RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
	Wakeup Time Base = 16 /(~37KHz) = ~0,432 ms
	Wakeup Time = ~5s = 0,432ms  * WakeUpCounter
	==> WakeUpCounter = ~5s/0,432ms = 11562 */
	i = sleeptime*2396;
    HAL_RTCEx_SetWakeUpTimer_IT(&RTC_Handler, i, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    
    /*Suspend Tick increment to prevent wakeup by Systick interrupt. 
    Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base)*/
    HAL_SuspendTick();

    /* Enter Sleep Mode , wake up is done once Key push button is pressed */
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

    /* Resume Tick interrupt if disabled prior to sleep mode entry*/
    HAL_ResumeTick();
}

//低功耗关闭项
void system_power_config(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure = {0};

//	/* Enable Power Control clock */
//	__HAL_RCC_PWR_CLK_ENABLE();

	/* Enable Ultra low power mode */
//	HAL_PWREx_EnableUltraLowPower();

//	/* Enable the fast wake up from Ultra low power mode */
//	HAL_PWREx_EnableFastWakeUp();

//	/* Enable GPIOs clock */
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	__HAL_RCC_GPIOB_CLK_ENABLE();
//	__HAL_RCC_GPIOC_CLK_ENABLE();
//	__HAL_RCC_GPIOD_CLK_ENABLE();
//	__HAL_RCC_GPIOH_CLK_ENABLE();
//	__HAL_RCC_GPIOE_CLK_ENABLE();

//	/* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
//	/* Note: Debug using ST-Link is not possible during the execution of this   */
//	/*       example because communication between ST-link and the device       */
//	/*       under test is done through UART. All GPIO pins are disabled (set   */
//	/*       to analog input mode) including  UART I/O pins.           */
//	GPIO_InitStructure.Pin = GPIO_PIN_All;
//	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;

//	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); 
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
//	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
//	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
//	HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
//	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

//	/* Disable GPIOs clock */
//	__HAL_RCC_GPIOA_CLK_DISABLE();
//	__HAL_RCC_GPIOB_CLK_DISABLE();
//	__HAL_RCC_GPIOC_CLK_DISABLE();
//	__HAL_RCC_GPIOD_CLK_DISABLE();
//	__HAL_RCC_GPIOH_CLK_DISABLE();
//	__HAL_RCC_GPIOE_CLK_DISABLE();

//	/* Configure RTC */
//	RTC_Handler.Instance = RTC;
//	/* Configure RTC prescaler and RTC data registers as follow:
//	- Hour Format = Format 24
//	- Asynch Prediv = Value according to source clock
//	- Synch Prediv = Value according to source clock
//	- OutPut = Output Disable
//	- OutPutPolarity = High Polarity
//	- OutPutType = Open Drain */
//	RTC_Handler.Init.HourFormat = RTC_HOURFORMAT_24;
//	RTC_Handler.Init.AsynchPrediv = 0x7F;
//	RTC_Handler.Init.SynchPrediv = 0xFF;
//	RTC_Handler.Init.OutPut = RTC_OUTPUT_DISABLE;
//	RTC_Handler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
//	RTC_Handler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
//	if(HAL_RTC_Init(&RTC_Handler) != HAL_OK)
//	{
//		/* Initialization Error */
////		Error_Handler(); 
//	}
}




