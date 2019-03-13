/** 
 * @file     BME280.c
 * @brief    温湿压传感器
 */

#include "BME280.h"
#include "delay.h"
#include "MY_MATH.h"
#include "SHT2x.h"
#include <math.h>
#include "stdbool.h"
#include "uart5.h"
#include "string.h"

/*bme280 气压和温度过采样 工作模式*/
#define BME280_PRESSURE_OSR			(BME280_OVERSAMP_8X)
#define BME280_TEMPERATURE_OSR		(BME280_OVERSAMP_16X)
#define BME280_HUMIDITY_OSR         (BME280_OVERSAMP_16X)
#define BME280_MODE					(BME280_PRESSURE_OSR<<2|BME280_TEMPERATURE_OSR<<5|BME280_NORMAL_MODE)

u8 E1[7];

extern u8 BME280_OK;

typedef struct 
{
    u16 dig_T1;                                                                /* calibration T1 data */
    s16 dig_T2;                                                                /* calibration T2 data */
    s16 dig_T3;                                                                /* calibration T3 data */
    u16 dig_P1;                                                                /* calibration P1 data */
    s16 dig_P2;                                                                /* calibration P2 data */
    s16 dig_P3;                                                                /* calibration P3 data */
    s16 dig_P4;                                                                /* calibration P4 data */
    s16 dig_P5;                                                                /* calibration P5 data */
    s16 dig_P6;                                                                /* calibration P6 data */
    s16 dig_P7;                                                                /* calibration P7 data */
    s16 dig_P8;                                                                /* calibration P8 data */
    s16 dig_P9;                                                                /* calibration P9 data */
	u8  dig_H1;                                                                /* calibration H1 data */
	s16 dig_H2;                                                                /* calibration H2 data */
	u8  dig_H3;                                  							   /* calibration H3 data */
	s16 dig_H4;                                                                /* calibration H4 data */
	s16 dig_H5;                                                                /* calibration H5 data */
	u8  dig_H6;                                                                /* calibration H6 data */
  s32 t_fine;                                                                /* calibration t_fine data */
} bme280Calib;

bme280Calib  bme280Cal;

static u8 bme280ID=0;
//static bool isInit=false;
static s32 bme280RawPressure=0;
static s32 bme280RawTemperature=0;
static s32 bme280RawHumidity=0;

//static void presssureFilter(float* in,float* out);
static float bme280PressureToAltitude(float* pressure/*, float* groundPressure, float* groundTemp*/);

//char uart_temp[100];

void BME280_SCL_OUTPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.Pin =  BME280_SCL_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(BME280_I2C_PORT, &GPIO_InitStructure);
}

void BME280_SCL_INPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.Pin =  BME280_SCL_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(BME280_I2C_PORT, &GPIO_InitStructure);
}

void BME280_SDA_OUTPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.Pin =  BME280_SDA_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(BME280_I2C_PORT, &GPIO_InitStructure);
}

void BME280_SDA_INPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.Pin =  BME280_SDA_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(BME280_I2C_PORT, &GPIO_InitStructure);
}

void BME280_I2cStartCondition(void)
{
    BME280_SCL_OUTPUT();
    BME280_SDA_OUTPUT();
    
    BME280_SDA_HIGH();
    BME280_SCL_HIGH();
	  delay_us(4);
    BME280_SDA_LOW(); 
    delay_us(4);	
    BME280_SCL_LOW();   
}

void BME280_I2cStopCondition(void)
{
    BME280_SCL_OUTPUT();
    BME280_SDA_OUTPUT();
    
    BME280_SDA_LOW();
    BME280_SCL_LOW();
    delay_us(4);
    BME280_SCL_HIGH();
    BME280_SDA_HIGH();
    delay_us(4);
}

void BME280_I2cAcknowledge(void)
{
    BME280_SCL_OUTPUT();
    BME280_SDA_OUTPUT();

    BME280_SDA_LOW();
    
    BME280_SCL_HIGH();
    delay_us(80);
    BME280_SCL_LOW();   
    delay_us(80);
}

void BME280_I2cNoAcknowledge(void)
{
    BME280_SCL_OUTPUT();
    BME280_SDA_OUTPUT();

    BME280_SDA_HIGH();
    
    BME280_SCL_HIGH();
    delay_us(80);
    BME280_SCL_LOW();   
    delay_us(80);
}

u8 BME280_I2cReadByte(unsigned char ack)
{
    u8 i, val = 0;


    BME280_SCL_OUTPUT();
    BME280_SDA_OUTPUT();

    BME280_SDA_HIGH();
    
    BME280_SDA_INPUT();
        
    for(i = 0; i < 8; i++)
    {
        val <<= 1; 
        
        BME280_SCL_HIGH();   
        
        if(Bit_SET == BME280_SDA_STATE()) 
        {
            val |= 0x01;
        }
        
        BME280_SCL_LOW();    
    }

    if (!ack)
        BME280_I2cNoAcknowledge();       //发送nACK
    else
        BME280_I2cAcknowledge();    //发送ACK     
    return (val);
}

