#include "mpu6050.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

// 对接IMU硬件驱动
// 减少函数输出参数
u8 MPU_Write_Len_0(u8 addr,u8 reg,u8 len,u8 *buf)
{
	return MPU_Write_Len(0, addr, reg, len, buf);
}
u8 MPU_Write_Len_1(u8 addr,u8 reg,u8 len,u8 *buf)
{
	return MPU_Write_Len(1, addr, reg, len, buf);
}
u8 MPU_Write_Len_2(u8 addr,u8 reg,u8 len,u8 *buf)
{
	return MPU_Write_Len(2, addr, reg, len, buf);
}
u8 MPU_Write_Len_3(u8 addr,u8 reg,u8 len,u8 *buf)
{
	return MPU_Write_Len(3, addr, reg, len, buf);
}

u8 MPU_Read_Len_0(u8 addr,u8 reg,u8 len,u8 *buf)
{
	return MPU_Read_Len(0, addr, reg, len, buf);
}
u8 MPU_Read_Len_1(u8 addr,u8 reg,u8 len,u8 *buf)
{
	return MPU_Read_Len(1, addr, reg, len, buf);
}
u8 MPU_Read_Len_2(u8 addr,u8 reg,u8 len,u8 *buf)
{
	return MPU_Read_Len(2, addr, reg, len, buf);
}
u8 MPU_Read_Len_3(u8 addr,u8 reg,u8 len,u8 *buf)
{
	return MPU_Read_Len(3, addr, reg, len, buf);
}

