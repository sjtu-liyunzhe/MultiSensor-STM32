#ifndef TIMER_H
#define TIMER_H
#include "sys.h"

extern u8 Ch_Flag;
extern TIM_HandleTypeDef TIM3_Handler;	//Handler of Timer 3

void TIM3_Init(u16 arr, u16 psc);	//Initialize the paremeters of Timer 3
void Start_TIM3(void);				//Start Timer 3
void Stop_TIM3(void);				//Stop Timer 3
#endif

