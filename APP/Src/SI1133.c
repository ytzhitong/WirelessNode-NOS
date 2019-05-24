/** 
 * @file     SI1133.c
 * @brief    温湿压传感器
 */

#include "SI1133.h"
#include "delay.h"
#include "MY_MATH.h"
#include <math.h>
#include "stdbool.h"
#include "uart5.h"
#include "string.h"
#include "SHT2x.h"

u8 cntt=0;

extern u8 SI1133_OK;

Si115xSample_t samples;				//Stores the sample data from reading the sensor

COEFF uk[2] = { {1537, 27440}, {2, 59952} };

// Coefficieints for the Si1133 ALS
LUX_COEFF  lk_SI1133_AA00 =
			  { { {0, 79},      // coeff_high[0]
                  {1665, 108},      // coeff_high[1]
                  {2064, 107},      // coeff_high[2]
                  {-2543, 232} },    // coeff_high[3]
                { {0, 0},      // coeff_low[0]
                  {1665, 18409},      // coeff_low[1]
                  {-1022, 35015},      // coeff_low[2]
                  {2064, 16737},      // coeff_low[3]
                  {-367, 52971},      // coeff_low[4]
                  {-1774, 34292},      // coeff_low[5]
                  {-1376, 42635},      // coeff_low[6]
                  {-1503, 64731},      // coeff_low[7]
                  {-2142, 33512} } };  // coeff_low[8]

void SI1133_SCL_OUTPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.Pin =  SI1133_SCL_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(SI1133_I2C_PORT, &GPIO_InitStructure);
}

void SI1133_SCL_INPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.Pin =  SI1133_SCL_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(SI1133_I2C_PORT, &GPIO_InitStructure);
}

void SI1133_SDA_OUTPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.Pin =  SI1133_SDA_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(SI1133_I2C_PORT, &GPIO_InitStructure);
}

void SI1133_SDA_INPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.Pin =  SI1133_SDA_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(SI1133_I2C_PORT, &GPIO_InitStructure);
}

void SI1133_I2cStartCondition(void)
{
    SI1133_SCL_OUTPUT();
    SI1133_SDA_OUTPUT();
    
    SI1133_SDA_HIGH();
    SI1133_SCL_HIGH();
	  delay_us(4);
    SI1133_SDA_LOW(); 
    delay_us(4);	
    SI1133_SCL_LOW();   
}

void SI1133_I2cStopCondition(void)
{
    SI1133_SCL_OUTPUT();
    SI1133_SDA_OUTPUT();
    
    SI1133_SDA_LOW();
    SI1133_SCL_LOW();
    delay_us(4);
    SI1133_SCL_HIGH();
    SI1133_SDA_HIGH();
    delay_us(4);
}

void SI1133_I2cAcknowledge(void)
{
    SI1133_SCL_OUTPUT();
    SI1133_SDA_OUTPUT();

    SI1133_SDA_LOW();
    
    SI1133_SCL_HIGH();
    delay_us(80);
    SI1133_SCL_LOW();   
    delay_us(80);
}

void SI1133_I2cNoAcknowledge(void)
{
    SI1133_SCL_OUTPUT();
    SI1133_SDA_OUTPUT();

    SI1133_SDA_HIGH();
    
    SI1133_SCL_HIGH();
    delay_us(80);
    SI1133_SCL_LOW();   
    delay_us(80);
}

u8 SI1133_I2cReadByte(unsigned char ack)
{
    u8 i, val = 0;


    SI1133_SCL_OUTPUT();
    SI1133_SDA_OUTPUT();

    SI1133_SDA_HIGH();
    
    SI1133_SDA_INPUT();
        
    for(i = 0; i < 8; i++)
    {
        val <<= 1; 
        
        SI1133_SCL_HIGH();   
        
        if(Bit_SET == SI1133_SDA_STATE()) 
        {
            val |= 0x01;
        }
        
        SI1133_SCL_LOW();    
    }

    if (!ack)
        SI1133_I2cNoAcknowledge();       //发送nACK
    else
        SI1133_I2cAcknowledge();    //发送ACK     
    return (val);
}

