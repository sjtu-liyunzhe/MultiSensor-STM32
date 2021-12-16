#include "ads1299.h"
#include "EMGProcessing.h"
#include "delay.h"
#include "wifi.h"
#include "usart.h"
#include "led.h"

//FreeRTOS相关
#include "FreeRTOS.h"
#include "task.h"


uint8_t EMG_Package[8];
unsigned char mask[]={0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
int ADS1299_Data2[9];
unsigned short EMG_Data2[8];
extern PLN_FILER plnf[4];//梳子滤波

uint8_t Package_Number=0;//~ 测试是否丢包
uint8_t sum_emgdata=0;//~ 和校验位

void SelectChip(int index)
{
	if(index == 2)
		SelectChip_2;
}

void ADS1299Init(void)
{
	//EEG共有引脚号
	//START 	PC7
	
	//EMG模块ads1299引脚号
	//CS2			output	PC9
	//CLK2		output	PA9
	//DIN2		output	PA11
	//DOUT2		input		PC8
	//DRDY2		input		PA8
	//RESET2	output	PA10
	//PWDN2		output	PA12
	
	GPIO_InitTypeDef	GPIO_Initure;
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	
	GPIO_Initure.Pin=GPIO_PIN_7;
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull=GPIO_NOPULL;
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);	//START
	
	GPIO_Initure.Pin=GPIO_PIN_9;
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull=GPIO_NOPULL;
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_9;
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull=GPIO_NOPULL;
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_11;
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull=GPIO_NOPULL;
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_8;
	GPIO_Initure.Mode=GPIO_MODE_INPUT;
	GPIO_Initure.Pull=GPIO_PULLUP;
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);  //DOUT
	
	GPIO_Initure.Pin=GPIO_PIN_8;
	GPIO_Initure.Mode=GPIO_MODE_INPUT;
	GPIO_Initure.Pull=GPIO_PULLUP;
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	//EMG PINS ENABLE  DRDY
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_SET); //RESET
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET); //PWDN
		
}

void setDev(unsigned char rate, unsigned char sleev_flag,	int index)
{
//	000: fMOD / 64 (16 kSPS) 
//	001: fMOD / 128 (8 kSPS) 
//	010: fMOD / 256 (4 kSPS) 
//	011: fMOD / 512 (2 kSPS) 
//	100: fMOD / 1024 (1 kSPS) 
//	101: fMOD / 2048 (500 SPS) 
//	110: fMOD / 4096 (250 SPS)
	int index_ch;
	unsigned char ADS1299_out;
	delay_ms(1);
//	delay(5);
	
	SelectChip(index);
	delay_ms(10);
//	delay(50);
	
	
	ADS1299_SDATAC(index);
	
	ADS1299_out=ADS1299_RREG(0x23,index);
	ADS1299_WREG(0x43,0x00,ADS1299_out|0x8C,index);
	
	ADS1299_out=ADS1299_RREG(0x21,index);
	ADS1299_WREG(0x41,0x00,(ADS1299_out&0xF8)|0x20|rate,index);//0x20, enable clock ouput 0x04 sampling at 1000hz
	
	ADS1299_out=ADS1299_RREG(0x37,index);
	ADS1299_WREG(0x57,0x00,ADS1299_out&0x08,index);
	
	ADS1299_out=ADS1299_RREG(0x35,index);
	ADS1299_WREG(0x55,0x00,sleev_flag<<5,index);
	ADS1299_out=ADS1299_RREG(0x35,index);
	
	for(index_ch=0;	index_ch<4;	index_ch++)
	{
		ADS1299_out=ADS1299_RREG(0x25+index_ch,index);
		ADS1299_WREG(0x45+index_ch,	0x00,	(ADS1299_out&0xF8)|0,index);
	}	
	
}

void ADS1299_SDATAC(int	index)
{
	int counter_bit;

	if(	index	==	2)
	{
		for(counter_bit=7; counter_bit>=0; counter_bit--)
		{
			SCLK2_HIGH;
			if((0x11	&	mask[counter_bit]) >> counter_bit)
				ADS2_DIN_HIGH;
			else
				ADS2_DIN_LOW;
		
			delay_us(1);
//			SPIdelay(1);
			SCLK2_LOW;
			delay_us(1);
//			SPIdelay(1);
		}
	}
	
}

