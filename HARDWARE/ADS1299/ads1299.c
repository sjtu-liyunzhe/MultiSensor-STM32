#include "ads1299.h"
#include "EMGProcessing.h"
#include "delay.h"
#include "wifi.h"
#include "usart.h"
#include "led.h"

//FreeRTOS���?
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "cQueue.h"
#include "IMUCollecting.h"

extern SemaphoreHandle_t xSemaphore;
uint8_t EMG_Package[8];
unsigned char mask[]={0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
volatile int ADS1299_Data2[9];
volatile unsigned short EMG_Data2[8];
volatile int16_t EMG_Data_16bit[8];
extern PLN_FILER plnf[4];//�����˲�
extern IIR_FILER iir_filter_array[4];
extern COMB_FILER comb_filter_array[4];
extern uint8_t errorMessage[66];
#define ERROR_PACKAGE_SIZE 66
uint8_t Package_Number=0;//~ �����Ƿ񶪰�
uint8_t sum_emgdata=0;//~ ��У��λ

extern cLinkQueue* EMGLinkQueueArray[4];
extern volatile cCircleQueue* EMGCircleQueueArray[4];

volatile char EMG_24bit[20];
uint8_t EMG_Package_24bit[20];

void SelectChip(int index)
{
	if(index == 2)
		SelectChip_2;
}

void ADS1299Init(void)
{
	//EEG�������ź�
	//START 	PC7
	
	//EMGģ��ads1299���ź�
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
		// ADS1299_WREG(0x05+index_ch,	0x00, 0x65,index);
	}	

	// ADS1299_out=ADS1299_RREG(0x02,index);
	// ADS1299_WREG(0x02, 0x00, (ADS1299_out | 0x10),index);	// �����ź�
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
	int16_t result = 0;
	int lsbshift=2;
	
		if(!(GET_DRDY2))
	{
		ADS1299_READ(2);
		
		for(index_ch=0;	index_ch<4;	index_ch++)
		{
			add_PLF(&plnf[index_ch], ADS1299_Data2[index_ch+1], &ADS1299_Data2[index_ch+1]);//�����˲���
			// COMB_Filter(&comb_filter_array[index_ch], ADS1299_Data2[index_ch+1], &ADS1299_Data2[index_ch+1]);
			// IIR_Filter(&iir_filter_array[index_ch], ADS1299_Data2[index_ch+1], &result);		// butterworth�˲���
			IIR_Filter(&iir_filter_array[index_ch], ADS1299_Data2[index_ch+1], &ADS1299_Data2[index_ch+1]);
			// printf("%f\n", result);
			temp = (ADS1299_Data2[index_ch+1]>>(lsbshift+4)); //�ƶ��ľ���λ����Ҫȷ����12�� lsbshift=8�� Ϊʲô�����м��λ��������ĩβ��������?��Ϊ��������λ
			// IIR_Filter(&iir_filter_array[index_ch], (int16_t)temp, &result);
			// temp = (unsigned short)result;
			temp = (unsigned short)temp;
			if(ADS1299_Data2[index_ch+1]<0)
				temp|= 0x800;
			else
				temp&= 0x7FF;
		
		EMG_Data2[index_ch]= (uint16_t)(temp&0xFFF);
		}
		// �о�����Ĳ��ԣ����水�Լ��������޸�һ�£�?
		// int dataNow;
		// for(index_ch=0;	index_ch<4;	index_ch++)
		// {
		// 	add_PLF(&plnf[index_ch], ADS1299_Data2[index_ch+1], &ADS1299_Data2[index_ch+1]);//�����˲���
		// 	dataNow = ADS1299_Data2[index_ch+1];
		// 	printf("%d\r\n", dataNow);
		// }
		// xSemaphoreTake(xSemaphore, portMAX_DELAY);		// �ȴ�������
		EMGSaveData(EMG_Data2);
		// xSemaphoreGive(xSemaphore);		// �����ź����ͷ�
		// sendPackage(EMG_Data2);
		// delay_ms(2);
	}

}

