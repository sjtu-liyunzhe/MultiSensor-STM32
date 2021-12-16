#include "led.h"

//LED_W		PE8		Output Push Pull Mode, Pullup
//LED_R		PE9		Output Push Pull Mode, Pullup


//LED_B		PE4		Output Push Pull Mode, Pullup
//LED_Y		PE5		Output Push Pull Mode, Pullup
//LED_G		PE10		Output Push Pull Mode, Pullup

//1 for LED on, 0 for LED off

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOE_CLK_ENABLE();	//GPIOE clock
//	__HAL_RCC_GPIOI_CLK_ENABLE();	//GPIOI clock
	
	GPIO_InitStructure.Pin = GPIO_PIN_8 | GPIO_PIN_9  | GPIO_PIN_2;	//PE8，9
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//Output push pull mode
	GPIO_InitStructure.Pull = GPIO_NOPULL;			//Pull down  共阴连接，拉低保持灭灯
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;			//High speed
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);			//PE8,9
	
	
	//Turn off all LED
//	LED_W(On);
//	LED_R(On);
//	LED_B(Off);
//	LED_Y(Off);
//	LED_G(Off);
//	LED_O(Off);
	
}