void ADS1299_RDATAC(int index)		
{
	int counter_bit;
	
	if(index	==	2)
	{
		for(counter_bit=7;counter_bit>=0;counter_bit--)
		{
			SCLK2_HIGH;
			if((0x10	&	mask[counter_bit])>>counter_bit)
				ADS2_DIN_HIGH;
			else
				ADS2_DIN_LOW;

			delay_us(1);
//			SPIdelay(1);
			SCLK2_LOW;
			delay_us(1);
//			SPIdelay(1);
		}
	}
}
	
void ADS1299_START_PIN(void)
{
	START_HIGH;
	delay_us(20);
//	SPIdelay(20);
	START_LOW;
	delay_us(20);
//	SPIdelay(20);
	START_HIGH;
}

unsigned char ADS1299_RREG(unsigned char address_register, int index)
{
	int	counter_bit;
	unsigned char out;
	
	
	if(index==2)
	{
			for(counter_bit=7;	counter_bit>=0;	counter_bit--)
		{
			SCLK2_HIGH;
			delay_us(1);
//			SPIdelay(1);
			if( (address_register & mask[counter_bit])>>counter_bit)
				ADS2_DIN_HIGH;
			else
				ADS2_DIN_LOW;

			delay_us(1);
//			SPIdelay(1);
			SCLK2_LOW;
			delay_us(1);
//			SPIdelay(1);
		}
	
			delay_us(400);
//		SPIdelay(400);
		
		for(counter_bit=7;	counter_bit>=0;	counter_bit--)
		{
			SCLK2_HIGH;
			ADS2_DIN_LOW;
			delay_us(1);
//			SPIdelay(1);
			SCLK2_LOW;
			delay_us(1);
//			SPIdelay(1);
		}

			delay_us(1);
//		SPIdelay(400);
		out=0;
		for(counter_bit=7;	counter_bit>=0;	counter_bit--)
		{
			SCLK2_HIGH;
			delay_us(1);
//			SPIdelay(1);
			if(GET_DOUT2)
				out|=	1	<< counter_bit;
			
			SCLK2_LOW;
			delay_us(1);
//			SPIdelay(1);
		}	

	}
	return out;
}

void ADS1299_WREG(unsigned char address_register, unsigned char number_Reg_toWrite,	unsigned char parameter_toWrite, int index)
{
	int counter_bit;

	
	if(index==2)
	{
			for(counter_bit=7;	counter_bit>=0;	counter_bit--)
		{
			SCLK2_HIGH;
			delay_us(1);
//			SPIdelay(1);
			if( (address_register & mask[counter_bit])>>counter_bit)
				ADS2_DIN_HIGH;
			else
				ADS2_DIN_LOW;

			delay_us(1);
//			SPIdelay(1);
			SCLK2_LOW;
			delay_us(1);
//			SPIdelay(1);
		}

			delay_us(400);
//		SPIdelay(400);
	
		for(counter_bit=7;	counter_bit>=0;	counter_bit--)
			{
				SCLK2_HIGH;
				delay_us(1);
//				SPIdelay(1);
				if( (0x00 & mask[counter_bit])>>counter_bit)
					ADS2_DIN_HIGH;
				else
					ADS2_DIN_LOW;

				delay_us(1);
//				SPIdelay(1);
				SCLK2_LOW;
				delay_us(1);
//				SPIdelay(1);
			}

			delay_us(400);
//			SPIdelay(400);
		
		for(counter_bit=7;	counter_bit>=0;	counter_bit--)
			{
				SCLK2_HIGH;
				delay_us(1);
//				SPIdelay(1);
				if( (parameter_toWrite & mask[counter_bit])>>counter_bit)
					ADS2_DIN_HIGH;
				else
					ADS2_DIN_LOW;

				delay_us(1);
//				SPIdelay(1);
				SCLK2_LOW;
				delay_us(1);
//				SPIdelay(1);
			}
	}
}

