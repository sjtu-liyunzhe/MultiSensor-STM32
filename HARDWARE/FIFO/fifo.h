#ifndef FIFO_H
#define FIFO_H
#include "sys.h"

//For AL422B
//FIFO DO Port:
//FIFO_D0	PE7		Input  Floating  Mode
//FIFO_D1	PE8		Input  Floating  Mode
//FIFO_D2	PE9		Input  Floating  Mode
//FIFO_D3	PE10	Input  Floating  Mode
//FIFO_D4	PE15	Input  Floating  Mode
//FIFO_D5	PB10	Input  Floating  Mode
//FIFO_D6	PB11	Input  Floating  Mode
//FIFO_D7	PH6		Input  Floating  Mode

//FIFO Control Port:
//FIFO_RE_		PE11	Output Push Pull Mode
//FIFO_OE_		PE12	Output Push Pull Mode
//FIFO_RRST_	PE13	Output Push Pull Mode
//FIFO_RCK		PE14	Output Push Pull Mode
//FIFO_WE_		PH7		Output Push Pull Mode
//FIFO_WRST_	PH8		Output Push Pull Mode

//Define FIFO Data Port
#define FIFO_D0			HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_7)	
#define FIFO_D1			HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8)	
#define FIFO_D2			HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9)	
#define FIFO_D3			HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10)	
#define FIFO_D4			HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15)	
#define FIFO_D5			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)	
#define FIFO_D6			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11)	
#define FIFO_D7			HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_6)	

//Define FIFO IO port
#define FIFO_RE_(n)		(n? HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET))
#define FIFO_OE_(n)		(n? HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET))
#define FIFO_RRST_(n)	(n? HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_RESET))
#define FIFO_RCK(n)		(n? HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_RESET))
#define FIFO_WE_(n)		(n? HAL_GPIO_WritePin(GPIOH, GPIO_PIN_7, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOH, GPIO_PIN_7, GPIO_PIN_RESET))
#define FIFO_WRST_(n)	(n? HAL_GPIO_WritePin(GPIOH, GPIO_PIN_8, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOH, GPIO_PIN_8, GPIO_PIN_RESET))

extern u8 FIFO_Buffer[1000];

void delay_kTicks(u16 tick_num);	//Delay 0.x us

void FIFO_IO_Init(void);
void FIFO_Init(void);			//Initialize FIFO
void FIFO_Reset_Pointer(void);	//Reset read address
void FIFO_Reset_Write_Pointer(void);	//Reset write address

void FIFO_Read(void);			//Read data steam from FIFO
u8 FIFO_Read_Port(void);		//Read single byte of data from FIFO

#endif
