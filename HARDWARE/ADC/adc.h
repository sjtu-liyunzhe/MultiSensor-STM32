#ifndef __ADC_H
#define __ADC_H
#include "sys.h"


extern u8 FIFO_buffer[1000];
#include "stm32f7xx.h"

// ADC GPIO 宏定义
#define RHEOSTAT_ADC_GPIO_PORT              GPIOA
#define RHEOSTAT_ADC_GPIO_PIN               GPIO_PIN_1
#define RHEOSTAT_ADC_GPIO_CLK_ENABLE()      __GPIOC_CLK_ENABLE()

// ADC 序号宏定义
#define RHEOSTAT_ADC                        ADC1
#define RHEOSTAT_ADC_CLK_ENABLE()           __ADC1_CLK_ENABLE()
#define RHEOSTAT_ADC_CHANNEL                ADC_CHANNEL_1

// ADC DR寄存器宏定义，ADC转换后的数字值则存放在这里
#define RHEOSTAT_ADC_DR_ADDR                ((uint32_t)ADC1+0x4c)

// ADC DMA 通道宏定义，这里我们使用DMA传输
#define RHEOSTAT_ADC_DMA_CLK_ENABLE()       __DMA2_CLK_ENABLE()
#define RHEOSTAT_ADC_DMA_CHANNEL            DMA_CHANNEL_0
#define RHEOSTAT_ADC_DMA_STREAM             DMA2_Stream0



void Rheostat_Init(void);
void FIFO_read(u16 times,u8 Ch_Num);
#endif /* __BSP_ADC_H */