void dataAcq_16bit(void)
{
	int index_ch;
	int temp;
	int16_t result = 0;
	if(!(GET_DRDY2))
	{
		ADS1299_READ(2);
		
		for(index_ch=0;	index_ch<4;	index_ch++)
		{
			// add_PLF(&plnf[index_ch], ADS1299_Data2[index_ch+1], &ADS1299_Data2[index_ch+1]);//�����˲���
			COMB_Filter(&comb_filter_array[index_ch], ADS1299_Data2[index_ch+1], &ADS1299_Data2[index_ch+1]);
			// IIR_Filter(&iir_filter_array[index_ch], ADS1299_Data2[index_ch+1], &result);		// butterworth�˲���
			// IIR_Filter(&iir_filter_array[index_ch], ADS1299_Data2[index_ch+1], &ADS1299_Data2[index_ch+1]);
			// printf("%f\n", result);
			temp = (ADS1299_Data2[index_ch+1]) >> 4;
			if(ADS1299_Data2[index_ch+1] < 0)
				temp|= 0x8000;
			else
				temp&= 0x7FFF;
			// IIR_Filter(&iir_filter_array[index_ch], (int16_t)temp, &result);
			// temp = (unsigned short)result;
			int16_t tmp_16bit = (int16_t)temp;
		
			EMG_Data_16bit[index_ch]= (int16_t)(tmp_16bit & 0xFFFF);
		}
		// xSemaphoreTake(xSemaphore, portMAX_DELAY);		// �ȴ�������
		EMGSaveData_16bit(EMG_Data_16bit);
		// xSemaphoreGive(xSemaphore);		// �����ź����ͷ�
	}
}

// return ָ�������ָ��?
unsigned short* EMGdataAcq(void)
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
			add_PLF(&plnf[index_ch], ADS1299_Data2[index_ch+1], &ADS1299_Data2[index_ch+1]);//�����˲���
			// ΪʲôҪ����
			temp = (unsigned short)(ADS1299_Data2[index_ch+1]>>(lsbshift+4)); //�ƶ��ľ���λ����Ҫȷ����12�� lsbshift=8�� Ϊʲô�����м��λ��������ĩβ��������?��Ϊ��������λ
			if(ADS1299_Data2[index_ch+1]<0)
				temp|= 0x800;
			else
				temp&= 0x7FF;
		
		EMG_Data2[index_ch]= (uint16_t)(temp&0xFFF);
		}
		xSemaphoreTake(xSemaphore, portMAX_DELAY);		// �ȴ�������
		EMGSaveData(EMG_Data2);
		xSemaphoreGive(xSemaphore);		// �����ź����ͷ�
		sendPackage(EMG_Data2);
		return EMG_Data2;
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

	//~ 4ch EMG������8���ֽ����ݴ����ã���1~4���ֽ�Ϊ�ĸ�ͨ��������
	for(j=0;j<4;j++) 
	{
		*(EMG_Package+j) = emgdata_4ch[j];
	}

	
	*(EMG_Package+4) = sum_emgdata; //~ ��У��
	*(EMG_Package+5) = Package_Number; //~ ���ݰ�����
	*(EMG_Package+6) = 0x0D; //��ͷ
	*(EMG_Package+7) = 0x0A; //��β
	
//	//~ 4ch EMG������16���ֽ����ݴ����ã�ǰ�ĸ��ֽ�Ϊ�ĸ�ͨ��������
//	for(j=0;j<4;j++) 
//	{
//		*(EMG_Package+j) = emgdata_4ch[j];
//	}
//	*(EMG_Package+14) = 0x0D; //��ͷ
//	*(EMG_Package+15) = 0x0A; //��β
	
	//~ ���ݴ��䷽ʽ��WiFi+���ڣ�
//	Wifi_Send_Packet();//~ ʹ��WiFi�������ݣ���ҪԤ�ȶ���WiFi�����EMG_package��С
	USART2_Send_Data(EMG_Package,8);//~ ʹ�ô��ڷ������ݣ��ڶ�������Ϊ��Ҫ������ֽ����?
	
	//~ ͨ���жϰ�����ֵ������֪�����ݴ���������Ƿ���ڶ������⣬���磬���Խ���ֵ����λ���л������Σ�������ֱ�ۿ���Ч��
	Package_Number++;
	if(Package_Number > 256)
	{
		Package_Number=0;
//		LED_G_Toggle;
	}
}

 void USART2_Send_Data(uint8_t* data_buffer,uint16_t bytes_num)
 {
	 
			HAL_UART_Transmit(&UART2_Handler, (uint8_t*)data_buffer, bytes_num, 1000);//~ ����������������������ֽ���?
			while(__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_TC)!=SET);
		 
 }

// int16_t EMGSaveData(unsigned short* EMGdata)
// {
// 	for(int i = 0; i < 4; ++i)
// 	{
// 		if(!pushLinkQueue(EMGLinkQueueArray[i], (int16_t)EMGdata[i]))
// 			printf("EMG Save fails, number:%d  ", i);
// 	}
// 	return EMGLinkQueueArray[0]->front->data;
// }

