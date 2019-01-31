#include "uart5.h"
#include "rtc.h"
#include "w25qxx.h" 
#include "string.h"
#include "delay.h"
#include "timer.h"

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
 */
void uart5_init(u32 bound)
{
    //UART 初始化设置
	
		UART5_Handler.Instance = UART5;
		UART5_Handler.Init.BaudRate = 115200;
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

    if((__HAL_UART_GET_FLAG(&UART5_Handler, UART_FLAG_RXNE) != RESET)) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
			UART5_RX.rx_flag=1;			
      HAL_UART_Receive(&UART5_Handler, &Res, 1, 1000);

      UART5_RX.rx_buf[UART5_RX.rx_cnt]=Res;
			if((UART5_RX.rx_buf[UART5_RX.rx_cnt]=='D')&&(UART5_RX.rx_buf[UART5_RX.rx_cnt-1]=='N') &&(UART5_RX.rx_buf[UART5_RX.rx_cnt-2]=='E')&&(UART5_RX.rx_buf[UART5_RX.rx_cnt-3]=='#'))
			{
				UART5_RX.con_flag=1;
			}
			UART5_RX.rx_cnt++;
    }
    HAL_UART_IRQHandler(&UART5_Handler);
}

void UART5_RX_task(void)
{	
    char uart_temp[60];	
	  u16 i;
	
		if(UART5_RX.con_flag==1)
		{
			UART5_RX.con_flag=0;
			UART5_RX.rx_cnt=0;
			UART5_RX.rx_flag=0;
			
			if((UART5_RX.rx_buf[0]=='#')&&(UART5_RX.rx_buf[1]=='R')&&(UART5_RX.rx_buf[2]=='T')&&(UART5_RX.rx_buf[3]=='#'))
			{
				RTC_Set_Time(UART5_RX.rx_buf[4], UART5_RX.rx_buf[5], UART5_RX.rx_buf[6], RTC_HOURFORMAT12_PM);	     //设置时间 ,根据实际时间修改
			}

			if((UART5_RX.rx_buf[0]=='#')&&(UART5_RX.rx_buf[1]=='R')&&(UART5_RX.rx_buf[2]=='D')&&(UART5_RX.rx_buf[3]=='#'))
			{ 
        RTC_Set_Date(UART5_RX.rx_buf[4], UART5_RX.rx_buf[5], UART5_RX.rx_buf[6],UART5_RX.rx_buf[7]);		                 //设置日期
			}
			//读取flash
			if((UART5_RX.rx_buf[0]=='#')&&(UART5_RX.rx_buf[1]=='R')&&(UART5_RX.rx_buf[2]=='R')&&(UART5_RX.rx_buf[3]=='#'))
			{ 
				run.uart5tx_flag=1;
        //读取flash
				W25QXX_ReadCNT();
				sprintf((char*)uart_temp, "flashCNT:%d\n",W25Q128_CNTT );
			  HAL_UART_Transmit(&UART5_Handler,(uint8_t *)uart_temp,strlen((char *)uart_temp),1);				
//				HAL_UART_Transmit(&UART5_Handler,(uint8_t *)"\n",1,1);
				
        for(i=0;i<W25Q128_CNTT;i++)
				{
					W25QXX_Read((u8*)uart_temp,i*60+2,60);
					HAL_UART_Transmit(&UART5_Handler,(uint8_t *)uart_temp,strlen((char *)uart_temp),1);
					delay_ms(10);
				}
//				run.uart5tx_flag=0;
			}

			//擦除flash
			if((UART5_RX.rx_buf[0]=='#')&&(UART5_RX.rx_buf[1]=='R')&&(UART5_RX.rx_buf[2]=='E')&&(UART5_RX.rx_buf[3]=='#'))
			{ 
				run.uart5tx_flag=1;
        //擦除flash
					W25Q128_CNTT=0;
					W25QXX_WriteCNT();
//        for(i=0;i<W25Q128_CNTT;i++)
//				{
//					W25QXX_Read((u8*)uart_temp,i*60+2,60);
//					HAL_UART_Transmit(&UART5_Handler,(uint8_t *)uart_temp,strlen((char *)uart_temp),1);
//					delay_ms(10);
//				}
//				run.uart5tx_flag=0;
			}	
			if((UART5_RX.rx_buf[0]=='#')&&(UART5_RX.rx_buf[1]=='R')&&(UART5_RX.rx_buf[2]=='X')&&(UART5_RX.rx_buf[3]=='#'))
			{ 

				run.uart5tx_flag=0;
			}					
		}
		
}