u8 SI1133_I2cWriteByte(u8 byte)
{
    u8 i, ack;


    SI1133_SCL_OUTPUT();
    SI1133_SDA_OUTPUT();
	  SI1133_SCL_LOW();
    
    for(i = 0; i < 8; i++)
    {
        if(byte & 0x80) 
        {
            SI1133_SDA_HIGH();
        }
        else 
        {
            SI1133_SDA_LOW();
        }
        delay_us(4);
        SI1133_SCL_HIGH();
        delay_us(4);
        SI1133_SCL_LOW();   
        delay_us(4);
        
        byte <<= 1;
    }

    SI1133_SDA_INPUT();
    
    SI1133_SCL_HIGH();
    
    if(Bit_SET == SI1133_SDA_STATE()) 
    {
        ack = ACK_ERROR;
    }
    else
    {
        ack = ACK_OK;
    }
    
    SI1133_SCL_LOW();  

    SI1133_SDA_OUTPUT();

    SI1133_SDA_HIGH();
    
    return (ack);
}




//从指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 SI1133iicDevReadByte(u8 devaddr,u8 addr)
{				  
	u8 temp=0;		  	    																 
	SI1133_I2cStartCondition();  
	SI1133_I2cWriteByte(devaddr);                                                    //发送器件写命令 	   
	SI1133_I2cWriteByte(addr);                                                       //发送低地址	

	SI1133_I2cStartCondition();  	 	   
	SI1133_I2cWriteByte(devaddr|1);                                                  //发送器件读命令			   
	temp=SI1133_I2cReadByte(0);			   
	SI1133_I2cStopCondition();                                                                //产生一个停止条件	    
	return temp;
}

//连续读多个字节
//addr:起始地址
//rbuf:读数据缓存
//len:数据长度
void SI1133iicDevRead(u8 devaddr,u8 addr,u8 len,u8 *rbuf)
{
	int i=0;
	SI1133_I2cStartCondition();  
	SI1133_I2cWriteByte(devaddr);  
	SI1133_I2cWriteByte(addr);                                                       //地址自增  

	SI1133_I2cStartCondition();  	
	SI1133_I2cWriteByte(devaddr|1);  	
	for(i=0; i<len; i++)
	{
		if(i==len-1)
		{
			rbuf[i]=SI1133_I2cReadByte(0);                                          //最后一个字节不应答
		}
		else
			rbuf[i]=SI1133_I2cReadByte(1);
	}
	SI1133_I2cStopCondition( );	
}


//从指定地址写入一个数据
//WriteAddr :写入数据的目的地址    
//DataToWrite:要写入的数据
void SI1133iicDevWriteByte(u8 devaddr,u8 addr,u8 data)
{				   	  	    																 
	SI1133_I2cStartCondition();  
	SI1133_I2cWriteByte(devaddr);                                                    //发送器件写命令 	 	   
	SI1133_I2cWriteByte(addr);                                                       //发送低地址 	 										  		   
	SI1133_I2cWriteByte(data);                                                       //发送字节							    		    	   
	SI1133_I2cStopCondition();		                                                           //产生一个停止条件 	 
}

//连续写多个字节
//addr:起始地址
//wbuf:写数据缓存
//len:数据的长度
void SI1133iicDevWrite(u8 devaddr,u8 addr,u8 len,u8 *wbuf)
{
	int i=0;
	SI1133_I2cStartCondition();  
	SI1133_I2cWriteByte(devaddr);  		
	SI1133_I2cWriteByte(addr);  //地址自增	
	for(i=0; i<len; i++)
	{
		SI1133_I2cWriteByte(wbuf[i]);  		
	}
	SI1133_I2cStopCondition( );	
}

void SI1133_Test(void)
{   

}



bool SI1133_GPIO_Init(void)
{	
    __HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOA时钟
	
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin =  SI1133_SCL_PIN | SI1133_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(SI1133_I2C_PORT, &GPIO_InitStruct);

    SI1133_SCL_HIGH();
    SI1133_SDA_HIGH();

    delay_ms(20);
	
    return true;
}

u8 SI1133_ReadRegister(uint8_t addr)
{
	return SI1133iicDevReadByte(SI1133_PART_ID, addr);
	 
}
 
int16_t SI1133_WriteRegister(uint8_t addr, uint8_t data)
{
	SI1133iicDevWriteByte(SI1133_PART_ID,addr,data);
	return 0;
}
 
static int16_t SI1133_WriteBlock(uint8_t addr, uint8_t* p_data, uint16_t len)
{
//	I2C_WriteBlock(SI1133_PART_ID, addr, p_data, len);
	SI1133iicDevWrite(SI1133_PART_ID,addr,len,p_data);
	return 0;
}
 