// 初始化MPU6050
// 依次对所有IMU进行初始化
// 成功返回0
// 失败返回imuNum + 1
u8 MPU_Init(void)
{
	u8 res; 
	MPU_IIC_Init();//初始化IIC总线
	// for(int imuNum = 0; imuNum < 4; imuNum++)	没有用0号
	for(int imuNum = 1; imuNum < 4; imuNum++)
	{
		MPU_Write_Byte(imuNum, MPU_PWR_MGMT1_REG, 0X80);	//复位MPU6050
		delay_ms(100);
		MPU_Write_Byte(imuNum, MPU_PWR_MGMT1_REG, 0X00);	//唤醒MPU6050 
		MPU_Set_Gyro_Fsr(imuNum, 3);					//陀螺仪传感器,±2000dps
		MPU_Set_Accel_Fsr(imuNum, 0);					//加速度传感器,±2g
		MPU_Set_Rate(imuNum, 100);						//设置采样率50Hz
		MPU_Write_Byte(imuNum, MPU_INT_EN_REG, 0X00);	//关闭所有中断
		MPU_Write_Byte(imuNum, MPU_USER_CTRL_REG, 0X00);	//I2C主模式关闭
		MPU_Write_Byte(imuNum, MPU_FIFO_EN_REG, 0X00);	//关闭FIFO
		MPU_Write_Byte(imuNum, MPU_INTBP_CFG_REG, 0X80);	//INT引脚低电平有效
		res = MPU_Read_Byte(imuNum, MPU_DEVICE_ID_REG); 
		if(res == MPU_ADDR)//器件ID正确
		{
			MPU_Write_Byte(imuNum, MPU_PWR_MGMT1_REG, 0X01);	//设置CLKSEL,PLL X轴为参考
			MPU_Write_Byte(imuNum, MPU_PWR_MGMT2_REG, 0X00);	//加速度与陀螺仪都工作
			MPU_Set_Rate(imuNum, 100);						//设置采样率为50Hz
		}
		else
			return imuNum + 1;
	}
	return 0;
}
// 设置MPU6050陀螺仪传感器满量程范围
// fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
// 返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Gyro_Fsr(int imuNum, u8 fsr)
{
	return MPU_Write_Byte(imuNum, MPU_GYRO_CFG_REG, fsr<<3);//设置陀螺仪满量程范围  
}
// 设置MPU6050加速度传感器满量程范围
// fsr:0,±2g;1,±4g;2,±8g;3,±16g
// 返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Accel_Fsr(int imuNum, u8 fsr)
{
	return MPU_Write_Byte(imuNum, MPU_ACCEL_CFG_REG, fsr<<3);//设置加速度传感器满量程范围  
}
// 设置MPU6050的数字低通滤波器
// lpf:数字低通滤波频率(Hz)
// 返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_LPF(int imuNum, u16 lpf)
{
	u8 data=0;
	if(lpf >= 188)	data = 1;
	else if(lpf >= 98)	data = 2;
	else if(lpf >= 42)	data=3;
	else if(lpf >= 20)	data=4;
	else if(lpf >= 10)	data=5;
	else	data = 6; 
	return MPU_Write_Byte(imuNum, MPU_CFG_REG,data);//设置数字低通滤波器  
}
// 设置MPU6050的采样率(假定Fs=1KHz)
// rate:4~1000(Hz)
// 返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Rate(int imuNum, u16 rate)
{
	u8 data;
	if(rate > 1000)	rate = 1000;
	if(rate<4)	rate=4;
	data = 1000 / rate - 1;
	data = MPU_Write_Byte(imuNum, MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
 	return MPU_Set_LPF(imuNum, rate / 2);	//自动设置LPF为采样率的一半
}

// 得到温度值
// 返回值:温度值(扩大了100倍)
short MPU_Get_Temperature(int imuNum)
{
    u8 buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(imuNum, MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp=36.53+((double)raw)/340;  
    return temp*100;;
}
//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
u8 MPU_Get_Gyroscope(int imuNum, short *gx,short *gy,short *gz)
{
    u8 buf[6],res;  
	res = MPU_Read_Len(imuNum, MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res == 0)
	{
		*gx = ((u16)buf[0]<<8) | buf[1];  
		*gy = ((u16)buf[2]<<8) | buf[3];  
		*gz = ((u16)buf[4]<<8) | buf[5];
	} 	
    return res;;
}
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
u8 MPU_Get_Accelerometer(int imuNum, short *ax,short *ay,short *az)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(imuNum, MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax = ((u16)buf[0]<<8) | buf[1];  
		*ay = ((u16)buf[2]<<8) | buf[3];  
		*az = ((u16)buf[4]<<8) | buf[5];
	} 	
    return res;;
}
//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Len(int imuNum, u8 addr,u8 reg,u8 len,u8 *buf)
{
    MPU_IIC_Start(imuNum); 
	MPU_IIC_Send_Byte(imuNum, (addr<<1)|0);//发送器件地址+写命令	
	if(MPU_IIC_Wait_Ack(imuNum))	//等待应答
	{
		MPU_IIC_Stop(imuNum);
		return 1;		
	}
    MPU_IIC_Send_Byte(imuNum, reg);	//写寄存器地址
    MPU_IIC_Wait_Ack(imuNum);		//等待应答
	for(u8 i = 0;i < len; i++)
	{
		MPU_IIC_Send_Byte(imuNum, buf[i]);	//发送数据
		if(MPU_IIC_Wait_Ack(imuNum))		//等待ACK
		{
			MPU_IIC_Stop(imuNum);	 
			return 1;		 
		}		
	}    
    MPU_IIC_Stop(imuNum);	 
	return 0;	
} 
//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Read_Len(int imuNum, u8 addr,u8 reg,u8 len,u8 *buf)
{ 
 	MPU_IIC_Start(imuNum); 
	MPU_IIC_Send_Byte(imuNum, (addr<<1)|0);//发送器件地址+写命令	
	if(MPU_IIC_Wait_Ack(imuNum))	//等待应答
	{
		MPU_IIC_Stop(imuNum);		 
		return 1;		
	}
    MPU_IIC_Send_Byte(imuNum, reg);	//写寄存器地址
    MPU_IIC_Wait_Ack(imuNum);		//等待应答
    MPU_IIC_Start(imuNum);
	MPU_IIC_Send_Byte(imuNum, (addr<<1)|1);//发送器件地址+读命令	
    MPU_IIC_Wait_Ack(imuNum);		//等待应答 
	while(len)
	{
		if(len == 1)
			*buf = MPU_IIC_Read_Byte(imuNum, 0);//读数据,发送nACK 
		else
			*buf = MPU_IIC_Read_Byte(imuNum, 1);		//读数据,发送ACK  
		len--;
		buf++; 
	}    
    MPU_IIC_Stop(imuNum);	//产生一个停止条件 
	return 0;	
}
//IIC写一个字节 
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Byte(int imuNum, u8 reg,u8 data) 				 
{ 
    MPU_IIC_Start(imuNum); 
	MPU_IIC_Send_Byte(imuNum, (MPU_ADDR<<1)|0);//发送器件地址+写命令	
	if(MPU_IIC_Wait_Ack(imuNum))	//等待应答
	{
		MPU_IIC_Stop(imuNum);		 
		return 1;		
	}
    MPU_IIC_Send_Byte(imuNum, reg);	//写寄存器地址
    MPU_IIC_Wait_Ack(imuNum);		//等待应答 
	MPU_IIC_Send_Byte(imuNum, data);//发送数据
	if(MPU_IIC_Wait_Ack(imuNum))	//等待ACK
	{
		MPU_IIC_Stop(imuNum);	 
		return 1;		 
	}		 
    MPU_IIC_Stop(imuNum);
	return 0;
}
//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
u8 MPU_Read_Byte(int imuNum, u8 reg)
{
	u8 res;
    MPU_IIC_Start(imuNum); 
	MPU_IIC_Send_Byte(imuNum, (MPU_ADDR<<1)|0);//发送器件地址+写命令	
	MPU_IIC_Wait_Ack(imuNum);		//等待应答 
    MPU_IIC_Send_Byte(imuNum, reg);	//写寄存器地址
    MPU_IIC_Wait_Ack(imuNum);		//等待应答
    MPU_IIC_Start(imuNum);
	MPU_IIC_Send_Byte(imuNum, (MPU_ADDR<<1)|1);//发送器件地址+读命令	
    MPU_IIC_Wait_Ack(imuNum);		//等待应答 
	res=MPU_IIC_Read_Byte(imuNum, 0);//读取数据,发送nACK 
    MPU_IIC_Stop(imuNum);			//产生一个停止条件 
	return res;		
}


