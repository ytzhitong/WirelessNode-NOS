#include "usart2.h"
#include "rtc.h"
//#include "w25qxx.h" 
#include "string.h"
#include "delay.h"
#include "timer.h"

//����5�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
//char USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
//u16 USART_RX_STA = 0;     //����״̬���

UART_HandleTypeDef UART2_Handler; //UART2���

UART_RX UART2_RX;

u16 pm25;
float HCHO_value=0;
//int rx5_cnt=0;

/**
 * @brief	��ʼ������1����
 *
 * @param	bound	���ڲ�����
 *
 * @return  void
 */
void uart2_init(u32 bound)
{
    //UART ��ʼ������
	
		UART2_Handler.Instance = USART2;
		UART2_Handler.Init.BaudRate = bound;
		UART2_Handler.Init.WordLength = UART_WORDLENGTH_8B;
		UART2_Handler.Init.StopBits = UART_STOPBITS_1;
		UART2_Handler.Init.Parity = UART_PARITY_NONE;
		UART2_Handler.Init.Mode = UART_MODE_TX_RX;
		UART2_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		UART2_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
		HAL_UART_Init(&UART2_Handler);

    __HAL_UART_ENABLE_IT(&UART2_Handler, UART_IT_RXNE); //���������ж�
    HAL_NVIC_EnableIRQ(USART2_IRQn);					//ʹ��USART1�ж�ͨ��
    HAL_NVIC_SetPriority(USART2_IRQn, 3, 3);				//��ռ���ȼ�3�������ȼ�3
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
void USART2_IRQHandler(void)
{
    u8 Res;

    if((__HAL_UART_GET_FLAG(&UART2_Handler, UART_FLAG_RXNE) != RESET)) //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
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