void initErrorMessage()
{
	errorMessage[0] = 0x11;
	errorMessage[1] = 0x00;
	for(int i = 0; i < 11; ++i)
	{
		errorMessage[5 * i + 2] = 0x65;		// e
		errorMessage[5 * i + 3] = 0x72;		// r
		errorMessage[5 * i + 4] = 0x72;		// r
		errorMessage[5 * i + 5] = 0x6F;		// o
		errorMessage[5 * i + 6] = 0x72;		// r
	}		// index 61
	errorMessage[ERROR_PACKAGE_SIZE - 1] = 0x0F;	// 65
	errorMessage[ERROR_PACKAGE_SIZE - 2] = 0xFF;	// 64
	errorMessage[ERROR_PACKAGE_SIZE - 3] = 0x88;	// 63
}

void EMGSaveData(unsigned short* EMGdata)
{
	for(int i = 0; i < 4; ++i)
	{
		int16_t temp = (EMGdata[i] >> 4) & 0xff;
		if(!pushCircleQueue(EMGCircleQueueArray[i], (int16_t)temp))
		{
			// printf("EMG Save fails, number:%d  ", i);
			usart3_SendPackage(errorMessage, ERROR_PACKAGE_SIZE);
		}
	}
}

void EMGSaveData_16bit(int16_t* EMGdata)
{
	for(int i = 0; i < 4; ++i)
	{
		int16_t temp = EMG_Data_16bit[i] & 0xFFFF;
		if(!pushCircleQueue(EMGCircleQueueArray[i], (int16_t)temp))
		{
			usart3_SendPackage(errorMessage, ERROR_PACKAGE_SIZE);
		}
	}
}

// Ϊ����ʱʹ��sendPackage(),������Ķ��������?��λ��������λ��ʱ�ĳ������?
// void EMGSaveData(unsigned short* EMGdata)
// {
// 	// ֻtest2��
// 	for(int i = 0; i < 4; ++i)
// 	{
// 		int16_t temp = EMGdata[i];
// 		if(!pushCircleQueue(EMGCircleQueueArray[i], (int16_t)temp))
// 			printf("EMG Save fails, number:%d  ", i);
// 	}
// }

// ��Ҫ��λ
void dataAcq_24bit(void)
{
	int index_ch;
//	int index_EMG=0;
	int temp;
	int16_t result = 0;
		if(!(GET_DRDY2))
	{
		ADS1299_READ(2);
		
		for(index_ch=0;	index_ch<4;	index_ch++)
		{
			add_PLF(&plnf[index_ch], ADS1299_Data2[index_ch+1], &ADS1299_Data2[index_ch+1]);//�����˲���
			// COMB_Filter(&comb_filter_array[index_ch], ADS1299_Data2[index_ch+1], &ADS1299_Data2[index_ch+1]);
			// IIR_Filter(&iir_filter_array[index_ch], ADS1299_Data2[index_ch+1], &result);		// butterworth�˲���
			// printf("%f\n", result);
			temp = ADS1299_Data2[index_ch+1];
			EMG_24bit[index_ch * 4]= (temp >> 24) & 0xFF;
			EMG_24bit[index_ch * 4 + 1] = (temp >> 16) & 0xFF;
			EMG_24bit[index_ch * 4 + 2] = (temp >> 8) & 0xFF;
			EMG_24bit[index_ch * 4 + 3] = temp & 0xFF;
		}
		sendPackage_24bit(EMG_24bit);
	}
}
// ����������24λ��������
void sendPackage_24bit(char *theData)
{
	int j;
	char emgdata_4ch[16];
	for( j = 0; j < 16; j++)
	{
		emgdata_4ch[j] = *(theData + j);
		sum_emgdata += emgdata_4ch[j];
	}
	for(j = 0; j < 16; j++) 
	{
		EMG_Package_24bit[j] = emgdata_4ch[j];
	}

	
	EMG_Package_24bit[16] = sum_emgdata; //~ ��У��
	EMG_Package_24bit[17] = Package_Number; //~ ���ݰ�����
	EMG_Package_24bit[18] = 0x0D; //��ͷ
	EMG_Package_24bit[19] = 0x0A; //��β
	
	USART2_Send_Data(EMG_Package_24bit, 20);//~ ʹ�ô��ڷ������ݣ��ڶ�������Ϊ��Ҫ������ֽ����?
	
	//~ ͨ���жϰ�����ֵ������֪�����ݴ���������Ƿ���ڶ������⣬���磬���Խ���ֵ����λ���л������Σ�������ֱ�ۿ���Ч��
	Package_Number++;
	if(Package_Number > 256)
	{
		Package_Number=0;
//		LED_G_Toggle;
	}
}