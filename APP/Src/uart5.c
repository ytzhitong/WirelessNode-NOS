#include "uart5.h"
#include "rtc.h"
#include "w25qxx.h" 
#include "string.h"
#include "delay.h"
#include "timer.h"
#include "FlashFile.h"
#include "main.h"

//串口5中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
char USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0;     //接收状态标记

UART_HandleTypeDef UART5_Handler; //UART5句柄

UART_RX UART5_RX;
int rx5_cnt=0;

/**
 * @brief	初始化串口1函数
 *
 * @param	bound	串口波特率
 *
 * @return  void
 */void uart5_init(u32 bound)
{
    //UART 初始化设置
	
		UART5_Handler.Instance = UART5;
		UART5_Handler.Init.BaudRate = bound;
		UART5_Handler.Init.WordLength = UART_WORDLENGTH_8B;
		UART5_Handler.Init.StopBits = UART_STOPBITS_1;
		UART5_Handler.Init.Parity = UART_PARITY_NONE;
		UART5_Handler.Init.Mode = UART_MODE_TX_RX;
		UART5_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		UART5_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
		HAL_UART_Init(&UART5_Handler);

    __HAL_UART_ENABLE_IT(&UART5_Handler, UART_IT_RXNE); //开启接收中断
    HAL_NVIC_EnableIRQ(UART5_IRQn);					//使能USART1中断通道
    HAL_NVIC_SetPriority(UART5_IRQn, 3, 3);				//抢占优先级3，子优先级3
}

/**
 * @brief	串口5中断服务程序
 *
 * @remark	下面代码我们直接把中断控制逻辑写在中断服务函数内部
 * 			说明：采用HAL库处理逻辑，效率不高。
 *
 * @param   void
 *
 * @return  void
 */
void UART5_IRQHandler(void)
{
    u8 Res;

    if((__HAL_UART_GET_FLAG(&UART5_Handler, UART_FLAG_RXNE) != RESET)) //接收中断
    {
			UART5_RX.rx_flag=1;			
      HAL_UART_Receive(&UART5_Handler, &Res, 1, 1000);

      UART5_RX.rx_buf[UART5_RX.rx_cnt]=Res;

			UART5_RX.rx_cnt++;
    }
    HAL_UART_IRQHandler(&UART5_Handler);
}

void UART5_RX_task(void)
{	
#if WIFI		
	  if(run.WIFI_RST_cnt>130000)
		{
			run.WIFI_RST_cnt=0;
			RST_WIFI();   //wifi复位
		}				
	
	  if(run.uart5_cnt>20) //20ms接收到一帧数据。
		{		
			if (strstr((const char *)UART5_RX.rx_buf, "publish success") != NULL)
			{
        run.WIFI_RST_cnt=0;
			}

			//串口复位
			run.uart5_cnt=0;
			UART5_RX.rx_cnt=0;
      UART5_RX.rx_flag=0;	
			memset(UART5_RX.rx_buf, 0, sizeof UART5_RX.rx_buf);
		}
#endif		
		
}








