#include "timer.h"
#include "led.h"

//~����TIM3��ʱ��������ÿ��ͨ���������ɼ��������ʱ�����ͬ����ʱ�����ᴥ���жϺ���HAL_TIM_PeriodElapsedCallback������־λCh_Flag��1������main�����е�whileѭ����
u8 Ch_Flag = 0;		//Flag for controlling the time of each channel 
TIM_HandleTypeDef TIM3_Handler;		//Handler of Timer 3

/********************************************************************
 * @Function	TIM3_Init
 * @Brief		Initialize the parameters of Timer 3, set arr and psc
 * @Parameter	arr: Auto reload value, psc: Prescaler value
 * Timing time: Tout = ((arr+1)*(psc+1))/Ft (us)
 * Ft is the frequency of timer clock (MHz)
 * For STM32F7 working at 216MHz: Ft = 2*APB1 = 108MHz //~APB1��Ӧ�ķ�Ƶϵ����3��ƵΪ53MHz��ϵͳʱ��sysclk��1/3���������ߵ�ͨ�ö�ʱ��ʱ��FtΪAPB1ʱ�ӵ�2����
 ********************************************************************/
void TIM3_Init(u16 arr, u16 psc)
{
	TIM3_Handler.Instance = TIM3;
	TIM3_Handler.Init.Prescaler = psc;						//Prescaler value of timer clk
	TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;		//Count up mode
	TIM3_Handler.Init.Period = arr;			//Auto reload value, range from Min_Data = 0x0000 to Max_Data = 0xFFFF
	TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;	//Frequency division ratio of timer clock and digital filter, no use. Here use default
	
	HAL_TIM_Base_Init(&TIM3_Handler);		//Timer time base paremeter initialization function. It will call HAL_TIM_Base_MspInit()
}


 void Start_TIM3(void)
{
	HAL_TIM_Base_Start_IT(&TIM3_Handler);	//Start Timer 3 and enable Timer 3 update interrupt:TIM_IT_UPDATE
}

void Stop_TIM3(void)
{
	HAL_TIM_Base_Stop_IT(&TIM3_Handler);	//Stop Timer 3 and disable Timer 3 update interrupt
}

/*************************************************************************************************************
 * @Function	HAL_TIM_Base_MspInit
 * @Brief		Callback function of timer time base initialization function. All Timer x share this function
 * @Parameter	htim--TIMx_Handler
 * Classify the Timer NO., enable timer clock, set interrupt priority and enable timer interrupt
 *************************************************************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim -> Instance == TIM3)				//Firstly, we need to classify the Timer NO. 
	{
		__HAL_RCC_TIM3_CLK_ENABLE();			//Enable Timer 3 clock
		HAL_NVIC_SetPriority(TIM3_IRQn, 1, 3);	//Set interrupt priority, Preemption Priority: 1, Sub Priority: 3 //~���ö�ʱ��3���ж���ռ���ȼ�Ϊ1����Ӧ���ȼ�λ3��
		HAL_NVIC_EnableIRQ(TIM3_IRQn);			//Enable Timer 3 interrupt
	}
	/* "else if" for other Timer x */	
}


/**************************************************************************************************************
 * @Function	TIM3_IRQHandler
 * @Brief		Interrupt request service function of Timer 3  //~ͨ�ö�ʱ��3���жϷ�����
 * @Parameter	This function will call the general timer interrupt handling entry function:HAL_TIM_IRQHandler
 **************************************************************************************************************/
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM3_Handler);	//General timer interrupt handling entry function
										//This function classifies the type of timer interrupt, and calls the corresponding callback function
										//For example, it will call HAL_TIM_PeriodElapsedCallback() when Update Interrupt is enabled  
										//~��ͨ�ö�ʱ���ж�
}


/******************************************************************
 * @Function	HAL_TIM_PeriodElapsedCallback
 * @Brief		Callback function of timer update interrupt  //~���¶�ʱ���ж�ʱ����õĻص�����
 * @Parameter	htim--TIMx_Handler
 * Classify the Timer NO., finish the user-defined operation
 ******************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)			//Firstly, we need to classify the Timer NO.
	{
		/* User-defined operation */
		Ch_Flag = 1;					//Set the flag of time control for each channel	
	}
	/* "else if" for other Timer x */	
}

