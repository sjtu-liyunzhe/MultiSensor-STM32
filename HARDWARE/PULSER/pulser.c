#include "pulser.h"

//CH1_EN	PB5	Output Push Pull Mode
//CH2_EN	PB7	Output Push Pull Mode
//CH3_EN	PB6	Output Push Pull Mode
//CH4_EN	PB4	Output Push Pull Mode

//IN3_C_1	CH1_IN3		PE0		Output Push Pull Mode
//IN3_B_1	CH2_IN3		PE1		Output Push Pull Mode
//IN3_A_1	CH3_IN3		PB8		Output Push Pull Mode
//IN3_D_1	CH4_IN3		PB9		Output Push Pull Mode

/******************************************************************
 * @Function	Pulser_IO_Init
 * @Brief		Initialize the IO port of Pulser control
 * @Parameter	None
 ******************************************************************/
void Pulser_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();     	//GPIOB clock    
	__HAL_RCC_GPIOE_CLK_ENABLE();  		//GPIOE clock

	GPIO_InitStructure.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;	//PB4,5,6,7,8,9
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  //Output push pull mode
	GPIO_InitStructure.Pull = GPIO_NOPULL;          //No pull
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;     //High speed
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);		//PB4,5,6,7,8,9
	
	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1;		//PE0,1
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);		
}


/******************************************************************
 * @Function	ALL_CH_HZ
 * @Brief		Close all channels to HZ state
 * @Parameter	None
 ******************************************************************/
//Since we used Positive-OR-Gate, Pulser can only be closed to HZ state but not Clamp state
void ALL_CH_HZ(void)
{
	//IN4 was connected to GND
	CH1_EN(1);	CH2_EN(1);	CH3_EN(1);	CH4_EN(1);
	CH1_IN3(1);	CH2_IN3(1);	CH3_IN3(1);	CH4_IN3(1);
}
 

/******************************************************************
 * @Function	CH_TX
 * @Brief		Control Pulser transmission
 * @Parameter	Channel number
 ******************************************************************/
//Since we used Positive-OR-Gate, set CHx_EN=0 to transmit Pn/Nn to Pulser

void CH_TX(u8 Ch_Num)
{
	switch(Ch_Num)
	{
		case 1:	CH1_EN(0);	CH1_IN3(0);	break;
		case 2:	CH2_EN(0);	CH2_IN3(0);	break;
		case 3:	CH3_EN(0);	CH3_IN3(0);	break;
		case 4:	CH4_EN(0);	CH4_IN3(0);	break;
		default:	break;
	}
}	

/******************************************************************
 * @Function	CH_RX
 * @Brief		Control Pulser receive
 * @Parameter	Channel number
 ******************************************************************/
//Since we used Positive-OR-Gate, set CHx_EN=1 to disable transmitting Pn/Nn to Pulser
//void CH_RX(u8 Ch_Num)
//{
//	switch(Ch_Num)
//	{
//		case 1:	CH1_EN(1);	CH1_IN3(0);	break;
//		case 2:	CH2_EN(1);	CH2_IN3(0);	break;
//		case 3:	CH3_EN(1);	CH3_IN3(0);	break;
//		case 4:	CH4_EN(1);	CH4_IN3(0);	break;
//		default:	break;
//	}
//}


void CH_RX(u8 Ch_Num)
{
	switch(Ch_Num)
	{
		case 1:	CH1_EN(1);	break;
		case 2:	CH2_EN(1);	break;
		case 3:	CH3_EN(1);	break;
		case 4:	CH4_EN(1);	break;
		default:	break;
	}
}
