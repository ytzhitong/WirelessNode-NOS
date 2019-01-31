#include "uart5.h"
#include "rtc.h"
#include "w25qxx.h" 
#include "string.h"
#include "delay.h"
#include "timer.h"

//����5�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
char USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0;     //����״̬���

UART_HandleTypeDef UART5_Handler; //UART5���

UART_RX UART5_RX;
int rx5_cnt=0;

/**
 * @brief	��ʼ������1����
 *
 * @param	bound	���ڲ�����
 *
 * @return  void
 */
void uart5_init(u32 bound)
{
    //UART ��ʼ������
	
		UART5_Handler.Instance = UART5;
		UART5_Handler.Init.BaudRate = 115200;
		UART5_Handler.Init.WordLength = UART_WORDLENGTH_8B;
		UART5_Handler.Init.StopBits = UART_STOPBITS_1;
		UART5_Handler.Init.Parity = UART_PARITY_NONE;
		UART5_Handler.Init.Mode = UART_MODE_TX_RX;
		UART5_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		UART5_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
		HAL_UART_Init(&UART5_Handler);

    __HAL_UART_ENABLE_IT(&UART5_Handler, UART_IT_RXNE); //���������ж�
    HAL_NVIC_EnableIRQ(UART5_IRQn);					//ʹ��USART1�ж�ͨ��
    HAL_NVIC_SetPriority(UART5_IRQn, 3, 3);				//��ռ���ȼ�3�������ȼ�3
}

/**
 * @brief	����5�жϷ������
 *
 * @remark	�����������ֱ�Ӱ��жϿ����߼�д���жϷ������ڲ�
 * 			˵��������HAL�⴦���߼���Ч�ʲ��ߡ�
 *
 * @param   void
 *
 * @return  void
 */
void UART5_IRQHandler(void)
{
    u8 Res;

    if((__HAL_UART_GET_FLAG(&UART5_Handler, UART_FLAG_RXNE) != RESET)) //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
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
				RTC_Set_Time(UART5_RX.rx_buf[4], UART5_RX.rx_buf[5], UART5_RX.rx_buf[6], RTC_HOURFORMAT12_PM);	     //����ʱ�� ,����ʵ��ʱ���޸�
			}

			if((UART5_RX.rx_buf[0]=='#')&&(UART5_RX.rx_buf[1]=='R')&&(UART5_RX.rx_buf[2]=='D')&&(UART5_RX.rx_buf[3]=='#'))
			{ 
        RTC_Set_Date(UART5_RX.rx_buf[4], UART5_RX.rx_buf[5], UART5_RX.rx_buf[6],UART5_RX.rx_buf[7]);		                 //��������
			}
			//��ȡflash
			if((UART5_RX.rx_buf[0]=='#')&&(UART5_RX.rx_buf[1]=='R')&&(UART5_RX.rx_buf[2]=='R')&&(UART5_RX.rx_buf[3]=='#'))
			{ 
				run.uart5tx_flag=1;
        //��ȡflash
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

			//����flash
			if((UART5_RX.rx_buf[0]=='#')&&(UART5_RX.rx_buf[1]=='R')&&(UART5_RX.rx_buf[2]=='E')&&(UART5_RX.rx_buf[3]=='#'))
			{ 
				run.uart5tx_flag=1;
        //����flash
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






