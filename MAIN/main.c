
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
#include <stdio.h>
#include "FlashFile.h"
#include "iwdg.h"

u8 LoraTxTemp[8];
int cnt=0;

float bmp280_temp;
float bmp280_press;
float bmp280_humi;
float high;

float uvi;
float lux;

float temp,hum;

char uart_temp[200];
u8 TaskCnt=0;
u8 SentCNT=0;

u8 SI1133_OK=0;
u8 BME280_OK=0;
u8 SHT21_OK=0;

extern u16 pm25;

float co2_value;
extern float HCHO_value;

void TEST_task(void);
void DATAFRESH_task(void);
void SendLORA(void);
void SendWIFI(void);
void IDOG_task(void);
void CO2_get(void);
//author:ytzhitong


int main(void)
{	
	
	  HAL_Init();
    SystemClock_Config();	//初始化系统时钟
    delay_init(); 		    //初始化延时函数  
	  delay_ms(500);
	
	  GPIO_Init();
		
	  uart5_init(74880);
		
    uart2_init(9600);

#if LPOWER	
	  WKUP_Init();
#endif
//	  RTC_Init();					//初始化RTC
    
	  TIM3_Init(10 - 1, 1600 - 1);       	//定时器3初始化，定时器时钟为16M，分频系数为1600-1，
											//所以定时器3的频率为16M/1600=10K，自动重装载为10-1，那么定时器周期就是1ms
    W25QXX_Init();			//W25QXX初始化
	
	  SI1133_Init();
		bme280Init();
  	SHT2x_Init();
	
	  ReadFlashStat_First();
	
	  MY_ADC_Init();				//初始化ADC1通道3
    MH_CTL(1);
		PM_SET(1);

    //IWDG_Init(IWDG_PRESCALER_64, 500);	//分频数为64,重载值为500,溢出时间为1s(4*2^4*500/32=1000ms)
		
		while(1)
		{

			LED_task();
			UART5_RX_task();
			UART2_RX_task();
			DATAFRESH_task();
			//IDOG_task();
		}	

}

void IDOG_task(void)
{
	if(run.IDOG_cnt>200)
	{
		run.IDOG_cnt=0;
		IWDG_Feed();
	}
}
void WriteFlash(void)
{
	
}

void DATAFRESH_task(void)
{
  if(run.TEST_cnt>2000)  //每2S取一次数据
	{	
    run.TEST_cnt=0;
    SentCNT++;
	
    CO2_get();
		
		if(SI1133_OK==1)
		{
  		getSensorData();	
			
		  uvi = Si1133_getUVReading(0, &samples);
      lux = Si1133_getLuxReading(0, &samples);
      
			#if DEBUG								
      sprintf((char*)uart_temp, "A%06f U%05.2f\n", 
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
			
			#if DEBUG
      sprintf((char*)uart_temp, "T%04.1f H%04.1f\n", 
			         temp,hum);	
      HAL_UART_Transmit(&UART5_Handler,(uint8_t *)uart_temp,strlen(uart_temp),1);
			#endif	
		}

#if WIFI //WIFI模式，非低功耗，每15s发送一组数据。
    if(SentCNT>WIFI_SEND_REPE/2)	
		{
			SendWIFI();
			SentCNT=0;
		}					
#else
		if(SentCNT>LORA_SEND_REPE/2)
		{
			SendLORA();
			SentCNT=0;
		}		
		//W25QXX_PowerDown();
		#if LPOWER
		Sys_Enter_Standby();
		#endif
#endif
  }
}

void SendLORA(void)
{
	LORA_WAKE(0);
	delay_ms(10);

  DecimalToHex(temp,uart_temp);
	DecimalToHex(hum ,uart_temp+4);
	DecimalToHex(bmp280_press ,uart_temp+8);
	DecimalToHex(uvi ,uart_temp+12);
	DecimalToHex(lux ,uart_temp+16);
	
	HAL_UART_Transmit(&UART5_Handler,(uint8_t *)uart_temp,20,1);
	
	LORA_WAKE(1);	
}


void RST_WIFI(void)
{
	LORA_WAKE(0);
	delay_ms(10);
	LORA_WAKE(1);
}
void SendWIFI(void)
{
	u16 msg_len;
	msg_len=sprintf(uart_temp,"{\"id\":\"%d\",\"params\":{\"CurrentTemperature\":%.1f,\"CurrentHumidity\":%d,\"CO2\":%d, \"HCHO\":%.3f, \"UVindex\":%.1f,\"LightLux\":%.1f,\"PM25\":%d},\"method\":\"thing.event.property.post\"}", cnt,temp,(int)hum,(u16)co2_value,HCHO_value,uvi,lux,pm25);
  HAL_UART_Transmit(&UART5_Handler,(uint8_t *)uart_temp,msg_len,1);
}

void CO2_get(void)
{
	u16 adc_value;
	
	adc_value=Get_Adc_Average(ADC_CHANNEL_7,5);
	
	co2_value=((float)adc_value/1024-0.4)*5000/1.6;
}

