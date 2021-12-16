#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"

//KEY_1		PC13	IT_RISING Mode, Pullup		
//KEY_2		PI11	IT_RISING Mode, Pullup
//Key press trigger at rising edge

/******************************************************************
 * @Function	EXTI_Init
 * @Brief		�ⲿ�жϳ�ʼ��
 * @Parameter	None
 ******************************************************************/
//stm32f7xx_hal.c��HAL_Init()�����У��������ж����ȼ�����HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
//��2��2λ��ռ���ȼ�	2λ��Ӧ���ȼ�	���ȼ���������Ϊ0, 1, 2, 3
//��ֵԽС����������ȼ�Խ��

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
	
	//Response priority: KEY1 is prior to KEY2	��ֵԽС���ȼ�Խ��
	//Interrupt Line 13 (10-15����)						//For KEY_1	PC13
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);    		//��ռ���ȼ�Ϊ2�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);           		//ʹ���ж���
	
	//Interrupt Line 8 (5-9����)						//For KEY_2	PI8
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 1);       	//��ռ���ȼ�Ϊ2�������ȼ�Ϊ1
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);             		//ʹ���ж���
	
}

/******************************************************************
 * @Function	�ⲿ�жϷ�����
 * @Brief		���������ֿ�����startup_stm32f767xx.s���ҵ�
 * @Parameter	None
 ******************************************************************/
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);				//�����жϴ����ú���	KEY_1	PC13
}

void EXTI9_5_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);				//�����жϴ����ú���	KEY_2	PI8
}

/******************************************************************
 * @Function	HAL_GPIO_EXTI_Callback
 * @Brief		�жϴ���ص�������HAL�������е��ⲿ�жϷ�����������ô˺�����
 * @Parameter	GPIO_Pin
 * �ڴ˺�����д���жϴ�����߼������ж����źţ�ִ����ú������Զ������жϱ�־λ
 ******************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	delay_ms(10);				//����
	switch(GPIO_Pin)
	{
		case GPIO_PIN_13:
			if(KEY1 == 0)		//ȷʵ����
			{
				LED_R_Toggle;
			}
			break;
		case GPIO_PIN_8:		//ȷʵ����
			if(KEY2 == 0)
			{
				LED_W_Toggle;
			}
			break;
	}
}




