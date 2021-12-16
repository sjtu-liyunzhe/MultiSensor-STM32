#ifndef DAC_H
#define DAC_H
#include "sys.h"

extern DAC_HandleTypeDef DAC1_Handler;		//Handler of DAC configuration, defined by extern so that it can be used in main.c
extern DAC_HandleTypeDef DAC2_Handler;		//Handler of DAC configuration, defined by extern so that it can be used in main.c

void DAC_Init(void);
void DAC_Set_Vol(u8 DACx, double vol);


#endif
