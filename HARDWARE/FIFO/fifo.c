#include "fifo.h"
#include "delay.h"

//For AL422B
//FIFO DO Port:
//FIFO_D0	PE7		Input  Floating  Mode
//FIFO_D1	PE8		Input  Floating  Mode
//FIFO_D2	PE9		Input  Floating  Mode
//FIFO_D3	PE10	Input  Floating  Mode
//FIFO_D4	PE15	Input  Floating  Mode
//FIFO_D5	PB10	Input  Floating  Mode
//FIFO_D6	PB11	Input  Floating  Mode
//FIFO_D7	PH6		Input  Floating  Mode

//FIFO Control Port:
//FIFO_RE_		PE11	Output Push Pull Mode
//FIFO_OE_		PE12	Output Push Pull Mode
//FIFO_RRST_	PE13	Output Push Pull Mode
//FIFO_RCK		PE14	Output Push Pull Mode
//FIFO_WE_		PH7		Output Push Pull Mode
//FIFO_WRST_	PH8		Output Push Pull Mode

u8 FIFO_Buffer[1000];	//Buffer for storing data read from FIFO

/******************************************************************
 * @Function	delay_kns
 * @Brief		Delay 0.x us
 * @Parameter	tick_num: The ticks number of SysTick
 ******************************************************************/
 /*Description of tick_num*/
 //ÀíÂÛÉÏSysTickµÄÆµÂÊÎªHCLK216MHz£¬µ«ÊÇÔÚ¸Ãº¯ÊýÖÐ£¬tick_numÉèÎª1-15×óÓÒ£¬Êµ²âÑÝÑÓÊ±0.34us£¬¿ÉÄÜÊÇdebug²âÁ¿ÓÐÑÓÊ±µ¼ÖÂ¾«¶ÈÖ»ÄÜµ½´ï0.3us×óÓÒ
 //tick_num£º	15=0.34us	30=0.43us	40=0.47us	50=0.51us	60=0.55us	80=0.63us	90=0.68us	100=0.76us
void delay_kTicks(u16 tick_num)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOADµÄÖµ	    	 
	ticks=tick_num; 						//ÐèÒªµÄ½ÚÅÄÊý 
	told=SysTick->VAL;        				//¸Õ½øÈëÊ±µÄ¼ÆÊýÆ÷Öµ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//ÕâÀï×¢ÒâÒ»ÏÂSYSTICKÊÇÒ»¸öµÝ¼õµÄ¼ÆÊýÆ÷¾Í¿ÉÒÔÁË.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//Ê±¼ä³¬¹ý/µÈÓÚÒªÑÓ³ÙµÄÊ±¼ä,ÔòÍË³ö.
		}  
	};
}

/******************************************************************
 * @Function	FIFO_IO_Init
 * @Brief		Initialize the IO port of FIFO AL422B
 * @Parameter	None
 ******************************************************************/
void FIFO_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOE_CLK_ENABLE();     	//GPIOE clock 
	__HAL_RCC_GPIOB_CLK_ENABLE();     	//GPIOB clock   	
	__HAL_RCC_GPIOH_CLK_ENABLE();  		//GPIOH clock

	//FIFO DO Port:
	GPIO_InitStructure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_15;	//PE7,8,9,10,15
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;  		//Input floating mode
	GPIO_InitStructure.Pull = GPIO_NOPULL;          	//No pull
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;     	//High speed
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);			//PE7,8,9,10,15
	
	GPIO_InitStructure.Pin = GPIO_PIN_10 | GPIO_PIN_11;	//PB10,11
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);			//PB10,11
	
	GPIO_InitStructure.Pin = GPIO_PIN_6;				//PH6
	HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);			//PH6
	
	//FIFO Control Port:
	GPIO_InitStructure.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;	//PE11,12,13,14
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  	//Output push pull mode
	GPIO_InitStructure.Pull = GPIO_NOPULL;          	//No pull
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;     	//High speed
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);			//PE11,12,13,14
	
	GPIO_InitStructure.Pin = GPIO_PIN_7 | GPIO_PIN_8;	//PH7,8
	HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);			//PH7,8	
}

/***************************************************************************
 * @Function	FIFO_Init
 * @Brief		Initialize FIFO AL422B
 * @Parameter	Close read and write operation. Reset read and write address
 ***************************************************************************/
 
 void FIFO_Init(void)
{
	FIFO_RRST_(1);					//Disable read address reset
	FIFO_RCK(0);					//Read data on rising edge of RCK
	FIFO_OE_(0);					//Enable data output
	FIFO_RE_(1);					//Disable data-reading and read address change
	FIFO_WE_(1);					//Disable data-writing
	FIFO_WRST_(1);					//Disable write address reset
	FIFO_Reset_Pointer();			//Reset read address to 0
	FIFO_Reset_Write_Pointer();		//Reset write address to 0
}

