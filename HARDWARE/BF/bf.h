#ifndef BF_H
#define BF_H
#include "sys.h"
#include "delay.h"

//BF_TX_EN		PB3		Output Push Pull Mode
//BF_RST		PD7		Output Push Pull Mode
//BF_SRD		PD5		Input  Floating  Mode
//BF_SLE		PD4 	Output Push Pull Mode
//BF_SWR		PD3 	Output Push Pull Mode
//BF_SCLK		PD2 	Output Push Pull Mode

#define QUANTA_US	delay_us(20)	//used in Beamformer register writing

//Define Beamformer IO port
#define BF_FIRE		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET)		//BF_TX_EN=1 
#define BF_STOP		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET)	//BF_TX_EN=0 

#define BF_RST(n)	(n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET))
#define BF_SRD(n)	(n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET))
#define BF_SLE(n)	(n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET))
#define BF_SWR(n)	(n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET))
#define BF_SCLK(n)  (n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET))


void BF_IO_Init(void);	//Initialze the IO pin for Beamformer
void BF_Write(u8 Data_LEN, u8 Register_Add, long long BF_Data);	//Write data to Beamformer's registers
void BF_Init(void);	//Initialize the Beamformer setting

#endif

