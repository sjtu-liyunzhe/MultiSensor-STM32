#include "IMUCollecting.h"
#include "inv_mpu_0.h"
#include "inv_mpu_1.h"
#include "inv_mpu_2.h"
#include "inv_mpu_3.h"
#include "inv_mpu_dmp_motion_driver_0.h"
#include "inv_mpu_dmp_motion_driver_1.h"
#include "inv_mpu_dmp_motion_driver_2.h"
#include "inv_mpu_dmp_motion_driver_3.h"
#include "mpu6050.h"
#include "usart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"

extern int temp;

void GetMPUData(int imuNum, struct mpuData *data)
{
	taskENTER_CRITICAL();           //进入临界区
	switch (imuNum)
	{
	case 0:
		mpu_dmp_get_data_0(&(data->pitch), &(data->roll), &(data->yaw));
		break;
	case 1:
		mpu_dmp_get_data_1(&(data->pitch), &(data->roll), &(data->yaw));
		break;
	case 2:
		mpu_dmp_get_data_2(&(data->pitch), &(data->roll), &(data->yaw));
		break;
	case 3:
		mpu_dmp_get_data_3(&(data->pitch), &(data->roll), &(data->yaw));
		break;
	default:
		break;
	}
	taskEXIT_CRITICAL();            //退出临界区
	delay_ms(1);
	taskENTER_CRITICAL();           //进入临界区
	data->temp = MPU_Get_Temperature(imuNum);
	taskEXIT_CRITICAL();            //退出临界区
	delay_ms(1);
	taskENTER_CRITICAL();           //进入临界区
	MPU_Get_Accelerometer(imuNum, &(data->accx), &(data->aacy), &(data->aacz));
	taskEXIT_CRITICAL();            //退出临界区
	delay_ms(1);
	taskENTER_CRITICAL();           //进入临界区
	MPU_Get_Gyroscope(imuNum, &(data->gyrox), &(data->gyrox), &(data->gyroz));
	taskEXIT_CRITICAL();            //退出临界区
	delay_ms(1);
}
//
void PrintData(int imuNum, struct mpuData *data)
{
	// 温度值
	temp = data->temp;
	if(temp < 0)
	{
		temp = -temp;
		printf("Temp_%d:  -%d.%dC\r\n", imuNum, temp / 100, temp % 10);
	}
	else
		printf("Temp_%d:  %d.%dC\r\n", imuNum, temp / 100, temp % 10);
	// Pitch值
	temp = data->pitch * 10;
	if(temp < 0)
	{
		temp = -temp;
		printf("Pitch_%d:  -%d.%dC\r\n", imuNum, temp / 10, temp % 10);
	}
	else
		printf("Pitch_%d:  %d.%dC\r\n", imuNum, temp / 10, temp % 10);
	// Roll值
	temp = data->roll * 10;
	if(temp < 0)
	{
		temp = -temp;
		printf("Roll_%d:  -%d.%dC\r\n", imuNum, temp / 10, temp % 10);
	}
	else
	{
		printf("Roll_%d:  %d.%dC\r\n", imuNum, temp / 10, temp % 10);
	}
	// Yaw值
	temp = data->yaw * 10;
	if(temp < 10)
	{
		temp = -temp;
		printf("Yaw_%d:  -%d.%dC\r\n", imuNum, temp / 10, temp % 10);
	}
	else
		printf("Yaw_%d:  %d.%dC\r\n", imuNum, temp / 10, temp % 10);
	printf("\r\n");
	delay_ms(1);
}
// data_buffer 要传输的数据串
// bytes_num   数据总字节数
void usart3_SendPackage(u8 *data_buffer, u16 bytes_num)
{
	HAL_UART_Transmit(&UART3_Handler, (u8 *)data_buffer, bytes_num, 1000);
	while(__HAL_USART_GET_FLAG(&UART3_Handler, UART_FLAG_TC) != SET);
}

// 传送数据给匿名四轴上位机软件(V2.6版本)
// fun:功能字. 0XA0~0XAF
// data:数据缓存区,最多28字节
// 多字节数据高位在前
// len:data区有效数据个数
void usart3_Report(u8 fun, u8 *data, u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len > 28)
		return;
	send_buf[len + 3] = 0;    // 和校验位置0
	send_buf[0] = 0x88;       // 帧头
	send_buf[1] = fun;        // 功能字
	send_buf[2] = len;        // 数据长度
	for(i = 0; i < len; i++)
	{
		send_buf[3 + i] = data[i];
	}
	for(i = 0; i < len + 3; i++)
	{
		send_buf[len + 3] += send_buf[i];    // 和校验位
	}
	usart3_SendPackage(send_buf, len + 4);
}
// roll, pitch , yaw *100 分别化成整数发送
void IMUSendData(int imuNum, struct mpuData *data)
{
	// data字节数9 * 2
	u8 tbuf[18];
	u16 intRoll = (int)(100 * (data->roll));
	u16 intPitch = (int)(100 * (data->pitch));
	u16 intYaw = (int)(100 * (data->yaw));
	tbuf[0] = ((data->accx) >> 8) & 0xFF;    // 高字节
	tbuf[1] = (data->accx) & 0xFF;           // 低字节
	tbuf[2] = ((data->aacy) >> 8) & 0xFF;
	tbuf[3] = (data->aacy) & 0xFF;
	tbuf[4] = ((data->aacz) >> 8) & 0xFF;
	tbuf[5] = (data->aacz) & 0xFF;
	tbuf[6] = ((data->gyrox)>> 8) & 0xFF;
	tbuf[7] = (data->gyrox) & 0xFF;
	tbuf[8] = ((data->gyroy)>> 8) & 0xFF;
	tbuf[9] = (data->gyroy) & 0xFF;
	tbuf[10] = ((data->gyroz) >> 8) & 0xFF;
	tbuf[11] = (data->gyroz) & 0xFF;
	tbuf[12] = (intRoll >> 8) & 0xFF;
	tbuf[13] = intRoll & 0xFF;
	tbuf[14] = (intPitch >> 8) & 0xFF;
	tbuf[15] = intPitch & 0xFF;
	tbuf[16] = (intYaw >> 8) & 0xFF;
	tbuf[17] = intYaw & 0xFF;
	// 功能字对应：
	// 0 : 0xA1; 1 : 0xA2; 2 : 0xA3; 3 : 0xA4
	usart3_Report((imuNum + 1) | 0xA0, tbuf, 18);
}