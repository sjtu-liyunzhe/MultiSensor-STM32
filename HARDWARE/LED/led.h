#ifndef LED_H
#define LED_H
#include "sys.h"

//LED_W		PE2		Output Push Pull Mode, Pullup
//LED_R		PE3		Output Push Pull Mode, Pullup
//LED_B		PE4		Output Push Pull Mode, Pullup
//LED_Y		PE5		Output Push Pull Mode, Pullup
//LED_G		PE6		Output Push Pull Mode, Pullup

//1 for LED on, 0 for LED off

//Define LED IO port
#define LED_W(n)		(n? HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET))
#define LED_R(n)		(n? HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET))
//#define LED_B(n)		(n? HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET))
//#define LED_Y(n)		(n? HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET))
#define LED_G(n)		(n? HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET))


//·­×ª
#define LED_W_Toggle	(HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_8))	
#define LED_R_Toggle	(HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9))	
//#define LED_B_Toggle	(HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4))	
//#define LED_Y_Toggle	(HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5))	
#define LED_G_Toggle	(HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_2))	


#define On 	1
#define Off	0

void LED_Init(void);
#endif
