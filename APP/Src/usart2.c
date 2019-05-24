#include "usart2.h"
#include "rtc.h"
//#include "w25qxx.h" 
#include "string.h"
#include "delay.h"
#include "timer.h"

//串口5中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
//char USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
//u16 USART_RX_STA = 0;     //接收状态标记

UART_HandleTypeDef UART2_Handler; //UART2句柄

UART_RX UART2_RX;

u16 pm25;
float HCHO_value=0;
//int rx5_cnt=0;

/**
 * @brief	初始化串口1函数
 *
 * @param	bound	串口波特率
 *
 * @return  void
 */
void uart2_init(u32 bound)
{
    //UART 初始化设置
	
		UART2_Handler.Instance = USART2;
		UART2_Handler.Init.BaudRate = bound;
		UART2_Handler.Init.WordLength = UART_WORDLENGTH_8B;
		UART2_Handler.Init.StopBits = UART_STOPBITS_1;
		UART2_Handler.Init.Parity = UART_PARITY_NONE;
		UART2_Handler.Init.Mode = UART_MODE_TX_RX;
		UART2_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		UART2_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
		HAL_UART_Init(&UART2_Handler);

    __HAL_UART_ENABLE_IT(&UART2_Handler, UART_IT_RXNE); //开启接收中断
    HAL_NVIC_EnableIRQ(USART2_IRQn);					//使能USART1中断通道
    HAL_NVIC_SetPriority(USART2_IRQn, 3, 3);				//抢占优先级3，子优先级3
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
void USART2_IRQHandler(void)
{
    u8 Res;

    if((__HAL_UART_GET_FLAG(&UART2_Handler, UART_FLAG_RXNE) != RESET)) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
			UART2_RX.rx_flag=1;			
      HAL_UART_Receive(&UART2_Handler, &Res, 1, 1000);

      UART2_RX.rx_buf[UART2_RX.rx_cnt]=Res;
			
			UART2_RX.check=UART2_RX.check+Res;
			
			UART2_RX.check2=UART2_RX.check-UART2_RX.rx_buf[UART2_RX.rx_cnt]-UART2_RX.rx_buf[UART2_RX.rx_cnt-1];
			
			if((UART2_RX.rx_buf[UART2_RX.rx_cnt]==(UART2_RX.check2&0x00ff))&&(UART2_RX.rx_buf[UART2_RX.rx_cnt-1]==(UART2_RX.check2>>8))&&
				 (UART2_RX.rx_buf[0]==0x42)&&(UART2_RX.rx_buf[1]==0x4D))
			{
				UART2_RX.con_flag=1;
			}
			UART2_RX.rx_cnt++;
    }
    HAL_UART_IRQHandler(&UART2_Handler);
}

void UART2_RX_task(void)
{	
//    char uart_temp[60];	
//	  u16 i;
	   u16 HCHO_temp;
	
		if(UART2_RX.con_flag==1)
		{
			UART2_RX.con_flag=0;
			UART2_RX.rx_cnt=0;
			UART2_RX.rx_flag=0;
			UART2_RX.check=0;
			
			pm25=(UART2_RX.rx_buf[12]<<8)|UART2_RX.rx_buf[13];
			HCHO_temp=(UART2_RX.rx_buf[28]<<8)|UART2_RX.rx_buf[29];
			HCHO_value=(float)HCHO_temp/1000;
							
		}
		
}






