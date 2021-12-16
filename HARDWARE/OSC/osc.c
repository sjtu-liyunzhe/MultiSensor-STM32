#include "osc.h"
#include "delay.h"

//For LTC6903
//OSC_SEN	PA0		Output Push Pull Mode
//OSC_SCK	PA1		Output Push Pull Mode
//OSC_SDI	PA2		Output Push Pull Mode
//OSC_OE	PH2		Output Push Pull Mode
//GATE		PH3		Output Push Pull Mode

/******************************************************************
 * @Function	OSC_IO_Init
 * @Brief		Initialize the IO port of Oscillator and AND_Gate control
 * @Parameter	None
 ******************************************************************/
void OSC_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();     	//GPIOA clock    
	__HAL_RCC_GPIOH_CLK_ENABLE();  		//GPIOH clock

	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;	//PA0,1,2
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  //Output push pull mode
	GPIO_InitStructure.Pull = GPIO_NOPULL;          //No pull
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;     //High speed
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);		//PA0,1,2
	
	GPIO_InitStructure.Pin = GPIO_PIN_2 | GPIO_PIN_3;	//PH2,3
	HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);			//PH2,3
}


/******************************************************************
 * @Function	Oscillator_Init
 * @Brief		Initialize oscillator LTC6903
 * @Parameter	None
 ******************************************************************/
void Oscillator_Init(void)
{
	OSC_SDI(1);		//Why is 1 ??????????
	OSC_SCK(1);		//Write data at rising edge
	OSC_SEN_(1);	//Close SPI interface
	OSC_OE(1);		//Disable CLK output. OE=LOW, CLK and /CLK are LOW. OE=HIGH, both CLK output
}


/**************************************************************************
 * @Function	Write_Oscillator_20MHz
 * @Brief		Use SPI interface to set the frequency of LTC6903 to 20MHz
 * @Parameter	None
 *************************************************************************/
/*Description of serial port register*/
//bit 0-1: CNF0 and CNF1.  CNF1=1 and CNF0=0 for CLK on and /CLK off.
//bit 2-11: DAC0-9
//bit 12-15: OCT0-3
//The serial data transfer starts with MSB and ends with LSB
//For 20MHz output: OCT = 14, DAC = 305 or 300, ¬ CNF1=?, CNF0=? (Ô­±¾µÄÊÇ00£¿£¿£¿)
void Write_Oscillator_20MHz(void)		
{
	u16 osc_tmp = 0;
	u16 osc_val = 0;		//Store the value that is written to serial port register 
	u16 osc_bkp;			//To backup osc_val
	u16 osc_point = 0x8000;	//Pointer for judging the value of each bit in osc_val. 0x8000 = 0b 1000 0000 0000 0000
	u8 osc_i = 0;			//Control the loop in Oscillator writing

	osc_tmp = 14;			//OCT = 14 for 20MHz
//	osc_tmp = 15;			//OCT = 15 for 40MHz
	osc_tmp <<= 12;			//bit 12-15 for OCT
	osc_val = osc_tmp;

	osc_tmp = 300;			//DAC = 305 or 300, according to the actually measured OSC frequency
//	osc_tmp = 305;			//DAC = 305 for 40MHz
	osc_tmp <<= 2;			//bit 2-11 for DAC
	osc_val |= osc_tmp;		//set OCT and DAC, and use CNF1=CNF2=0
	osc_bkp = osc_val;		//Back up the value of osc_val
	
	OSC_SEN_(0);			//Open SPI interface
	delay_us(250);			
	OSC_SCK(0);				//Write data at rising edge
	delay_us(250);
	
	for(osc_i = 0; osc_i < 16; osc_i++)
	{
		//The serial data transfer starts with MSB and ends with LSB
		osc_val = osc_bkp;			//Set osc_val before each judgement
		if(osc_val &= osc_point)	//This operation will change the value of osc_val, so osc_bkp is needed !
		{
			OSC_SDI(1);		//Write 1
		}
		else
		{
			OSC_SDI(0);		//Write 0
		}
		osc_point >>= 1;	//The pointer right shift 1 bit for judging next bit of osc_val
		delay_us(125);
		OSC_SCK(1);			//Write data at rising edge
		delay_us(125);
		OSC_SCK(0);
		delay_us(125);
	}
	OSC_SEN_(1);			//Close SPI interface	
}

