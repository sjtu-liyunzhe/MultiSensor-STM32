#include "bf.h"
#include "delay.h"
//#include "binary.h"

//BF_TX_EN		PB3		Output Push Pull Mode
//BF_RST		PD7		Output Push Pull Mode
//BF_SRD		PD5		Input  Floating  Mode
//BF_SLE		PD4 	Output Push Pull Mode
//BF_SWR		PD3 	Output Push Pull Mode
//BF_SCLK		PD2 	Output Push Pull Mode

/****************************************************************************
 * @Function	BF_IO_Init
 * @Brief		Initialize the IO pin for Beamformer control
 * @Parameter	None
 ****************************************************************************/
void BF_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();           //GPIOB clock
	__HAL_RCC_GPIOD_CLK_ENABLE();           //GPIOD clock

	GPIO_InitStructure.Pin = GPIO_PIN_3; 	//PB3
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  	//Output push pull mode
	GPIO_InitStructure.Pull = GPIO_NOPULL;          	//No pull
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;     	//High speed
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);			//PB3
	
	GPIO_InitStructure.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_7;	//PD2,3,4,7
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);			
			
	GPIO_InitStructure.Pin = GPIO_PIN_5; 				//PD5
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;			//Input
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);			//PD5
	
	BF_RST(0);		//BF_RST low	
	BF_SLE(1);		//BF_SLE high to close 4-wire serial interface
	BF_STOP;		//Keep TX_EN low before setting Beamformer
}


/**********************************************************************************************
 * @Function	BF_Write
 * @Brief		Write data to Beamformer's registers through 4-wire serial interface operation
 * @Parameter	the length of the data to be written; register address; data to be written
 **********************************************************************************************/
//3 steps:	1. chose register	2. write or read	3. write/read data
void BF_Write(u8 Data_LEN, u8 Register_Add, long long BF_Data)	//u8 or u16 ??????
{
	u8	Reg_Add_Point = 0x01;						//Pointer for judging the value of each bit in Register_Add, start from bit 0, the maximal register address is 5 bits
	long long	BF_Data_Point = 0x0000000000000001;	//Pointer for judging the value of each bit in BF_Data, start from bit 0, the maximal data size is 64 bits
	u8	Reg_Add_Temp = 0;								
	long long	BF_Data_Temp = 0;
	u16	BF_i;		//Control the loop in Beamformer writing
	
	BF_SCLK(0);		//Pull down SCLK to start writing
	QUANTA_US;
	BF_STOP;		//TX_EN must be inactiver during 4-wire serial interface operation
	QUANTA_US;
	BF_SLE(0);		//Pull down SLE to open 4-wire serial interface. 0 for open, 1 for close
	delay_us(500);
	BF_SWR(0);		//Initialize SWR to low level
	
	/* Description for BF_Data: */
	//bit 0-4:	register address, the highest register is 1Bh(0b0001 1011), so only 5 bits are used for register address
	//bit 5:	indicate the operation, 0: write data to BF through SWR; 1: read data from BF through SRD
	//the rest bits are data bits, which should be written into the corresponding register
	//note: data stream starts with the LSB and ends with the MSB
	
	for (BF_i = 0; BF_i < (Data_LEN+6); BF_i++)		//Total length of BF_Data is Data_LEN+6
	{
		Reg_Add_Temp = Register_Add;
		Reg_Add_Temp &= 0xDF;			//0xDF = 0b1101 1111, set bit 5(the 6th bit) to 0 for writing
		BF_Data_Temp = BF_Data;
		
		if (BF_i < 6)					//For bit 0 to bit 5
		{
			if (Reg_Add_Temp &= Reg_Add_Point)	
				BF_SWR(1);				//If the i bit of register address is 1, then write 1
			else
				BF_SWR(0);				//If the i bit of register address is 0, then write 0
			Reg_Add_Point <<= 1;		//The pointer left shift 1 bit, to judge the value of next bit
		}
		else							//For the rest bits--data bits
		{
			if (BF_Data_Temp &= BF_Data_Point)
				BF_SWR(1);				//If the i bit of data is 1, then write 1
			else
				BF_SWR(0);				//If the i bit of data is 0, then write 0
			BF_Data_Point <<= 1;		//The pointer left shift 1 bit, to judge the value of next bit
		}
		QUANTA_US;						//To ensure that the data can be written to Beamformer
		BF_SCLK(1);						//The data is written at the rising edge of SCLK
		QUANTA_US;
		BF_SCLK(0);						//For next data writing
		QUANTA_US;
	}
	BF_SLE(1);							//Pull up SLE to close 4-wire serial interface. 0 for open, 1 for close
	delay_us(500);	
}


