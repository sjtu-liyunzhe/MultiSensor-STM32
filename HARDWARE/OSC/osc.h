#ifndef OSC_H
#define OSC_H
#include "sys.h"

//For LTC6903
//OSC_SEN	PA0		Output Push Pull Mode
//OSC_SCK	PA1		Output Push Pull Mode
//OSC_SDI	PA2		Output Push Pull Mode
//OSC_OE	PH2		Output Push Pull Mode
//GATE		PH3		Output Push Pull Mode

//Define OSC IO port
#define OSC_SEN_(n)		(n? HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET))
#define OSC_SCK(n)		(n? HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET))
#define OSC_SDI(n)		(n? HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET))
#define OSC_OE(n)		(n? HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_RESET))

#define OSC_GATE_OPEN		HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, GPIO_PIN_SET)
#define OSC_GATE_CLOSE		HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, GPIO_PIN_RESET)

void OSC_IO_Init(void);
void Oscillator_Init(void);
void Write_Oscillator_20MHz(void);

#endif
