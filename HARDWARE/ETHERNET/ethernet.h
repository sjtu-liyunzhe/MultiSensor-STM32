#ifndef ETHERNET_H
#define ETHERNET_H
#include "sys.h"

//For CP2200
//Control Port:
//INT_E	    PD8		Input Floating Mode
//CS_E		PD9		Output Push Pull Mode
//WR_E		PD10	Output Push Pull Mode
//RD_E		PD11	Output Push Pull Mode
//RST_E 	PA15	Output Push Pull Mode or Input Floating Mode

//Address Port:
//A0_E		PC9		Output Push Pull Mode
//A1_E		PC8		Output Push Pull Mode
//A2_E		PC7		Output Push Pull Mode
//A3_E		PC6		Output Push Pull Mode
//A4_E		PD15	Output Push Pull Mode
//A5_E		PD14	Output Push Pull Mode
//A6_E		PD13	Output Push Pull Mode
//A7_E		PD12	Output Push Pull Mode

//Data Port:
//D0_E		PD1		Output Push Pull Mode or Input Floating Mode
//D1_E		PD0		Output Push Pull Mode or Input Floating Mode
//D2_E		PC12	Output Push Pull Mode or Input Floating Mode
//D3_E		PC11	Output Push Pull Mode or Input Floating Mode
//D4_E		PC10	Output Push Pull Mode or Input Floating Mode
//D5_E		PA12	Output Push Pull Mode or Input Floating Mode
//D6_E		PA11	Output Push Pull Mode or Input Floating Mode
//D7_E		PA8 	Output Push Pull Mode or Input Floating Mode


//Define Control Port:
#define INT_E_			HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8)	

#define CS_E_(n)		(n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET))
#define WR_E_(n)		(n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET))
#define RD_E_(n)		(n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET))

#define RST_E_READ		HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)	
#define RST_E_WRITE(n)	(n? HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET))

//Define Address Port:
#define A0_E(n)		(n? HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET))
#define A1_E(n)		(n? HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET))
#define A2_E(n)		(n? HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET))
#define A3_E(n)		(n? HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET))
#define A4_E(n)		(n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET))
#define A5_E(n)		(n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET))
#define A6_E(n)		(n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET))
#define A7_E(n)		(n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET))

//Define Data Port:
//Read data port
#define D0_E_READ			HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1)
#define D1_E_READ			HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0)
#define D2_E_READ			HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12)
#define D3_E_READ			HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11)
#define D4_E_READ			HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10)
#define D5_E_READ			HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12)
#define D6_E_READ			HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11)
#define D7_E_READ			HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8)
//Write data port
#define D0_E_WRITE(n)		(n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET))
#define D1_E_WRITE(n)		(n? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET))
#define D2_E_WRITE(n)		(n? HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET))
#define D3_E_WRITE(n)		(n? HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET))
#define D4_E_WRITE(n)		(n? HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET))
#define D5_E_WRITE(n)		(n? HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET))
#define D6_E_WRITE(n)		(n? HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET))
#define D7_E_WRITE(n)		(n? HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET))

extern u8 MAC_Address[6];

void Ethernet_IO_Init(void);			//Initialize INT, CS, WR, RD and A0--A7
void Ethernet_RST_Mode(u8 mode);		//Config RST_E_ to input or output mode
void Ethernet_DataPort_Mode(u8 mode);	//Config D0--D7 to read or write mode

void Ethernet_Arrange_Address(u8 raw_address);
void Ethernet_Arrange_Data(u8 raw_data);

u8 Ethernet_Read_Data(void);
u8 Read_Ethernet(u8 Reg_Address);

void Write_Ethernet(u8 Reg_Address, u8 TX_Data);
void Ethernet_Transmit_Packet(void);

void Ethernet_Controller_Init(void);	//Initialize CP2200

#endif
