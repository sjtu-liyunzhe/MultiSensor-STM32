#ifndef __IMUCollecting_H
#define __IMUCollecting_H
#include "sys.h"

typedef struct mpuData
{
	float pitch, roll, yaw;			//欧拉角
	short accx, aacy, aacz;			//加速度传感器原始数据
	short gyrox, gyroy, gyroz;		//陀螺仪原始数据
	short temp;						//温度
}mpuData;
void GetMPUData(int imuNum, struct mpuData *data);
void PrintData(int imuNum, struct mpuData *data);
void usart3_Report(uint8_t fun, uint8_t *data, uint8_t len);
void usart3_SendPackage(u8 *data_buffer, u16 bytes_num);
void IMUSendData(int imuNum, struct mpuData *data);
void IMUSaveData(int imuNum, mpuData* data);


#endif // !__IMUCollecting_H