void stopDev(int index)
{
	
	if(index==2)
	{
		SelectChip_2;
		delay_ms(1);
//		delay(50);
		
		ADS1299_SDATAC(index);
	}
}
		
void startDev(int index)
{
	
	if(index==2)
	{
		SelectChip_2;
		delay_ms(1);
//		delay(5);
		ADS1299_RDATAC(2);
		ADS1299_START_PIN();
	}
}

void dataAcq(void)
{
	int index_ch;
//	int index_EMG=0;
	unsigned int temp;
	int lsbshift=2;
	
		if(!(GET_DRDY2))
	{
		ADS1299_READ(2);
		
		for(index_ch=0;	index_ch<4;	index_ch++)
		{
			add_PLF(&plnf[index_ch], ADS1299_Data2[index_ch+1], &ADS1299_Data2[index_ch+1]);//梳子滤波器
			temp = (unsigned short)(ADS1299_Data2[index_ch+1]>>(lsbshift+4)); //移动的具体位数需要确定，12？ lsbshift=8？ 为什么保留中间的位数，舍弃末尾可以理解，但为何舍弃高位
			if(ADS1299_Data2[index_ch+1]<0)
				temp|= 0x800;
			else
				temp&= 0x7FF;
		
		EMG_Data2[index_ch]= (uint16_t)(temp&0xFFF);
		}
		
	
		sendPackage(EMG_Data2);
	}

}


void ADS1299_READ(int index)
{
	int index_ch,index_bit;
	
	if(index==2)
	{
		ADS2_DIN_LOW;
		for(index_ch=0; index_ch<5; index_ch++)
		{
			ADS1299_Data2[index_ch]=0;
			for(index_bit=23; index_bit>=0;	index_bit--)
			{
				SCLK2_HIGH;
				ADS1299_Data2[index_ch]|= (GET_DOUT2) <<index_bit;
				SCLK2_LOW;
			}
			
		if(ADS1299_Data2[index_ch]&0x00800000)
		{
			ADS1299_Data2[index_ch] = ADS1299_Data2[index_ch]|0xFF000000;
		}
		}
	}
}
				
void sendPackage(unsigned short *theData)
{

	int j;
	char emgdata_4ch[4];
	for( j=0;j<4;j++)
	{
		emgdata_4ch[j] = (*(theData+j)>>4)&0xff;
		sum_emgdata += emgdata_4ch[j];
	}

	//~ 4ch EMG，测试8个字节数据传输用，第1~4个字节为四个通道的数据
	for(j=0;j<4;j++) 
	{
		*(EMG_Package+j) = emgdata_4ch[j];
	}

	
	*(EMG_Package+4) = sum_emgdata; //~ 和校验
	*(EMG_Package+5) = Package_Number; //~ 数据包计数
	*(EMG_Package+6) = 0x0D; //包头
	*(EMG_Package+7) = 0x0A; //包尾
	
//	//~ 4ch EMG，测试16个字节数据传输用，前四个字节为四个通道的数据
//	for(j=0;j<4;j++) 
//	{
//		*(EMG_Package+j) = emgdata_4ch[j];
//	}
//	*(EMG_Package+14) = 0x0D; //包头
//	*(EMG_Package+15) = 0x0A; //包尾
	
	//~ 数据传输方式（WiFi+串口）
//	Wifi_Send_Packet();//~ 使用WiFi传输数据，需要预先定义WiFi传输的EMG_package大小
	USART2_Send_Data(EMG_Package,8);//~ 使用串口发送数据，第二个参数为需要传输的字节数目
	
	//~ 通过判断包的数值，可以知道数据传输过程中是否存在丢包问题，例如，可以将数值在上位机中画出波形，更容易直观看出效果
	Package_Number++;
	if(Package_Number > 256)
	{
		Package_Number=0;
//		LED_G_Toggle;
	}
}

 void USART2_Send_Data(uint8_t* data_buffer,uint16_t bytes_num)
 {
	 
			HAL_UART_Transmit(&UART2_Handler, (uint8_t*)data_buffer, bytes_num, 1000);//~ 第三个参数代表传输的总字节数
			while(__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_TC)!=SET);
		 
 }

	

	