/******************************************************************
 * @Function	FIFO_Reset_Pointer
 * @Brief		Reset read address
 * @Parameter	Nones
 ******************************************************************/
void FIFO_Reset_Pointer(void)
{
	FIFO_RRST_(1);
	FIFO_RCK(1);
	delay_us(1);		//Need to delay 0.4us, corresponding to 8 beats of 20MHz clock	ÐèÒªÖØÐÂÐÞ¸Ä
//	delay_kTicks(30);	//Delay 0.43us
	FIFO_RRST_(0);		//Reset write address to 0
	
	FIFO_RCK(0);
	delay_us(1);		//Need to delay 0.4us, corresponding to 8 beats of 20MHz clock	ÐèÒªÖØÐÂÐÞ¸
//	delay_kTicks(30);	//Delay 0.43us
	FIFO_RCK(1);		//Reset read address on rising edge
	delay_us(1);		//Need to delay 0.4us, corresponding to 8 beats of 20MHz clock	ÐèÒªÖØÐÂÐÞ¸
//	delay_kTicks(30);	//Delay 0.43us
	
	FIFO_RCK(0);
	delay_us(1);		//Need to delay 0.4us, corresponding to 8 beats of 20MHz clock	ÐèÒªÖØÐÂÐÞ¸
//	delay_kTicks(30);	//Delay 0.43us
	FIFO_RCK(1);		//There were 2 rising edges to ensure reset read address definitely	¿ÉÄÜ¿ÉÒÔÉ¾¼õÒ»´Î
	delay_us(1);		//Need to delay 0.4us, corresponding to 8 beats of 20MHz clock	ÐèÒªÖØÐÂÐÞ¸
//	delay_kTicks(30);	//Delay 0.43us
	
	FIFO_RCK(0);
	delay_us(1);		//Need to delay 0.4us, corresponding to 8 beats of 20MHz clock	ÐèÒªÖØÐÂÐÞ¸
//	delay_kTicks(30);	//Delay 0.43us
	FIFO_RRST_(1);		//Stop reset to wait the next rising edge	
}

/******************************************************************
 * @Function	FIFO_Reset_Write_Pointer
 * @Brief		Reset write address
 * @Parameter	None
 ******************************************************************/
void FIFO_Reset_Write_Pointer(void)
{
	FIFO_WRST_(0);
	delay_us(1);		//Need to delay 0.75us, corresponding to 15 beats of 20MHz  ÐèÒªÖØÐÂµ÷Õû
						//Sufficient delay to ensure reset !!!!!!!!!
//	delay_kTicks(100);	//Delay 0.76us
	FIFO_WRST_(1);
}

/******************************************************************
 * @Function	FIFO_Read_Port
 * @Brief		Read single byte of data from FIFO
 * @Parameter	Return one Byte of data 
 ******************************************************************/
u8 FIFO_Read_Port(void)
{
	u8 result = 0;
	result |= (FIFO_D0 << 0);
	result |= (FIFO_D1 << 1);
	result |= (FIFO_D2 << 2);
	result |= (FIFO_D3 << 3);
	result |= (FIFO_D4 << 4);
	result |= (FIFO_D5 << 5);
	result |= (FIFO_D6 << 6);
	result |= (FIFO_D7 << 7);
	return result;
}	

/******************************************************************
 * @Function	FIFO_Read
 * @Brief		Read data steam from FIFO
 * @Parameter	Read 1000 Bytes by default
 ******************************************************************/
void FIFO_Read(void)
{
	u16 byte = 0;		//The Byte number that need to be read
	FIFO_RE_(0);		//Enable data reading
	FIFO_OE_(0);		//Enable data output
	FIFO_Reset_Pointer();	//Reset read address
	FIFO_RCK(1);		//The first rising edge. Read address move 1 bit
	delay_us(1);		//Need to delay 0.4us   ÐèÒªÖØÐÂµ÷Õû
//	delay_kTicks(30);	//Delay 0.43us
	FIFO_RCK(0);		//Wait for data reading
	delay_us(1);		//Need to delay 0.4us	ÐèÒªÖØÐÂµ÷Õû
//	delay_kTicks(30);	//Delay 0.43us
	
	for(byte = 0; byte < 1000; byte++)	//The total "for loop" takes xxx us 	ÐèÒª²âÊÔforÑ­»·µÄÔËÐÐÊ±¼ä£¡£¡£¡£¡£¡
	{
		FIFO_RCK(1);
		delay_us(1);		//Need to delay 0.2us   ÐèÒªÖØÐÂµ÷Õû
//		delay_kTicks(15);	//Delay 0.34us
		FIFO_Buffer[byte] = FIFO_Read_Port();
		FIFO_RCK(0);
		delay_us(1);		//Need to delay 0.4us	ÐèÒªÖØÐÂµ÷Õû
//		delay_kTicks(30);	//Delay 0.43us
	}
	FIFO_RE_(1);			//Disable data reading
}
