#ifndef _ads1299_H
#define _ads1299_H
#include "sys.h"

//EEG-EMG共有引脚号
//START 	PC7

//EMG模块ads1299引脚号
//CS2			output	PC9
//CLK2		output	PA9
//DIN2		output	PA11
//DOUT2		input		PC8
//DRDY2		input		PA8
//RESET2	output	PA10
//PWDN2		output	PA12

// START
#define START_LOW			(HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET))
#define START_HIGH		(HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET))


//4ch EMG模块
#define SCLK2_LOW			(HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET))
#define SCLK2_HIGH		(HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET))
#define	ADS2_DIN_LOW	(HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET))
#define ADS2_DIN_HIGH	(HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET))
#define	GET_DOUT2			(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8))
#define GET_DRDY2			(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8))
#define	SelectChip_2	(HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET))

void SelectChip(int);
void ADS1299Init(void);
void setDev(unsigned char, unsigned char, int);
void ADS1299_SDATAC(int);
void ADS1299_RDATAC(int);
void ADS1299_START_PIN(void);
unsigned char ADS1299_RREG(unsigned char,	int);
void ADS1299_WREG(unsigned char, unsigned char, unsigned char, int);
void stopDev(int);
void startDev(int);        //只有这个函数可以设置0，1，2，分别对应三种模式：ads1299_2
void dataAcq(void);
void ADS1299_READ(int);
void sendPackage(unsigned short*);
void USART2_Send_Data(uint8_t*,uint16_t);


#endif