u8 BME280_I2cWriteByte(u8 byte)
{
    u8 i, ack;


    BME280_SCL_OUTPUT();
    BME280_SDA_OUTPUT();
	  BME280_SCL_LOW();
    
    for(i = 0; i < 8; i++)
    {
        if(byte & 0x80) 
        {
            BME280_SDA_HIGH();
        }
        else 
        {
            BME280_SDA_LOW();
        }
        delay_us(2);
        BME280_SCL_HIGH();
        delay_us(2);
        BME280_SCL_LOW();   
        delay_us(2);
        
        byte <<= 1;
    }

    BME280_SDA_INPUT();
    
    BME280_SCL_HIGH();
    
    if(Bit_SET == BME280_SDA_STATE()) 
    {
        ack = ACK_ERROR;
    }
    else
    {
        ack = ACK_OK;
    }
    
    BME280_SCL_LOW();  

    BME280_SDA_OUTPUT();

    BME280_SDA_HIGH();
    
    return (ack);
}




//从指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 iicDevReadByte(u8 devaddr,u8 addr)
{				  
	u8 temp=0;		  	    																 
	BME280_I2cStartCondition();  
	BME280_I2cWriteByte(devaddr);                                                    //发送器件写命令 	   
	BME280_I2cWriteByte(addr);                                                       //发送低地址	

	BME280_I2cStartCondition();  	 	   
	BME280_I2cWriteByte(devaddr|1);                                                  //发送器件读命令			   
	temp=BME280_I2cReadByte(0);			   
	BME280_I2cStopCondition();                                                                //产生一个停止条件	    
	return temp;
}

//连续读多个字节
//addr:起始地址
//rbuf:读数据缓存
//len:数据长度
void iicDevRead(u8 devaddr,u8 addr,u8 len,u8 *rbuf)
{
	int i=0;
	BME280_I2cStartCondition();  
	BME280_I2cWriteByte(devaddr);  
	BME280_I2cWriteByte(addr);                                                       //地址自增  

	BME280_I2cStartCondition();  	
	BME280_I2cWriteByte(devaddr|1);  	
	for(i=0; i<len; i++)
	{
		if(i==len-1)
		{
			rbuf[i]=BME280_I2cReadByte(0);                                          //最后一个字节不应答
		}
		else
			rbuf[i]=BME280_I2cReadByte(1);
	}
	BME280_I2cStopCondition( );	
}

//读取校正参数
//addr:起始地址
//rbuf:读数据缓存
void BME280_GET_CIL(u8 devaddr,u8 *rbuf)
{
	int i=0;	
//读88-9F	
	BME280_I2cStartCondition();  
	BME280_I2cWriteByte(devaddr);  
	BME280_I2cWriteByte(0X88);     //地址自增  

	BME280_I2cStartCondition();  	
	BME280_I2cWriteByte(devaddr|1);  	
	for(i=0; i<23; i++)
	{			
		rbuf[i]=BME280_I2cReadByte(1);
	}
	rbuf[23]=BME280_I2cReadByte(0);   //最后一个字节不应答
	BME280_I2cStopCondition( );	

//读A1	
	BME280_I2cStartCondition();  
	BME280_I2cWriteByte(devaddr);  
	BME280_I2cWriteByte(0XA1);     //地址自增  

	BME280_I2cStartCondition();  	
	BME280_I2cWriteByte(devaddr|1);  	
	rbuf[24]=BME280_I2cReadByte(0);   //最后一个字节不应答
	BME280_I2cStopCondition( );	

//E1-E7	
	BME280_I2cStartCondition();  
	BME280_I2cWriteByte(devaddr);  
	BME280_I2cWriteByte(0XE1);     //地址自增  

	BME280_I2cStartCondition();  	
	BME280_I2cWriteByte(devaddr|1);  	
	for(i=0; i<6; i++)
	{			
		E1[i]=BME280_I2cReadByte(1);
	}
	E1[6]=BME280_I2cReadByte(0);   //最后一个字节不应答
	BME280_I2cStopCondition( );		

//	for(i=0;i<3;i++)
//	{
//		rbuf[i+25]=E1[i]; //H2,H3
//	}
//	rbuf[28]=(E1[3]<<4)|(E1[4]&0x0f);   //H4_L
//	rbuf[29]=E1[3]>>4;   //H4_H
//	rbuf[30]=(E1[4]>>4)|(E1[5]<<4);   //H5_L
//	rbuf[31]=E1[5]>>4;   //H5_H
//	rbuf[32]=E1[6];   //H6

	bme280Cal.dig_H2=(E1[1]<<8)|(E1[0]);
	bme280Cal.dig_H3=(E1[2]);
  bme280Cal.dig_H4=(E1[3]<<4)|(E1[4]&0x0f);
	bme280Cal.dig_H5=(E1[5]<<4)|(E1[4]>>4);
		
	bme280Cal.dig_H6=(E1[6]);
	
}

