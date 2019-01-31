#ifndef __SI1133_H__
#define __SI1133_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys.h"
#include "stdbool.h"
	 
#define SI1133_I2C_PORT          GPIOA
#define SI1133_SCL_PIN           GPIO_PIN_5
#define SI1133_SDA_PIN           GPIO_PIN_4
#define SI1133_INT_PIN           GPIO_PIN_6

#define SI1133_SCL_HIGH()        HAL_GPIO_WritePin(SI1133_I2C_PORT,SI1133_SCL_PIN,GPIO_PIN_SET)
#define SI1133_SCL_LOW()         HAL_GPIO_WritePin(SI1133_I2C_PORT,SI1133_SCL_PIN,GPIO_PIN_RESET)

#define SI1133_SDA_HIGH() 	     HAL_GPIO_WritePin(SI1133_I2C_PORT,SI1133_SDA_PIN,GPIO_PIN_SET)
#define SI1133_SDA_LOW()         HAL_GPIO_WritePin(SI1133_I2C_PORT,SI1133_SDA_PIN,GPIO_PIN_RESET)

#define SI1133_SCL_STATE()       HAL_GPIO_ReadPin(SI1133_I2C_PORT, SI1133_SCL_PIN)
#define SI1133_SDA_STATE()       HAL_GPIO_ReadPin(SI1133_I2C_PORT, SI1133_SDA_PIN)


#define SI1133_PART_ID				  0xA4
 
//*******************************************************//
//*                   ADCCONFIGx                        *//
//*******************************************************//
 
//=========================================================
//  measurement rate,Offsets for Parameter Table ADCCONFIGx
//=========================================================
#define CFG_RATE_SHORT  		  0X60 //24.4us
#define CFG_RATE_NORMAL 		  0X00 //48.8us
#define CFG_RATE_LONG   		  0X20 //97.6us
#define CFG_RATE_VLONG  		  0X40 //195us
//==========================================================
// Photodiodos select,Offsets for Parameter Table ADCCONFIGx
//==========================================================
#define CFG_SMALL_IR 			  0X00 
#define CFG_MEDIUM_IR			  0X01
#define CFG_LARGE_IR			  0X02
#define CFG_WHITE   			  0X0B
#define CFG_LARGE_WHITE 		  0X0C
#define CFG_UV  				  0X18
#define CFG_UV_DEEP 			  0X19
 
//------------------------END------------------------------
 
//*******************************************************//
//*                   MEASCONFIGx                       *//
//*******************************************************//
 
//===========================
// MEASCOUNTER select,Value for Parameter Table MEASCONFIGx
//===========================
#define MEAS_COUNT_NONE 		  0X00
#define MEAS_COUNT_0 			  0X40
#define MEAS_COUNT_1 			  0X80
#define MEAS_COUNT_2 			  0XC0
 
//------------------------END------------------------------
 
//*******************************************************//
//*                      ADCPOSTx                       *//
//*******************************************************//
//===========================
// OUTPUT setting,Offsets for Parameter Table ADCPOSTx
//===========================
#define POST_BITS_16			  0X00
#define POST_BITS_24			  0X40
//===========================
// THRESHOLD setting,Offsets for Parameter Table ADCPOSTx
//===========================
#define POST_THRESHOLD_NONE		  0X00
#define POST_THRESHOLD_0		  0X01
#define POST_THRESHOLD_1		  0X02
#define POST_THRESHOLD_2		  0X03
//------------------------END------------------------------
 
//===========================
// I2C Registers Address
//===========================
#define REG_PART_ID               0x00
#define REG_REV_ID                0x01
#define REG_MFR_ID                0x02
#define REG_INFO0             	  0x03
#define REG_INFO1                 0x04
 
#define REG_HOSTIN3               0x07
#define REG_HOSTIN2               0x08
#define REG_HOSTIN1               0x09
#define REG_HOSTIN0               0x0A
#define REG_COMMAND               0x0B
 
#define REG_IRQ_ENABLE            0x0F
#define REG_RESET                 0x0F
#define REG_RESPONSE1             0x10
#define REG_RESPONSE0             0x11
#define REG_IRQ_STATUS            0x12
 
#define REG_HOSTOUT0              0x13
#define REG_HOSTOUT1              0x14
#define REG_HOSTOUT2              0x15
#define REG_HOSTOUT3              0x16
#define REG_HOSTOUT4              0x17
#define REG_HOSTOUT5              0x18
#define REG_HOSTOUT6              0x19
#define REG_HOSTOUT7              0x1A
#define REG_HOSTOUT8              0x1B
#define REG_HOSTOUT9              0x1C
#define REG_HOSTOUT10             0x1D
#define REG_HOSTOUT11             0x1E
#define REG_HOSTOUT12             0x1F
#define REG_HOSTOUT13             0x20
#define REG_HOSTOUT14             0x21
#define REG_HOSTOUT15             0x22
#define REG_HOSTOUT16             0x23
#define REG_HOSTOUT17             0x24
#define REG_HOSTOUT18             0x25
#define REG_HOSTOUT19             0x26
#define REG_HOSTOUT20             0x27
#define REG_HOSTOUT21             0x28
#define REG_HOSTOUT22             0x29
#define REG_HOSTOUT23             0x2A
#define REG_HOSTOUT24             0x2B
#define REG_HOSTOUT25             0x2C
//============================================================
// Parameter Table Offsets
//============================================================
#define PARAM_I2C_ADDR            0x00
#define PARAM_CHAN_LIST           0x01
 
