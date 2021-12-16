#ifndef __IMUCollecting_H
#define __IMUCollecting_H
#include "sys.h"

struct mpuData
{
	float pitch, roll, yaw;			//欧拉角
	short accx, aacy, aacz;			//加速度传感器原始数据
	short gyrox, gyroy, gyroz;		//陀螺仪原始数据
	short temp;						//温度
};
void GetMPUData(int imuNum, struct mpuData *data);
void PrintData(int imuNum, struct mpuData *data);
void usart3_Report(uint8_t fun, uint8_t *data, uint8_t len);
void IMUSendData(int imuNum, struct mpuData *data);

#endif // !__IMUCollecting_H