//从指定地址写入一个数据
//WriteAddr :写入数据的目的地址    
//DataToWrite:要写入的数据
void iicDevWriteByte(u8 devaddr,u8 addr,u8 data)
{				   	  	    																 
	BME280_I2cStartCondition();  
	BME280_I2cWriteByte(devaddr);                                                    //发送器件写命令 	 	   
	BME280_I2cWriteByte(addr);                                                       //发送低地址 	 										  		   
	BME280_I2cWriteByte(data);                                                       //发送字节							    		    	   
	BME280_I2cStopCondition();		                                                           //产生一个停止条件 	 
}

//连续写多个字节
//addr:起始地址
//wbuf:写数据缓存
//len:数据的长度
void iicDevWrite(u8 devaddr,u8 addr,u8 len,u8 *wbuf)
{
	int i=0;
	BME280_I2cStartCondition();  
	BME280_I2cWriteByte(devaddr);  		
	BME280_I2cWriteByte(addr);  //地址自增	
	for(i=0; i<len; i++)
	{
		BME280_I2cWriteByte(wbuf[i]);  		
	}
	BME280_I2cStopCondition( );	
}

void BME280_Test(void)
{   

}


#define CONST_PF 0.1902630958	                                               //(1/5.25588f) Pressure factor
#define FIX_TEMP 25				                                               // Fixed Temperature. ASL is a function of pressure and temperature, but as the temperature changes so much (blow a little towards the flie and watch it drop 5 degrees) it corrupts the ASL estimates.
								                                               // TLDR: Adjusting for temp changes does more harm than good.
/*
 * Converts pressure to altitude above sea level (ASL) in meters
*/
static float bme280PressureToAltitude(float* pressure/*, float* groundPressure, float* groundTemp*/)
{
    if (*pressure>0)
    {
        return((pow((1015.7f/ *pressure),CONST_PF)-1.0f)*(FIX_TEMP+273.15f))/0.0065f;
    }
    else
    {
        return 0;
    }
}

//#define FILTER_NUM	5
//#define FILTER_A	0.1f

///*限幅平均滤波法*/
//static void presssureFilter(float* in,float* out)
//{	
//	static u8 i=0;
//	static float filter_buf[FILTER_NUM]={0.0};
//	double filter_sum=0.0;
//	u8 cnt=0;	
//	float deta;

//	if(filter_buf[i]==0.0f)
//	{
//		filter_buf[i]=*in;
//		*out=*in;
//		if(++i>=FILTER_NUM)	
//			i=0;
//	} 
//	else 
//	{
//		if(i)
//			deta=*in-filter_buf[i-1];
//		else 
//			deta=*in-filter_buf[FILTER_NUM-1];
//		
//		if(fabs(deta)<FILTER_A)
//		{
//			filter_buf[i]=*in;
//			if(++i>=FILTER_NUM)	
//				i=0;
//		}
//		for(cnt=0;cnt<FILTER_NUM;cnt++)
//		{
//			filter_sum+=filter_buf[cnt];
//		}
//		*out=filter_sum /FILTER_NUM;
//	}
//}



void BME280_CONFIG(void)
{
	iicDevWriteByte(BME280_ADDR,BME280_CTRL_HUM,BME280_HUMIDITY_OSR);
	iicDevWriteByte(BME280_ADDR,BME280_CTRL_MEAS_REG,BME280_MODE);
	iicDevWriteByte(BME280_ADDR,BME280_CONFIG_REG,0XA0);		               /*配置IIR滤波*/	
}

bool bme280Init(void)
{	
    __HAL_RCC_GPIOC_CLK_ENABLE();   //使能GPIOC时钟
	
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin =  BME280_SCL_PIN | BME280_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(BME280_I2C_PORT, &GPIO_InitStruct);

    BME280_SCL_HIGH();
    BME280_SDA_HIGH();

    delay_ms(20);
	
  	bme280ID=iicDevReadByte(BME280_ADDR,BME280_CHIP_ID);	                   /* 读取BME280 ID*/
	
		if(bme280ID==BME280_DEFAULT_CHIP_ID)
		{
			BME280_OK=1;
//			sprintf(uart_temp,"BME280 ID IS: 0x%X\n",bme280ID);
//		  HAL_UART_Transmit(&UART5_Handler,(uint8_t *)uart_temp,strlen(uart_temp),1);
		}
		else
		{
			BME280_OK=0;
			return false;
		}			

    /* 读取校准数据 */
  BME280_GET_CIL(BME280_ADDR,(u8 *)&bme280Cal);
	BME280_CONFIG();
	delay_ms(100);
	
//	printf("BME280 Calibrate Registor Are: \r\n");
//	for(i=0;i<24;i++)
//		printf("Registor %2d: 0x%X\n",i,p[i]);
	
//    isInit=true;
    return true;
}

