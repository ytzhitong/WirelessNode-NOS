#ifndef __BME280_H__
#define __BME280_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys.h"
#include "stdbool.h"
	 
#define BME280_I2C_PORT          GPIOC
#define BME280_SCL_PIN           GPIO_PIN_2
#define BME280_SDA_PIN           GPIO_PIN_3

#define BME280_SCL_HIGH()        HAL_GPIO_WritePin(BME280_I2C_PORT,BME280_SCL_PIN,GPIO_PIN_SET)
#define BME280_SCL_LOW()         HAL_GPIO_WritePin(BME280_I2C_PORT,BME280_SCL_PIN,GPIO_PIN_RESET)

#define BME280_SDA_HIGH() 	     HAL_GPIO_WritePin(BME280_I2C_PORT,BME280_SDA_PIN,GPIO_PIN_SET)
#define BME280_SDA_LOW()         HAL_GPIO_WritePin(BME280_I2C_PORT,BME280_SDA_PIN,GPIO_PIN_RESET)

#define BME280_SCL_STATE()       HAL_GPIO_ReadPin(BME280_I2C_PORT, BME280_SCL_PIN)
#define BME280_SDA_STATE()       HAL_GPIO_ReadPin(BME280_I2C_PORT, BME280_SDA_PIN)


#define BME280_ADDR						(0xEC)
#define BME280_DEFAULT_CHIP_ID			(0x60)

#define BME280_CHIP_ID					(0xD0)                                 /* Chip ID Register */
#define BME280_RST_REG					(0xE0)                                 /* Softreset Register */
#define BME280_CTRL_HUM                 (0xF2)                                 /* Ctrl Humidity Register */
#define BME280_STAT_REG					(0xF3)                                 /* Status Register */
#define BME280_CTRL_MEAS_REG			(0xF4)                                 /* Ctrl Measure Register */
#define BME280_CONFIG_REG				(0xF5)                                 /* Configuration Register */
#define BME280_PRESSURE_MSB_REG			(0xF7)                                 /* Pressure MSB Register */
#define BME280_PRESSURE_LSB_REG			(0xF8)                                 /* Pressure LSB Register */
#define BME280_PRESSURE_XLSB_REG		(0xF9)                                 /* Pressure XLSB Register */
#define BME280_TEMPERATURE_MSB_REG		(0xFA)                                 /* Temperature MSB Reg */
#define BME280_TEMPERATURE_LSB_REG		(0xFB)                                 /* Temperature MSB Reg */
#define BME280_TEMPERATURE_XLSB_REG		(0xFC)                                 /* Temperature XLSB Reg */
#define BME280_HUMIDITY_MSB_REG			(0xFD)                                 /* Humidity MSB Reg */
#define BME280_HUMIDITY_LSB_REG		    (0xFE)                                 /* Humidity LSB Reg */

#define BME280_SLEEP_MODE				(0x00)
#define BME280_FORCED_MODE				(0x01)
#define BME280_NORMAL_MODE				(0x03)

#define BME280_TEMPERATURE_CALIB_DIG_T1_LSB_REG             (0x88)
#define BME280_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH       (32)
#define BME280_DATA_FRAME_SIZE			(8)

#define BME280_OVERSAMP_SKIPPED			(0x00)
#define BME280_OVERSAMP_1X				(0x01)
#define BME280_OVERSAMP_2X				(0x02)
#define BME280_OVERSAMP_4X				(0x03)
#define BME280_OVERSAMP_8X				(0x04)
#define BME280_OVERSAMP_16X				(0x05)


bool bme280Init(void);
void bme280GetData(float* pressure, float* temperature,float* humidity,float* asl);
	 

void BME280_Test(void);


#ifdef __cplusplus
}
#endif

#endif
