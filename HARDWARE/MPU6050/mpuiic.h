#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "sys.h"


// #define SDA_IN()  {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=0<<5*2;}	//PH5����ģʽ
// #define SDA_OUT() {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=1<<5*2;} //PH5���ģʽ
// //IO����
// #define IIC_SCL(n)  (n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_4,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_4,GPIO_PIN_RESET)) //SCL
// #define IIC_SDA(n)  (n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_5,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_5,GPIO_PIN_RESET)) //SDA
// #define READ_SDA    HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_5)  //����SDA

// IO��������
// #define MPU_SDA_IN()  {GPIOA->CRL&=0XFFFF0FFF;GPIOA->CRL|=8<<12;}
// #define MPU_SDA_OUT() {GPIOA->CRL&=0XFFFF0FFF;GPIOA->CRL|=3<<12;}
// PD(9)
#define MPU_SDA_IN_0()  {GPIOD->MODER &= ~(3 << (9 * 2)); GPIOD->MODER |= 0 << 9 * 2;}
#define MPU_SDA_OUT_0() {GPIOD->MODER &= ~(3 << (9 * 2)); GPIOD->MODER |= 1 << 9 * 2;}
// PD(11)
#define MPU_SDA_IN_1()  {GPIOD->MODER &= ~(3 << (11 * 2)); GPIOD->MODER |= 0 << 11 * 2;}
#define MPU_SDA_OUT_1() {GPIOD->MODER &= ~(3 << (11 * 2)); GPIOD->MODER |= 1 << 11 * 2;}
//PD(13)
#define MPU_SDA_IN_2()  {GPIOD->MODER &= ~(3 << (13 * 2)); GPIOD->MODER |= 0 << 13 * 2;}
#define MPU_SDA_OUT_2() {GPIOD->MODER &= ~(3 << (13 * 2)); GPIOD->MODER |= 1 << 13 * 2;}
// PD(15)
#define MPU_SDA_IN_3()  {GPIOD->MODER &= ~(3 << (15 * 2)); GPIOD->MODER |= 0 << ((u32)15 * 2);}
#define MPU_SDA_OUT_3() {GPIOD->MODER &= ~(3 << (15 * 2)); GPIOD->MODER |= 1 << ((u32)15 * 2);}

// IMU_0
#define MPU_IIC_SCL_0(n)    (n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET)) //SCL
#define MPU_IIC_SDA_0(n)    (n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET)) //SDA
#define MPU_READ_SDA_0      HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9)    // ����SDA
// IMU_1
#define MPU_IIC_SCL_1(n)    (n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET)) //SCL
#define MPU_IIC_SDA_1(n)    (n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET)) //SDA
#define MPU_READ_SDA_1      HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11)    // ����SDA
// IMU_2
#define MPU_IIC_SCL_2(n)    (n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET)) //SCL
#define MPU_IIC_SDA_2(n)    (n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET)) //SDA
#define MPU_READ_SDA_2      HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_13)    // ����SDA
// IMU_3
#define MPU_IIC_SCL_3(n)    (n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET)) //SCL
#define MPU_IIC_SDA_3(n)    (n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET)) //SDA
#define MPU_READ_SDA_3      HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15)    // ����SDA

// void MPU_IIC_SCL_0(u8 n)    {n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);}
// void MPU_IIC_SDA_0(u8 n)    {n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);}

// void MPU_IIC_SCL_1(u8 n)    {n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);}
// void MPU_IIC_SDA_1(u8 n)    {n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);}

// void MPU_IIC_SCL_2(u8 n)    {n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);}
// void MPU_IIC_SDA_2(u8 n)    {n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);}

// void MPU_IIC_SCL_3(u8 n)    {n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);}
// void MPU_IIC_SDA_3(u8 n)    {n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);}
//IIC���в�������
void MPU_IIC_Delay(void);				// MPU IIC��ʱ����
void MPU_IIC_Init(void);                // ��ʼ��IIC��IO��

void MPU_IIC_Start(int imuNum);				// ����IIC��ʼ�ź�
void MPU_IIC_Stop(int imuNum);	  			// ����IICֹͣ�ź�
void MPU_IIC_Send_Byte(int imuNum, u8 txd);			// IIC����һ���ֽ�
u8 MPU_IIC_Read_Byte(int imuNum, unsigned char ack);// IIC��ȡһ���ֽ�
u8 MPU_IIC_Wait_Ack(int imuNum); 				// IIC�ȴ�ACK�ź�
void MPU_IIC_Ack(int imuNum);					// IIC����ACK�ź�
void MPU_IIC_NAck(int imuNum);				// IIC������ACK�ź�

// IMU_0
// void MPU_IIC_Start_0(void);				// ����IIC��ʼ�ź�
// void MPU_IIC_Stop_0(void);	  			// ����IICֹͣ�ź�
// void MPU_IIC_Send_Byte_0(u8 txd);			// IIC����һ���ֽ�
// u8 MPU_IIC_Read_Byte_0(unsigned char ack);// IIC��ȡһ���ֽ�
// u8 MPU_IIC_Wait_Ack_0(void); 				// IIC�ȴ�ACK�ź�
// void MPU_IIC_Ack_0(void);					// IIC����ACK�ź�
// void MPU_IIC_NAck_0(void);				// IIC������ACK�ź�
// // IMU_1
// void MPU_IIC_Start_1(void);				// ����IIC��ʼ�ź�
// void MPU_IIC_Stop_1(void);	  			// ����IICֹͣ�ź�
// void MPU_IIC_Send_Byte_1(u8 txd);			// IIC����һ���ֽ�
// u8 MPU_IIC_Read_Byte_1(unsigned char ack);// IIC��ȡһ���ֽ�
// u8 MPU_IIC_Wait_Ack_1(void); 				// IIC�ȴ�ACK�ź�
// void MPU_IIC_Ack_1(void);					// IIC����ACK�ź�
// void MPU_IIC_NAck_1(void);				// IIC������ACK�ź�

// void IMPU_IC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
// u8 MPU_IIC_Read_One_Byte(u8 daddr,u8 addr);
#endif
















