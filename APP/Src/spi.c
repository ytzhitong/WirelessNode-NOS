#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//SPI���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
 
//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ������SD Card/W25Q64/NRF24L01						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��


SPI_HandleTypeDef SPI2_Handler;  //SPI3���


/**
 * @brief	SPI2��ʼ�����룬���ó�����ģʽ
 *
 * @param   void
 *
 * @return  void
 */
void SPI2_Init(void)
{
    SPI2_Handler.Instance=SPI2;                         //SPI3
    SPI2_Handler.Init.Mode=SPI_MODE_MASTER;             //����SPI����ģʽ������Ϊ��ģʽ
    SPI2_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ
    SPI2_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI2_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;    //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI2_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI2_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI2_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_2;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI2_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI2_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //�ر�TIģʽ
    SPI2_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//�ر�Ӳ��CRCУ��
    SPI2_Handler.Init.CRCPolynomial=7;                  //CRCֵ����Ķ���ʽ
    HAL_SPI_Init(&SPI2_Handler);//��ʼ��SPI3
    
    __HAL_SPI_ENABLE(&SPI2_Handler);                    //ʹ��SPI3
}

/**
 * @brief	SPI2�ײ�������ʱ��ʹ�ܣ���������
 *
 * @param   hspi	SPI���
 *
 * @return  void
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();       //ʹ��GPIOBʱ��
    __HAL_RCC_SPI2_CLK_ENABLE();        //ʹ��SPI3ʱ��

    //PB3.5
    GPIO_Initure.Pin=GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //�����������
    GPIO_Initure.Pull=GPIO_PULLUP;                  //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;   //80Mhz           
    GPIO_Initure.Alternate=GPIO_AF5_SPI2;           //PB3.5����ΪSPI3
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
}

/**
 * @brief	SPI2 ��дһ���ֽ�
 *
 * @param   TxData	Ҫд����ֽ�
 *
 * @return  u8		��ȡ�����ֽ�
 */
u8 SPI2_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
    HAL_SPI_TransmitReceive(&SPI2_Handler,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;          		    //�����յ�������		
}

/**
 * @brief	SPI2 д��һ���ֽ�
 *
 * @param   TxData	Ҫд����ֽ�
 * @param   size	д���ֽڴ�С
 *
 * @return  u8		0:д��ɹ�,����:д��ʧ��
 */
u8 SPI2_WriteByte(u8 *TxData,u16 size)
{
	return HAL_SPI_Transmit(&SPI2_Handler,TxData,size,1000);
}




