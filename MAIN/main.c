
#include "sys.h"
#include "delay.h"
#include "main.h"
#include "MY_GPIO.h"
#include "SHT2x.h"
#include "uart5.h"
#include "usart2.h"
#include "wkup.h"
#include "BME280.h"
#include "string.h"
#include "rtc.h"
#include "timer.h"
#include "w25qxx.h" 
#include "SI1133.h" 
#include "adc.h"
#include "MY_MATH.h"

u8 LoraTxTemp[8];
int cnt=0;

float bmp280_temp;
float bmp280_press;
float bmp280_humi;
float high;

float uvi;
float lux;

float temp,hum;

char uart_temp[60];
u8 TaskCnt=0;

u8 SI1133_OK=0;
u8 BME280_OK=0;
u8 SHT21_OK=0;

extern u16 pm25;

void TEST_task(void);
void DATAFRESH_task(void);
void SendLORA(void);
//author:ytzhitong

int main(void)
{	
	  HAL_Init();
    SystemClock_Config();	//初始化系统时钟
    delay_init(); 		    //初始化延时函数  
	  delay_ms(500);
	
	  GPIO_Init();
	  uart5_init(115200);
    uart2_init(9600);	
	  WKUP_Init();
//	  RTC_Init();					//初始化RTC
    
	  TIM3_Init(10 - 1, 1600 - 1);       	//定时器3初始化，定时器时钟为16M，分频系数为1600-1，
											//所以定时器3的频率为16M/1600=10K，自动重装载为10-1，那么定时器周期就是1ms
    W25QXX_Init();			//W25QXX初始化
	
	  SI1133_Init();
		bme280Init();
  	SHT2x_Init();
	
//	  MY_ADC_Init();				//初始化ADC1通道3
//    MH_CTL(1);
//		PM_SET(1);

		while(1)
		{

			LED_task();
			UART5_RX_task();
//			UART2_RX_task();
			DATAFRESH_task();
		}	

}

//void WriteFlash(void)
//{
//      W25QXX_Write((u8*)uart_temp,W25Q128_CNTT*60+2,60);						
//		  W25Q128_CNTT++;
//			if(W25Q128_CNTT>60000)
//			{
//				W25Q128_CNTT=0;
//			}
//		  W25QXX_WriteCNT();	
//}

void TEST_task(void)
{
//	RTC_TimeTypeDef RTC_TimeStruct;
//	RTC_DateTypeDef RTC_DateStruct;	
//	float temp,hum;
//	
//  u16 adcx;
//  float adcf;
//	u16 CO2_value;
//	
//  if(run.TEST_cnt>2000)
//	{	
//      run.TEST_cnt=0;
////	    LED_TogglePin;

//		if(SI1133_OK==1)
//		{
//  		getSensorData();		
//		  uvi = Si1133_getUVReading(0, &samples);
//      lux = Si1133_getLuxReading(0, &samples);
//		}
//		if(BME280_OK==1)
//		{
//      bme280GetData(&bmp280_press,&bmp280_temp,&bmp280_humi,&high);
//			temp=bmp280_temp;
//			hum=bmp280_humi;
//		}
//		if(SHT21_OK==1)
//		{
//		  SHT2x_Test();
//			temp=g_sht2x_param.TEMP_HM;
//			hum=g_sht2x_param.HUMI_HM;	
//		}
//	
//			HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
//			HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
//	
////			adcx= Get_Adc_Average(ADC_CHANNEL_14,3);
////			adcf = (float)adcx * (3.3 / 4096);
////		  CO2_value=(adcf-0.4)*5000/1.6;
//				
//      sprintf((char*)uart_temp, "D20%02d%02d%02dT%02d:%02d:%02dT%04.1fH%04.1fP%07.2fL%06dU%05.2fC%04dP%05d#END\n", 
//				       RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date,
//			         RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds,
//			         temp,hum,bmp280_press,
//			         lux,uvi,CO2_value,pm25);	
//      HAL_UART_Transmit(&UART5_Handler,(uint8_t *)uart_temp,strlen(uart_temp),1);
//			
//			
////      if(TaskCnt<100)			
////      TaskCnt++;			
////      if((TaskCnt>2)&&(KEY_PIN_STATE()==0))	
////			{
////				WriteFlash();
//////				delay_ms(100);
//////				Sys_Enter_Standby();
////				enter_standby_rtc(30);	
//////        delay_ms(100);
//////        enter_sleep_rtc(60);				
////			}				
//			
//	}			
}

void DATAFRESH_task(void)
{
	u8 SentCNT=0;
  if(run.TEST_cnt>2000)
	{	
    run.TEST_cnt=0;
    SentCNT++;
		
		if(SI1133_OK==1)
		{
  		getSensorData();	
			
		  uvi = Si1133_getUVReading(0, &samples);
      lux = Si1133_getLuxReading(0, &samples);
      
			#if DEBUG								
      sprintf((char*)uart_temp, "A%06d U%05.2f\n", 
			         lux,uvi);	
      HAL_UART_Transmit(&UART5_Handler,(uint8_t *)uart_temp,strlen(uart_temp),1);
      #endif			
		}
		if(BME280_OK==1)
		{
      bme280GetData(&bmp280_press,&bmp280_temp,&bmp280_humi,&high);
			temp=bmp280_temp;
			hum=bmp280_humi;
      
			#if DEBUG	
      sprintf((char*)uart_temp, "T%04.1f H%04.1f P%07.2f G%07.2f\n", 
			         temp,hum,bmp280_press,high);	
      HAL_UART_Transmit(&UART5_Handler,(uint8_t *)uart_temp,strlen(uart_temp),1);
			#endif	
		}
		if(SHT21_OK==1)
		{
		  SHT2x_Test();
			temp=g_sht2x_param.TEMP_HM;
			hum=g_sht2x_param.HUMI_HM;	
			
//			#if DEBUG
//      sprintf((char*)uart_temp, "T%04.1f H%04.1f\n", 
//			         temp,hum);	
//      HAL_UART_Transmit(&UART5_Handler,(uint8_t *)uart_temp,strlen(uart_temp),1);
//			#endif	
		}
//		if(SentCNT>1)
		{
			SendLORA();
			Sys_Enter_Standby();
		}	
  }
}

void SendLORA(void)
{
	LORA_WAKE(0);
	delay_ms(6);

  DecimalToHex(temp,uart_temp);
	DecimalToHex(hum ,uart_temp+4);
	DecimalToHex(bmp280_press ,uart_temp+8);
	DecimalToHex(uvi ,uart_temp+12);
	DecimalToHex(lux ,uart_temp+16);
	
	HAL_UART_Transmit(&UART5_Handler,(uint8_t *)uart_temp,20,1);
	
	LORA_WAKE(1);	
}


