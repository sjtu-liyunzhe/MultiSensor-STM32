//#include "key.h"
//#include "delay.h"

////KEY_1		PI8		Input Push Pull Mode, Pullup
////KEY_2		PC13	Input Push Pull Mode, Pullup
////0 for key press, 1 for key release

//void KEY_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	__HAL_RCC_GPIOI_CLK_ENABLE();	//GPIOI clock
//	__HAL_RCC_GPIOC_CLK_ENABLE();	//GPIOC clock
//	
//	GPIO_InitStructure.Pin = GPIO_PIN_8;				//PI8
//	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;			//Input
//	GPIO_InitStructure.Pull = GPIO_PULLUP;				//Pull up
//	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;			//High speed
//	HAL_GPIO_Init(GPIOI, &GPIO_InitStructure);			//PI8
//	
//	GPIO_InitStructure.Pin = GPIO_PIN_13;				//PI8
//	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);			//PC13
//}


///*********************************************************************************************
// * @Function	KEY_Scan
// * @Brief		Detect key state and return key value
// * @Parameter	mode: 0 for continuous press not supported, 1 for continuous press supported
// * Response priority: KEY1 is prior to KEY2
// ********************************************************************************************/
//u8 KEY_Scan(u8 mode)
//{
//    static u8 key_up=1;   //Flag for key release, 0 for no release, 1 for being released
//							//Defined by static and be set only once
//    if(mode==1)				//If support continuous press
//		key_up=1;    		//All keys have been released in each scan
//	
//	if(key_up && (KEY1==0 || KEY2==0))		//If any key is pressed
//	{
//		delay_ms(10);		//Delay to avoid shake
//		key_up = 0;			//Did press a key
//		if(KEY1 == 0)		
//			return KEY1_PRES;
//		else if(KEY2 == 0)	
//			return KEY2_PRES;
//	}
//	else if(KEY1 == 1 && KEY2 == 1)			//If no key is pressed
//	{
//		key_up = 1;			//All keys have been released
//	}
//	return 0;				//No key press
//}

