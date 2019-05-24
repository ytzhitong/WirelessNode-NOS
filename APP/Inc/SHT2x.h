#ifndef __SHT2x_H__
#define __SHT2x_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys.h"
	 
#define SHT2x_I2C_PORT          GPIOC
#define SHT2x_SCL_PIN           GPIO_PIN_3
#define SHT2x_SDA_PIN           GPIO_PIN_2

#define SHT2x_SCL_HIGH()        HAL_GPIO_WritePin(SHT2x_I2C_PORT,SHT2x_SCL_PIN,GPIO_PIN_SET)
#define SHT2x_SCL_LOW()         HAL_GPIO_WritePin(SHT2x_I2C_PORT,SHT2x_SCL_PIN,GPIO_PIN_RESET)

#define SHT2x_SDA_HIGH() 	      HAL_GPIO_WritePin(SHT2x_I2C_PORT,SHT2x_SDA_PIN,GPIO_PIN_SET)
#define SHT2x_SDA_LOW()         HAL_GPIO_WritePin(SHT2x_I2C_PORT,SHT2x_SDA_PIN,GPIO_PIN_RESET)

#define SHT2x_SCL_STATE()       HAL_GPIO_ReadPin(SHT2x_I2C_PORT, SHT2x_SCL_PIN)
#define SHT2x_SDA_STATE()       HAL_GPIO_ReadPin(SHT2x_I2C_PORT, SHT2x_SDA_PIN)

typedef enum {
    TRIG_TEMP_MEASUREMENT_HM   = 0xE3, //command trig. temp meas. hold master
    TRIG_HUMI_MEASUREMENT_HM   = 0xE5, //command trig. humidity meas. hold master
    TRIG_TEMP_MEASUREMENT_POLL = 0xF3, //command trig. temp meas. no hold master
    TRIG_HUMI_MEASUREMENT_POLL = 0xF5, //command trig. humidity meas. no hold master
    USER_REG_W                 = 0xE6, //command writing user register
    USER_REG_R                 = 0xE7, //command reading user register
    SOFT_RESET                 = 0xFE  //command soft reset
} SHT2xCommand;

typedef enum {
    SHT2x_RES_12_14BIT         = 0x00, //RH=12bit, T=14bit
    SHT2x_RES_8_12BIT          = 0x01, //RH= 8bit, T=12bit
    SHT2x_RES_10_13BIT         = 0x80, //RH=10bit, T=13bit
    SHT2x_RES_11_11BIT         = 0x81, //RH=11bit, T=11bit
    SHT2x_RES_MASK             = 0x81  //Mask for res. bits (7,0) in user reg.
} SHT2xResolution;

typedef enum {
    SHT2x_EOB_ON               = 0x40, //end of battery
    SHT2x_EOB_MASK             = 0x40  //Mask for EOB bit(6) in user reg.
} SHT2xEob;

typedef enum {
    SHT2x_HEATER_ON            = 0x04, //heater on
    SHT2x_HEATER_OFF           = 0x00, //heater off
    SHT2x_HEATER_MASK          = 0x04  //Mask for Heater bit(2) in user reg.
} SHT2xHeater;

typedef enum {
    TEMP,  
    HUMI 
} SHT2xMeasureType;

typedef enum {
    I2C_ADR_W                  = 0x80, //sensor I2C address + write bit
    I2C_ADR_R                  = 0x81  //sensor I2C address + read bit
} SHT2xI2cHeader;

typedef enum {
    ACK                        = 0,
    NO_ACK                     = 1
} SHT2xI2cAck;

typedef enum {
    ACK_OK                     = 0x00,  
    ACK_ERROR                  = 0x01
} SHT2xI2cAckState;

typedef struct _sht2x_param_ {
    float TEMP_HM;
    float HUMI_HM;
    float TEMP_POLL;
    float HUMI_POLL;
    u8	TEMP_BT[4];
	  u8	HUMI_BT[4];
	  u8	SHT_BT[8];
	
    u8 SerialNumber[8];
} SHT2x_PARAM, *P_SHT2x_PARAM;

extern SHT2x_PARAM g_sht2x_param;

void SHT2x_Init(void);
float SHT2x_MeasureTempHM(void);
float SHT2x_MeasureHumiHM(void);
float SHT2x_MeasureTempPoll(void);
float SHT2x_MeasureHumiPoll(void);
u8 SHT2x_ReadUserReg(void);
u8 SHT2x_WriteUserReg(u8 reg);
void SHT2x_SoftReset(void);
void SHT2x_GetSerialNumber(u8 *buf);
void SHT2x_Test(void);


#ifdef __cplusplus
}
#endif

#endif
