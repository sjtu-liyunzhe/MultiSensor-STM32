#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"

//KEY_1		PC13	IT_RISING Mode, Pullup		
//KEY_2		PI11	IT_RISING Mode, Pullup
//Key press trigger at rising edge

/******************************************************************
 * @Function	EXTI_Init
 * @Brief		外部中断初始化
 * @Parameter	None
 ******************************************************************/
//stm32f7xx_hal.c的HAL_Init()函数中，设置了中断优先级分组HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
//组2：2位抢占优先级	2位响应优先级	优先级可以设置为0, 1, 2, 3
//数值越小，代表的优先级越高

void EXTI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOC_CLK_ENABLE();	//GPIOC clock
	__HAL_RCC_GPIOI_CLK_ENABLE();	//GPIOI clock
	
	GPIO_InitStructure.Pin = GPIO_PIN_13;				//PC13
	GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;		//Trigger at falling edge
	GPIO_InitStructure.Pull = GPIO_PULLUP;				//Pull up
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;			//High speed
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);			//PC13
	
	GPIO_InitStructure.Pin = GPIO_PIN_8;				//PI8
	HAL_GPIO_Init(GPIOI, &GPIO_InitStructure);			//PI8
	
	//Response priority: KEY1 is prior to KEY2	数值越小优先级越高
	//Interrupt Line 13 (10-15共用)						//For KEY_1	PC13
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);    		//抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);           		//使能中断线
	
	//Interrupt Line 8 (5-9共用)						//For KEY_2	PI8
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 1);       	//抢占优先级为2，子优先级为1
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);             		//使能中断线
	
}

/******************************************************************
 * @Function	外部中断服务函数
 * @Brief		函数的名字可以在startup_stm32f767xx.s中找到
 * @Parameter	None
 ******************************************************************/
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);				//调用中断处理公用函数	KEY_1	PC13
}

void EXTI9_5_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);				//调用中断处理公用函数	KEY_2	PI8
}

/******************************************************************
 * @Function	HAL_GPIO_EXTI_Callback
 * @Brief		中断处理回调函数，HAL库中所有的外部中断服务函数都会调用此函数，
 * @Parameter	GPIO_Pin
 * 在此函数中写入中断处理的逻辑，先判断引脚号，执行完该函数会自动清零中断标志位
 ******************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	delay_ms(10);				//消抖
	switch(GPIO_Pin)
	{
		case GPIO_PIN_13:
			if(KEY1 == 0)		//确实按下
			{
				LED_R_Toggle;
			}
			break;
		case GPIO_PIN_8:		//确实按下
			if(KEY2 == 0)
			{
				LED_W_Toggle;
			}
			break;
	}
}