#define PARAM_ADCCONFIG0          0x02
#define PARAM_ADCSENS0            0x03
#define PARAM_ADCPOST0            0x04
#define PARAM_MEASCONFIG0         0x05
 
#define PARAM_ADCCONFIG1          0x06
#define PARAM_ADCSENS1            0x07
#define PARAM_ADCPOST1            0x08
#define PARAM_MEASCONFIG1         0x09
 
#define PARAM_ADCCONFIG2          0x0A
#define PARAM_ADCSENS2            0x0B
#define PARAM_ADCPOST2            0x0C
#define PARAM_MEASCONFIG2         0x0D
 
#define PARAM_ADCCONFIG3          0x0E
#define PARAM_ADCSENS3            0x0F
#define PARAM_ADCPOST3            0x10
#define PARAM_MEASCONFIG3         0x11
 
#define PARAM_ADCCONFIG4          0x12
#define PARAM_ADCSENS4            0x13
#define PARAM_ADCPOST4            0x14
#define PARAM_MEASCONFIG4         0x15
 
#define PARAM_ADCCONFIG5          0x16
#define PARAM_ADCSENS5            0x17
#define PARAM_ADCPOST5            0x18
#define PARAM_MEASCONFIG5         0x19
 
#define PARAM_MEASRATE_H          0x1A
#define PARAM_MEASRATE_L          0x1B
#define PARAM_MEASCOUNT0          0x1C
#define PARAM_MEASCOUNT1          0x1D
#define PARAM_MEASCOUNT2          0x1E
 
#define PARAM_LED1_A        	  0x1F
#define PARAM_LED1_B        	  0x20
#define PARAM_LED2_A        	  0x21
#define PARAM_LED2_B        	  0x22
#define PARAM_LED3_A        	  0x23
#define PARAM_LED3_B        	  0x24
 
#define PARAM_THRESHOLD0_H  	  0x25
#define PARAM_THRESHOLD0_L  	  0x26
#define PARAM_THRESHOLD1_H  	  0x27
#define PARAM_THRESHOLD1_L  	  0x28
#define PARAM_THRESHOLD2_H  	  0x29
#define PARAM_THRESHOLD2_L  	  0x2A
#define PARAM_BURST         	  0x2B
 
//==================================================================
//COMMAND Register Value
//==================================================================
#define CMD_RESET_CTR			  0x00
#define CMD_RESET_SW			  0x01
#define CMD_FORCE				  0x11
#define CMD_PAUSE				  0x12
#define CMD_START				  0x13
#define CMD_WRITE				  0x80
#define CMD_READ				  0x40
 
/*******************************************************************************
 *******    Si115x Register and Parameter Bit Definitions  *********************
 ******************************************************************************/
#define RSP0_CHIPSTAT_MASK      0xe0
#define RSP0_COUNTER_MASK       0x1f
#define RSP0_SLEEP              0x20

/*This section is for variables related to reading ambient light and UV*/
#define X_ORDER_MASK 0x0070
#define Y_ORDER_MASK 0x0007
#define SIGN_MASK    0x0080
#define get_x_order(m)   ( (m & X_ORDER_MASK) >> 4 )
#define get_y_order(m)   ( (m & Y_ORDER_MASK)      )
#define get_sign(m)      ( (m & SIGN_MASK   ) >> 7 )

#define UV_INPUT_FRACTION       15
#define UV_OUTPUT_FRACTION      12
#define UV_NUMCOEFF             2

#define ADC_THRESHOLD           16000
#define INPUT_FRACTION_HIGH     7
#define INPUT_FRACTION_LOW      15
#define LUX_OUTPUT_FRACTION     12
#define NUMCOEFF_LOW            9
#define NUMCOEFF_HIGH           4

typedef struct Si115xSample
{
    uint8_t     irq_status;
    int32_t     ch0;
    int32_t     ch1;
    int32_t     ch2;
    int32_t     ch3;
} Si115xSample_t;
extern Si115xSample_t samples;	

typedef void *HANDLE;

typedef struct {
  int16_t     info;
  uint16_t    mag;
} COEFF;

typedef struct {
  COEFF   coeff_high[4];
  COEFF   coeff_low[9];
} LUX_COEFF;

extern int16_t SI1133_ReadRegister(uint8_t addr);
extern int16_t SI1133_WriteRegister(uint8_t addr, uint8_t data);
extern int16_t SI1133_ReadParameter(uint8_t addr);
extern int16_t SI1133_WriteParameter(uint8_t addr, uint8_t data);
extern int16_t SI1133_Init(void);
extern int16_t SI1133_NOP(void);
extern int16_t SI1133_Force(void);
extern int16_t SI1133_Start (void);
extern int16_t SI1133_Pause(void);

void getSensorData(void);
float Si1133_getUVReading( HANDLE si115x_handle,
                        Si115xSample_t *samples );
float Si1133_getLuxReading( HANDLE si115x_handle,
                        Si115xSample_t *samples );

//bool bme280Init(void);
//void bme280GetData(float* pressure, float* temperature,float* humidity,float* asl);
//	 

//void BME280_Test(void);


#ifdef __cplusplus
}
#endif

#endif