static void bme280GetPressure(void)
{
	u8 data[BME280_DATA_FRAME_SIZE];

	// read data from sensor
	iicDevRead(BME280_ADDR,BME280_PRESSURE_MSB_REG,BME280_DATA_FRAME_SIZE,data);
	bme280RawPressure=(s32)((((uint32_t)(data[0]))<<12)|(((uint32_t)(data[1]))<<4)|((uint32_t)data[2]>>4));
	bme280RawTemperature=(s32)((((uint32_t)(data[3]))<<12)|(((uint32_t)(data[4]))<<4)|((uint32_t)data[5]>>4));
	bme280RawHumidity=(s32)((((uint32_t)(data[6]))<<8)|(uint32_t)(data[7]));
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of "5123" equals 51.23 DegC
// t_fine carries fine temperature as global value
static s32 bme280CompensateT(s32 adcT)
{
    s32 var1,var2,T;

    var1=((((adcT>>3)-((s32)bme280Cal.dig_T1<<1)))*((s32)bme280Cal.dig_T2))>>11;
    var2=(((((adcT>>4)-((s32)bme280Cal.dig_T1))*((adcT>>4)-((s32)bme280Cal.dig_T1)))>>12)*((s32)bme280Cal.dig_T3))>>14;
    bme280Cal.t_fine=var1+var2;
	
    T=(bme280Cal.t_fine*5+128)>>8;

    return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of "24674867" represents 24674867/256 = 96386.2 Pa = 963.862 hPa
static uint32_t bme280CompensateP(s32 adcP)
{
    int64_t var1,var2,p;
    var1=((int64_t)bme280Cal.t_fine)-128000;
    var2=var1*var1*(int64_t)bme280Cal.dig_P6;
    var2=var2+((var1*(int64_t)bme280Cal.dig_P5)<<17);
    var2=var2+(((int64_t)bme280Cal.dig_P4)<<35);
    var1=((var1*var1*(int64_t)bme280Cal.dig_P3)>>8)+((var1*(int64_t)bme280Cal.dig_P2)<<12);
    var1=(((((int64_t)1)<<47)+var1))*((int64_t)bme280Cal.dig_P1)>>33;
    if (var1==0)
        return 0;
    p=1048576-adcP;
    p=(((p<<31)-var2)*3125)/var1;
    var1=(((int64_t)bme280Cal.dig_P9)*(p>>13)*(p>>13))>>25;
    var2=(((int64_t)bme280Cal.dig_P8)*p)>>19;
    p=((p+var1+var2)>>8)+(((int64_t)bme280Cal.dig_P7)<<4);
    return(uint32_t)p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer bits and 10 fractional bits).
// Output value of "47445" represents 47445/1024 = 46.333 %RH
static uint32_t bme280CompensateH(s32 adcH)
{
	s32 var;
	var=(bme280Cal.t_fine-((s32)76800));
	var=(((((adcH<<14)-(((s32)bme280Cal.dig_H4)<<20)-(((s32)bme280Cal.dig_H5)*var))+((s32)16384))>>15)*(((((((var*(
		(s32)bme280Cal.dig_H6))>>10)*(((var*((s32)bme280Cal.dig_H3))>>11)+((s32)32768)))>>10)+((s32)2097152))*((s32)
		bme280Cal.dig_H2)+8192)>>14));
	var=(var-(((((var>>15)*(var>>15))>>7)*((s32)bme280Cal.dig_H1))>>4));
	var=(var<0?0:var);
	var=(var>419430400?419430400:var);
	return (uint32_t)(var>>12);
}

void bme280GetData(float* pressure,float* temperature,float* humidity,float* asl)
{
    static float t;
    static float p;
	static float h;
	
	bme280GetPressure();

	t=bme280CompensateT(bme280RawTemperature)/100.0;		
	p=bme280CompensateP(bme280RawPressure)/25600.0;	
	h=bme280CompensateH(bme280RawHumidity)/1024.0;  // bme280RawHumidity/1024.0;

  *pressure=(float)p;
//	presssureFilter(&p,pressure);
	*temperature=(float)t;                                                     /*单位度*/
//	*pressure=(float)p ;	                                                   /*单位hPa*/	
	*humidity=(float)h;
	
	*asl=bme280PressureToAltitude(pressure);	                               /*转换成海拔*/	

  BME280_CONFIG();	
	
}