/******************************************************************
 * @Function	BF_Init
 * @Brief		Initialize the Beamformer's setting //~关于beamformer芯片的参数设置，包括几个脉冲波形，脉冲频率等
 * @Parameter	None
 ******************************************************************/
void BF_Init(void)
{
	char Add_i = 0;		//Store the register address 00h-07h
	
	BF_SLE(0);			//Pull down SLE to open 4-wire serial interface. 0 for open, 1 for close
	delay_us(500);
	BF_RST(1);			//Reset Beamformer 复位
	delay_ms(10);
	BF_RST (0);							
	delay_ms(100);
	
	//~重要！！！用于设置beamformer的方波频率和方波的个数，适用于不同的超声探头和激励方式，具体修改数据可查LM96570的datasheet；
	
	//1. Write register 1Ah to set the frequency and pulse number(pulse length)  
	/* Description of 1Ah */
	//bit 0-2:	Pulse length(000: 4, 001: 8, 010: 12, ..., 111: 64) pulse的个数设置，一次性产生多少个pulse
	//bit 3-9:	Frequency division(FD分频系数), freq of one pulse = 160M/FD（单个脉冲的频率，用于理解，非设置）； freq of a one-high-one-low pattern = 160M/(FD*2), 0010000 for 5MHz,100 0000 for 1.25MHz（一高一低脉冲的频率，这个才是需要设置的频率）
	//bit 10:	0 for PPL disabled, 1 for PPL enable, here enable
	//bit 11:	Invert Fire Enable, 0 for disable, 1 for enable, here disable
	//bit 12:	Continuous Wave, 0 for disable, 1 for enable, here disable
	//bit 13:	when writing to 1Ah, keep this bit at 0
	
//	BF_Write(14, 0x1A, 0x0480);	//用于5MHz的超声探头，0x0480 = 0b00 0100 1000 0000, 4 pulses, 5MHz, PPL enable, Invert Fire disable, Continuous Wave disable
	BF_Write(14, 0x1A, 0x0600);	//用于1MHz的超声探头，0x0600 = 0b00 0110 0000 0000, 4 pulses, 1.25MHz, PPL enable, Invert Fire disable, Continuous Wave disable
	
	delay_us(500);
	
	//2. Write register 00h-07h to set the delay-between-channel and Pulse Width adjustment for channels 0 to 7, respectively 
	/* Description of 00h-07h */
	//bit 0-2:		Fine Delay Adjust, control the fine delay of P/N relative to TX_EN, here use 000 for 0°s
	//bit 3-16:		Coarse Delay Adjust, control the coarse delay of P/N relative to TX_EN, here use 00000000000000 for no delay
	//bit 17-19:	Pulse Width Adjust, here use 000 for no phase delay
	//bit 20-21:	Pulse Width Adjust Enable, (00: disable, 01: N is delayed relative to P, 10: P is delayed relative to N, 11: disable), here use 00 for disable

	for (Add_i = 0; Add_i < 8; Add_i++)
	{
		BF_Write(22, Add_i, 0x000000);	//0x000000=0b00 0000 0000 0000 0000 0000, no fine and coarse delay, no pulse width adjust, no delay between channels and P/N
		delay_us(500);
	}
	
	//3. Write register 18h and 19h to set the pulse value(pulse pattern) of all 8 channels, 18h for P, 19h for N
	BF_Write(4, 0x18, 0xA);		//0xA = 0b1010, P = 1010
	delay_us(500);
	BF_Write(4, 0x19, 0x5);		//0x5 = 0b0101, N = 0101
	delay_us(500);
	
	BF_SLE(1);					//Pull up SLE to close 4-wire serial interface. 0 for open, 1 for close
	delay_us(500);
}
