#include "dac.h"

//PA4	DAC_OUT1	VGN_A
//PA5	DAC_OUT2	VREF_A

DAC_HandleTypeDef DAC1_Handler;		//Handler of DAC1 configuration
DAC_HandleTypeDef DAC2_Handler;		//Handler of DAC2 configuration

/******************************************************************
 * @Function	DAC_Init
 * @Brief		Initialize DAC1 and DAC2, automaticlly call HAL_DAC_MspInit
 * @Parameter	None
 ******************************************************************/
void DAC_Init(void)
{
	DAC_ChannelConfTypeDef DACch_Config;	//Handler of DAC-channel configuration
	
	DAC1_Handler.Instance = DAC;			//Base address of peripheral
	DAC2_Handler.Instance = DAC;			//Base address of peripheral
	HAL_DAC_Init(&DAC1_Handler);			//Initialize DAC1
	HAL_DAC_Init(&DAC2_Handler);			//Initialize DAC2
	
	DACch_Config.DAC_Trigger = DAC_TRIGGER_NONE;	//Conversion is automatic once the DACx_DHRxxxx register has been loaded, and not by external trigger
	DACch_Config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;	//Enable output buffer to reduce output impedance and drive external load directly
																//If enable, DAC output cannot reach 0V accurately. Here use DISABLE
	HAL_DAC_ConfigChannel(&DAC1_Handler, &DACch_Config, DAC_CHANNEL_1);	//PA4 corresponds to DAC channel 1
	HAL_DAC_ConfigChannel(&DAC2_Handler, &DACch_Config, DAC_CHANNEL_2);	//PA5 corresponds to DAC channel 2
	
	HAL_DAC_Start(&DAC1_Handler, DAC_CHANNEL_1);	//Start DAC1
	HAL_DAC_Start(&DAC2_Handler, DAC_CHANNEL_2);	//Start DAC2
}


/******************************************************************
 * @Function	HAL_DAC_MspInit	(called by HAL_DAC_Init())
 * @Brief		Bottom drive of DAC, include RCC-Clock and GPIO configuration	
 * @Parameter	hdac--DACx_Handler
 ******************************************************************/
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	__HAL_RCC_DAC_CLK_ENABLE();				//Enable DAC clock
	__HAL_RCC_GPIOA_CLK_ENABLE();			//Enable GPIOA clock
	
	GPIO_InitStructure.Pin = GPIO_PIN_4 | GPIO_PIN_5;	//PA4, PA5
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;			//Analog
	GPIO_InitStructure.Pull = GPIO_NOPULL;				//No pull
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;     	//High speed
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);			//PA4, PA5
}

///******************************************************************
// * @Function	Set the output voltage of DAC1
// * @Brief		vol=0~3300 for 0~3.3V, 1000->1V
// * @Parameter	DACx: DAC1 or DAC2, vol:mV
// ******************************************************************/
//void DAC_Set_Vol(u8 DACx, u16 vol)
//{
//	double temp=vol;
//	temp/=1000;
//	temp=temp*4096/3.3;		//2^12 = 4096, 2^8 = 256
//	
//	switch(DACx)
//	{
//		case 1:
//			HAL_DAC_SetValue(&DAC1_Handler, DAC_CHANNEL_1, DAC_ALIGN_12B_R, temp);	//12位右对齐数据格式设置DAC值
//		break;
//		case 2:
//			HAL_DAC_SetValue(&DAC2_Handler, DAC_CHANNEL_2, DAC_ALIGN_12B_R, temp);	//12位右对齐数据格式设置DAC值
//		break;	
//		default:	break;
//	}
//}

/******************************************************************
 * @Function	Set the output voltage of DAC1
 * @Brief		vol=0~3.3 for 0~3.3V, 1->1V
 * @Parameter	DACx: DAC1 or DAC2, vol
 ******************************************************************/
void DAC_Set_Vol(u8 DACx, double vol)
{
	double temp;
//	temp/=1000;
	temp=vol*4096/3.3;		//2^12 = 4096, 2^8 = 256	temp/4096 = vol/3.3
	
	switch(DACx)
	{
		case 1:
			HAL_DAC_SetValue(&DAC1_Handler, DAC_CHANNEL_1, DAC_ALIGN_12B_R, temp);	//12位右对齐数据格式设置DAC值
		break;
		case 2:
			HAL_DAC_SetValue(&DAC2_Handler, DAC_CHANNEL_2, DAC_ALIGN_12B_R, temp);	//12位右对齐数据格式设置DAC值
		break;	
		default:	break;
	}
}



