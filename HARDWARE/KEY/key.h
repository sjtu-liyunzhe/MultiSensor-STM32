#ifndef KEY_H
#define KEY_H
#include "sys.h"

//KEY_1		PC13		Input Push Pull Mode, Pullup
//KEY_2		PI8			Input Push Pull Mode, Pullup
//0 for key press, 1 for key release

//Define KEY state
#define KEY1		HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)	//state of KEY_1
#define KEY2		HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_8)		//state of KEY_2

#define KEY1_PRES	1	//Key value of KEY_1 press
#define KEY2_PRES	2	//Key value of KEY_2 press

void KEY_Init(void);
u8 KEY_Scan(u8 mode);

#endif
