
#include "sys.h"
#include "delay.h"
#include "main.h"
#include "MY_GPIO.h"
#include "SHT2x.h"
#include "uart5.h"
#include "wkup.h"
#include "BME280.h"
#include "string.h"
#include "rtc.h"
#include "timer.h"
#include "w25qxx.h" 
#include "SI1133.h" 

u8 LoraTxTemp[8];
int cnt=0;

float bmp280_temp;
float bmp280_press;
float bmp280_humi;
float high;

float uvi;
int32_t lux;

char uart_temp[60];
u8 TaskCnt=0;

u8 SI1133_OK=1;
u8 BME280_OK=0;
u8 SHT21_OK=1;

void TEST_task(void);
//author:ytzhitong

int main(void)
{	
	  HAL_Init();
    SystemClock_Config();	//初始化系统时钟
    delay_init(); 		    //初始化延时函数  
	
	  GPIO_Init();
	  uart5_init(115200);	
//	  WKUP_Init();
	  RTC_Init();					//初始化RTC
    
	  TIM3_Init(10 - 1, 1600 - 1);       	//定时器3初始化，定时器时钟为80M，分频系数为8000-1，
											//所以定时器3的频率为80M/8000=10K，自动重装载为5000-1，那么定时器周期就是500ms
    W25QXX_Init();			//W25QXX初始化
	  
	  SI1133_Init();
		bme280Init();
  	SHT2x_Init();
	  SHT2x_Test();

	
		while(1)
		{

//			LED_task();
			UART5_RX_task();
			TEST_task();
		}	

}

void WriteFlash(void)
{
      W25QXX_Write((u8*)uart_temp,W25Q128_CNTT*60+2,60);						
		  W25Q128_CNTT++;
			if(W25Q128_CNTT>60000)
			{
				W25Q128_CNTT=0;
			}
		  W25QXX_WriteCNT();	
}

void TEST_task(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;	
	float temp,hum;

  if(run.TEST_cnt>2000)
	{	
      run.TEST_cnt=0;
	    LED_TogglePin;

		if(SI1133_OK==1)
		{
  		getSensorData();		
		  uvi = Si1133_getUVReading(0, &samples);
      lux = Si1133_getLuxReading(0, &samples);
		}
		if(BME280_OK==1)
		{
      bme280GetData(&bmp280_press,&bmp280_temp,&bmp280_humi,&high);
			temp=bmp280_temp;
			hum=bmp280_humi;
		}
		if(SHT21_OK==1)
		{
		  SHT2x_Test();
			temp=g_sht2x_param.TEMP_HM;
			hum=g_sht2x_param.HUMI_HM;	
		}
	
			HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
			HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);

      sprintf((char*)uart_temp, "D20%02d%02d%02dT%02d:%02d:%02dT%04.1fH%04.1fP%07.2fL%06dU%05.2f#END\n", 
				       RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date,
			         RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds,
			         temp,hum,bmp280_press,
			         lux,uvi);	
      HAL_UART_Transmit(&UART5_Handler,(uint8_t *)uart_temp,strlen(uart_temp),1);
      if(TaskCnt<100)			
      TaskCnt++;			
      if((TaskCnt>2)&&(KEY_PIN_STATE()==0))	
			{
				WriteFlash();
//				delay_ms(100);
//				Sys_Enter_Standby();
				enter_standby_rtc(30);	
//        delay_ms(100);
//        enter_sleep_rtc(60);				
			}				
			
	}			
}