static int16_t SI1133_ReadBlock(uint8_t addr, uint8_t* p_data, uint16_t len)
{
//	I2C_ReadBlock(SI1133_PART_ID, addr, p_data, len);
	SI1133iicDevRead(SI1133_PART_ID,addr,len,p_data);
	return 0;
}
 
static int16_t WaitUntilSleep(void)
{
	int16_t retval = -1;
	uint8_t count = 0;
	while (count < 5)
	{
		retval = SI1133_ReadRegister(REG_RESPONSE0);
		if ((retval & RSP0_CHIPSTAT_MASK) == RSP0_SLEEP)   break;
		if (retval <  0)   return retval;
		count++;
	}
	return 0;
}
 
static int16_t SI1133_SendCMD(uint8_t command)
{
	int16_t  response, retval;
	uint8_t  count = 0;
	response = SI1133_ReadRegister(REG_RESPONSE0);
	if (response < 0) return response;
	response = response & RSP0_COUNTER_MASK;
	for (count = 0 ; count < 5; count++)
	{
		if ((retval = WaitUntilSleep()) != 0) return retval;
		if (command == 0) break;
		retval = SI1133_ReadRegister(REG_RESPONSE0);
		if ((retval & RSP0_COUNTER_MASK) == response) break;
		else if (retval < 0) return retval;
		else
			response = retval & RSP0_COUNTER_MASK;
	}
	if ((retval = (SI1133_WriteRegister(REG_COMMAND, command)) != 0))
	{
		return retval;
	}
	for (count = 0 ; count < 5; count++)
	{
		if (command == 0)  break;
		retval = SI1133_ReadRegister(REG_RESPONSE0);
		if ((retval & RSP0_COUNTER_MASK) != response) break;
		else if (retval < 0) return retval;
	}
	return 0;
}
 
int16_t SI1133_ReadParameter(uint8_t addr)
{
	int16_t retval;
	retval = SI1133_SendCMD(CMD_READ | (addr & 0x3F));
	if ( retval != 0 )    return retval;
	retval = SI1133_ReadRegister(REG_RESPONSE1);
	return retval;
}
 
int16_t SI1133_WriteParameter(uint8_t addr, uint8_t data)
{
	u8 cnt=0;
	
	uint8_t buffer[2];
	int16_t retval, response_stored, response;
	buffer[0] = data; buffer[1] = CMD_WRITE | (addr & 0x3F);
	if ((retval = WaitUntilSleep()) != 0)    return retval;
	response_stored = RSP0_COUNTER_MASK & SI1133_ReadRegister(REG_RESPONSE0);
	retval = SI1133_WriteBlock(REG_HOSTIN0, (uint8_t *)buffer, 2);
	if (retval != 0)  return retval;
	response = SI1133_ReadRegister(REG_RESPONSE0);
	while ((response & RSP0_COUNTER_MASK) == response_stored)
	{
		response = SI1133_ReadRegister(REG_RESPONSE0);
		delay_us(100);
		cnt++;
		if(cnt>20)
		{
			SI1133_Reset();
	    delay_ms(10);
			return 0;
		}
	}
//	SI1133_OK=1;
	if (retval < 0)   return retval;
	else   return 0;
}
 
 int16_t SI1133_Reset(void)
{
	int16_t retval = 0;
	delay_ms(10);
	delay_ms(10);
	delay_ms(10);
	retval += SI1133_WriteRegister(REG_COMMAND, CMD_RESET_SW);
	delay_ms(10);
	return retval;
}
 
int16_t SI1133_NOP(void)
{
	return SI1133_SendCMD(CMD_RESET_CTR);
}
 
int16_t SI1133_Force(void)
{
	return SI1133_SendCMD(CMD_FORCE);
}
 
int16_t SI1133_Start (void)
{
	return SI1133_SendCMD(CMD_START);
}
 
int16_t SI1133_Pause(void)
{
	uint8_t countA, countB;
	int8_t  retval = 0;
	while ((RSP0_CHIPSTAT_MASK & retval) != RSP0_SLEEP)
	{
		retval = SI1133_ReadRegister(REG_RESPONSE0);
	}
	countA = 0;
	while (countA < 5)
	{
		countB = 0;
		while (countB < 5)
		{
			retval = SI1133_ReadRegister(REG_RESPONSE0);
			if ((retval & RSP0_COUNTER_MASK) == 0)     break;
			else
			{
				SI1133_WriteRegister(REG_COMMAND, 0x00);
			}
			countB++;
		}
		SI1133_SendCMD(CMD_PAUSE);
		countB = 0;
		while (countB < 5)
		{
			retval = SI1133_ReadRegister(REG_RESPONSE0);
			if ((retval & RSP0_COUNTER_MASK) != 0)        break;
			countB++;
		}
		retval = SI1133_ReadRegister(REG_RESPONSE0);
		if ((retval & RSP0_COUNTER_MASK) == 1 )      break;
		countA++;
	}
	return 0;
}

