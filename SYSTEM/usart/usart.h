#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
extern UART_HandleTypeDef UART2_Handler; //UART���

extern u8  USART3_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART3_RX_STA;         		//����״̬���	
extern UART_HandleTypeDef UART3_Handler; //UART���

#define RXBUFFERSIZE   1 //�����С
extern u8 aRxBuffer[RXBUFFERSIZE];//HAL��USART����Buffer
extern u8 aRxBuffer3[RXBUFFERSIZE];//HAL��USART����Buffer
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void uart3_init(u32 bound);
// void HAL_UART_MspInit(UART_HandleTypeDef *huart)

#endif
