#ifndef	PULSER_H
#define PULSER_H
#include "sys.h"

//~ 此处的设置跟电路部分可能有点不太一样
//CH1_EN	PB4	Output Push Pull Mode
//CH2_EN	PB7	Output Push Pull Mode
//CH3_EN	PB5	Output Push Pull Mode
//CH4_EN	PB6	Output Push Pull Mode

//IN3_C_1	CH1_IN3		PB8		Output Push Pull Mode
//IN3_B_1	CH2_IN3		PE1		Output Push Pull Mode
//IN3_A_1	CH3_IN3		PB9		Output Push Pull Mode
//IN3_D_1	CH4_IN3		PE0		Output Push Pull Mode



//Define Pulser IO port
#define CH1_EN(n)	(n? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET))
#define CH2_EN(n)	(n? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET))
#define CH3_EN(n)	(n? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET))
#define CH4_EN(n)	(n? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET))

#define CH1_IN3(n)	(n? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET))
#define CH2_IN3(n)	(n? HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET))
#define CH3_IN3(n)	(n? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET))
#define CH4_IN3(n)	(n? HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET))


void Pulser_IO_Init(void);	//Initialize the IO port of pulser control
void ALL_CH_HZ(void);		//Close all channel to HZ state
void CH_TX(u8 Ch_Num);		//For pulser transmission
void CH_RX(u8 Ch_Num);		//For pulser receive

#endif