int16_t SI1133_Init_UvAls(void)
{
	int16_t    retval;
		
	retval  = SI1133_Reset();
	delay_ms(10);
	
	retval += SI1133_WriteParameter( PARAM_CHAN_LIST, 	0x0f);
	//UV
	retval += SI1133_WriteParameter( PARAM_ADCCONFIG0, 	0x78);
	retval += SI1133_WriteParameter( PARAM_ADCSENS0, 	0x09);
	retval += SI1133_WriteParameter( PARAM_ADCPOST0, 	0x40);
//	retval += SI1133_WriteParameter( PARAM_MEASCONFIG0, 0x4d);
	//
	retval += SI1133_WriteParameter( PARAM_ADCCONFIG1, 	0x4d);
	retval += SI1133_WriteParameter( PARAM_ADCSENS1, 	0x61);
	retval += SI1133_WriteParameter( PARAM_ADCPOST1, 	0x40);
//	retval += SI1133_WriteParameter( PARAM_MEASCONFIG1, 0x40);
 
	retval += SI1133_WriteParameter( PARAM_ADCCONFIG2, 	0x61);
	retval += SI1133_WriteParameter( PARAM_ADCSENS2, 	0x50);
	retval += SI1133_WriteParameter( PARAM_ADCPOST2, 	0x4d);
//	retval += SI1133_WriteParameter( PARAM_MEASCONFIG2, 0x40);
 
	retval += SI1133_WriteParameter( PARAM_ADCCONFIG3, 	0x4d);
	retval += SI1133_WriteParameter( PARAM_ADCSENS3, 	0x07);
	retval += SI1133_WriteParameter( PARAM_ADCPOST3, 	0x40);
//	retval += SI1133_WriteParameter( PARAM_MEASCONFIG3, 0x40);

	retval += SI1133_WriteRegister(  REG_IRQ_ENABLE, 	0x0f);
	return retval;
}

void SI1133_Init(void)
{
	u8 PART_ID;
	SI1133_GPIO_Init();
	
  while(PART_ID !=0x33)
	{
		PART_ID=SI1133_ReadRegister(REG_PART_ID);
		delay_ms(10);
		cntt++;
		if(cntt>50)
		break;
	}
		
  if(cntt>50)
	{
		SI1133_OK=0;
	}
	else
	{
		SI1133_OK=1;
	}

  SI1133_Init_UvAls();
	SI1133_Force();
	
}


void Si1133Handler(Si115xSample_t *samples)
{
    uint8_t buffer[13];
	
	  SI1133_ReadBlock(REG_IRQ_STATUS,buffer,13);
    samples->irq_status = buffer[0];
    samples->ch0  = buffer[1] << 16;
    samples->ch0 |= buffer[2] <<  8;
    samples->ch0 |= buffer[3];
    if( samples->ch0 & 0x800000 ) 		//Sign extending samples->ch0
        samples->ch0 |= 0xFF000000;
    samples->ch1  = buffer[4] << 16;
    samples->ch1 |= buffer[5] <<  8;
    samples->ch1 |= buffer[6];
    if( samples->ch1 & 0x800000 )
        samples->ch1 |= 0xFF000000;
    samples->ch2  = buffer[7] << 16;
    samples->ch2 |= buffer[8] <<  8;
    samples->ch2 |= buffer[9];
    if( samples->ch2 & 0x800000 )
        samples->ch2 |= 0xFF000000;
    samples->ch3  = buffer[10] << 16;
    samples->ch3 |= buffer[11] <<  8;
    samples->ch3 |= buffer[12];
    if( samples->ch3 & 0x800000 )
        samples->ch3 |= 0xFF000000;
}

void getSensorData(void)
{
	// Sensor data ready
	// Process measurement
	Si1133Handler(&samples);
	
	// Start next measurement
	SI1133_Force();
}

//-----------------------------------------------------------------------------
// @brief Generic functions used for UV and ALS (Lux) calculations
//-----------------------------------------------------------------------------
int32_t poly_inner( int32_t input,
               int8_t  fraction,
               uint16_t mag,
               int8_t  shift)
{
  if (shift < 0)
  {
    return ( ( input << fraction ) / mag ) >> -shift ;
  }
  else
  {
    return ( ( input << fraction ) / mag ) << shift  ;
  }
}


