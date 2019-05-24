#include "uart5.h"
#include "rtc.h"
#include "w25qxx.h" 
#include "string.h"
#include "delay.h"
#include "timer.h"
#include "FlashFile.h"
#include "main.h"

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
 */void uart5_init(u32 bound)
{
    //UART ��ʼ������
	
		UART5_Handler.Instance = UART5;
		UART5_Handler.Init.BaudRate = bound;
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

    if((__HAL_UART_GET_FLAG(&UART5_Handler, UART_FLAG_RXNE) != RESET)) //�����ж�
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
			RST_WIFI();   //wifi��λ
		}				
	
	  if(run.uart5_cnt>20) //20ms���յ�һ֡���ݡ�
		{		
			if (strstr((const char *)UART5_RX.rx_buf, "publish success") != NULL)
			{
        run.WIFI_RST_cnt=0;
			}

			//���ڸ�λ
			run.uart5_cnt=0;
			UART5_RX.rx_cnt=0;
      UART5_RX.rx_flag=0;	
			memset(UART5_RX.rx_buf, 0, sizeof UART5_RX.rx_buf);
		}
#endif		
		
}