/*
 * @brief: Used in the calculation of lux
 *
 */
int32_t eval_poly( int32_t x,
               int32_t y,
               uint8_t input_fraction,
               uint8_t output_fraction,
               uint8_t num_coeff,
               COEFF  *kp
             )
{
  uint8_t  info, x_order, y_order, counter;
  int8_t   sign, shift;
  uint16_t mag;
  int32_t  output=0, x1, x2, y1, y2;

  for(counter=0; counter < num_coeff; counter++)
  {
    info    = kp->info;
    x_order = get_x_order(info);
    y_order = get_y_order(info);

    shift   = ((uint16_t)kp->info&0xff00)>>8;
    shift  ^= 0x00ff;
    shift  += 1;
    shift   = -shift;

    mag     = kp->mag;

    if( get_sign(info) ) sign = -1;
    else                 sign = 1;

    if( (x_order==0) && (y_order==0) )
    {
      output += sign * mag << output_fraction;
    }
    else
    {
      if( x_order > 0 )
      {
        x1 = poly_inner( x, input_fraction, mag, shift);
        if ( x_order > 1 )
        {
          x2 = poly_inner( x, input_fraction, mag, shift);
        }
        else
          x2 = 1;
      }
      else { x1 = 1; x2 = 1; }

      if( y_order > 0 )
      {
        y1 = poly_inner( y, input_fraction, mag, shift);
        if ( y_order > 1 )
        {
          y2 = poly_inner( y, input_fraction, mag, shift);
        }
        else
          y2 = 1;
      }
      else
      { y1 = 1; y2 = 1; }

      output += sign * x1 * x2 * y1 * y2;
    }
    kp++;
  }
  if( output < 0 ) output = -output;
  return output;
}


int32_t get_uv ( int32_t uv,
                 COEFF *uk)
{
    int32_t uvi;

    uvi = eval_poly( 0,
                     uv,
                     UV_INPUT_FRACTION,
                     UV_OUTPUT_FRACTION,
                     UV_NUMCOEFF,
                     uk );
    return uvi;
}


float Si1133_getUVReading( HANDLE si115x_handle,
                        Si115xSample_t *samples )
{
  float  uvi;

  //
  // Conversion to human-readable lux values
  //
  uvi = (float) get_uv( samples->ch0, uk);
  uvi = uvi / ( 1 << UV_OUTPUT_FRACTION );

  return uvi;
}

//
// @brief This is the main entry point for computing lux. The value returned by
// get_lux is scaled by LUX_OUTPUT_FRACTION
//
// In order to get lux as an integer, do this:
//
//   lux = get_lux(vis_high, vis_low, ir, &lk) / ( 1 << LUX_OUTPUT_FRACTION )
//
int32_t get_lux( int32_t vis_high,
                 int32_t vis_low,
                 int32_t ir,
                 LUX_COEFF *lk)
{
  int32_t lux;

  if( (vis_high > ADC_THRESHOLD) || (ir > ADC_THRESHOLD) )
  {
    lux = eval_poly( vis_high,
                     ir,
                     INPUT_FRACTION_HIGH,
                     LUX_OUTPUT_FRACTION,
                     NUMCOEFF_HIGH,
                     &(lk->coeff_high[0]) );
  }
  else
  {
    lux = eval_poly( vis_low,
                     ir,
                     INPUT_FRACTION_LOW,
                     LUX_OUTPUT_FRACTION,
                     NUMCOEFF_LOW,
                     &(lk->coeff_low[0]) );
  }
  return lux;
}

// @brief Returns the lux reading of the si1133 sensor
// General steps:
//
// 1. Initialize the Si1133
//
// 2. Initiate a conversion by using si115x_force()
//
// 3. The interrupt causes the interrupt handler to fill the
//    Si115xSample_t structure
//
// 4. The example_calling_routine picks up data from the
//    Si115xSample_t structure and calls the get_lux()
//    routine to compute the lux
//
float Si1133_getLuxReading( HANDLE si115x_handle,
                        Si115xSample_t *samples )
{
  float lux;

  //
  // Conversion to human-readable lux values
  //
  lux = (float) get_lux( samples->ch1,
                         samples->ch3,
                         samples->ch2,
                         &lk_SI1133_AA00);
  lux = lux / ( 1 << LUX_OUTPUT_FRACTION );

  return lux;
}

