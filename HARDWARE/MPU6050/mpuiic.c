#include "mpuiic.h"
#include "delay.h"
#include "sys.h"
// #include "key.h"
#include "usart.h"
// #include "usmart.h"
//MPU IIC 延时函数
void MPU_IIC_Delay(void)
{
	delay_us(2);
}

//初始化IIC
void MPU_IIC_Init(void)
{
	printf("in iic init");
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOD_CLK_ENABLE();           // 开启GPIOD时钟
	 
    GPIO_Initure.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  // 推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;          // 上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    	// 高速
	
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);// 输出高	
	
	HAL_GPIO_Init(GPIOD, &GPIO_Initure);
 
}
//产生IIC起始信号
void MPU_IIC_Start(int imuNum)
{
	// MPU_SDA_OUT();     //sda线输出
	// MPU_IIC_SDA=1;	  	  
	// MPU_IIC_SCL=1;
	// MPU_IIC_Delay();
 	// MPU_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	// MPU_IIC_Delay();
	// MPU_IIC_SCL=0;//钳住I2C总线，准备发送或接收数据

	// MPU_SDA_OUT();		// SDA线输出

	// IMU 0 1 2 3 顺序接在PA上
	// int scl = 2 * imuNum + 1;
	// int sda = 2 * (imuNum + 1);
	// PAout(sda) = 1;
	// PAout(scl) = 1;
	// MPU_IIC_Delay();
	// PAout(sda) = 0;
	// MPU_IIC_Delay();
	// PAout(scl) = 0;
	// 适用于一般情况
	switch (imuNum)
	{
	case 0:
		MPU_SDA_OUT_0();
		MPU_IIC_SDA_0(1);
		MPU_IIC_SCL_0(1);
		MPU_IIC_Delay();
		MPU_IIC_SDA_0(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_0(0);
		break;
	case 1:
		MPU_SDA_OUT_1();
		MPU_IIC_SDA_1(1);
		MPU_IIC_SCL_1(1);
		MPU_IIC_Delay();
		MPU_IIC_SDA_1(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_1(0);
		break;
	case 2:
		MPU_SDA_OUT_2();
		MPU_IIC_SDA_2(1);
		MPU_IIC_SCL_2(1);
		MPU_IIC_Delay();
		MPU_IIC_SDA_2(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_2(0);
		break;
	case 3:
		MPU_SDA_OUT_3();
		MPU_IIC_SDA_3(1);
		MPU_IIC_SCL_3(1);
		MPU_IIC_Delay();
		MPU_IIC_SDA_3(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_3(0);
		break;
	default:
		break;
	}
}	  
//产生IIC停止信号
void MPU_IIC_Stop(int imuNum)
{
	// MPU_SDA_OUT();//sda线输出
	// MPU_IIC_SCL=0;
	// MPU_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	// MPU_IIC_Delay();
	// MPU_IIC_SCL=1;  
	// MPU_IIC_SDA=1;//发送I2C总线结束信号
	// MPU_IIC_Delay();

	switch (imuNum)
	{
	case 0:
		MPU_SDA_OUT_0();		// SDA线输出
		MPU_IIC_SCL_0(0);
		MPU_IIC_SDA_0(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_0(1);
		MPU_IIC_SDA_0(1);
		MPU_IIC_Delay();
		break;
	case 1:
		MPU_SDA_OUT_1();		// SDA线输出
		MPU_IIC_SCL_1(0);
		MPU_IIC_SDA_1(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_1(1);
		MPU_IIC_SDA_1(1);
		MPU_IIC_Delay();
		break;
	case 2:
		MPU_SDA_OUT_2();		// SDA线输出
		MPU_IIC_SCL_2(0);
		MPU_IIC_SDA_2(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_2(1);
		MPU_IIC_SDA_2(1);
		MPU_IIC_Delay();
		break;
	case 3:
		MPU_SDA_OUT_3();		// SDA线输出
		MPU_IIC_SCL_3(0);
		MPU_IIC_SDA_3(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_3(1);
		MPU_IIC_SDA_3(1);
		MPU_IIC_Delay();
		break;
	default:
		break;
	}					   	
}
// 等待应答信号到来
// 返回值：1，接收应答失败
//         0，接收应答成功
u8 MPU_IIC_Wait_Ack(int imuNum)
{
	u8 ucErrTime=0;
	switch (imuNum)
	{
	case 0:
		MPU_SDA_IN_0();      //SDA设置为输入
		MPU_IIC_SDA_0(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_0(1);
		MPU_IIC_Delay();
		while(MPU_READ_SDA_0)
		{
			ucErrTime++;
			if(ucErrTime>250)
			{
				MPU_IIC_Stop(imuNum);
				return 1;
			}
		}
		MPU_IIC_SCL_0(0);
		break;
	case 1:
		MPU_SDA_IN_1();      //SDA设置为输入
		MPU_IIC_SDA_1(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_1(1);
		MPU_IIC_Delay();
		while(MPU_READ_SDA_1)
		{
			ucErrTime++;
			if(ucErrTime>250)
			{
				MPU_IIC_Stop(imuNum);
				return 1;
			}
		}
		MPU_IIC_SCL_1(0);  
		break;
	case 2:
		MPU_SDA_IN_2();      //SDA设置为输入
		MPU_IIC_SDA_2(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_2(1);
		MPU_IIC_Delay();
		while(MPU_READ_SDA_2)
		{
			ucErrTime++;
			if(ucErrTime>250)
			{
				MPU_IIC_Stop(imuNum);
				return 1;
			}
		}
		MPU_IIC_SCL_2(0); 
		break;
	case 3:
		MPU_SDA_IN_3();      //SDA设置为输入
		MPU_IIC_SDA_3(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_3(1);
		MPU_IIC_Delay();
		while(MPU_READ_SDA_3)
		{
			ucErrTime++;
			if(ucErrTime>250)
			{
				MPU_IIC_Stop(imuNum);
				return 1;
			}
		}
		MPU_IIC_SCL_3(0); 
		break;
	default:
		break;
	}
	return 0;  
} 
// 产生ACK应答
void MPU_IIC_Ack(int imuNum)
{
	switch (imuNum)
	{
	case 0:
		MPU_IIC_SCL_0(0);
		MPU_SDA_OUT_0();
		MPU_IIC_SDA_0(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_0(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_0(0);
		break;
	case 1:
		MPU_IIC_SCL_1(0);
		MPU_SDA_OUT_1();
		MPU_IIC_SDA_1(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_1(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_1(0);
		break;
	case 2:
		MPU_IIC_SCL_2(0);
		MPU_SDA_OUT_2();
		MPU_IIC_SDA_2(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_2(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_2(0);
		break;
	case 3:
		MPU_IIC_SCL_3(0);
		MPU_SDA_OUT_3();
		MPU_IIC_SDA_3(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_3(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_3(0);
		break;
	default:
		break;
	}
}
//不产生ACK应答		    
void MPU_IIC_NAck(int imuNum)
{
	switch (imuNum)
	{
	case 0:
		MPU_IIC_SCL_0(0);
		MPU_SDA_OUT_0();
		MPU_IIC_SDA_0(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_0(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_0(0);
		break;
	case 1:
		MPU_IIC_SCL_1(0);
		MPU_SDA_OUT_1();
		MPU_IIC_SDA_1(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_1(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_1(0);
		break;
	case 2:
		MPU_IIC_SCL_2(0);
		MPU_SDA_OUT_2();
		MPU_IIC_SDA_2(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_2(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_2(0);
		break;
	case 3:
		MPU_IIC_SCL_3(0);
		MPU_SDA_OUT_3();
		MPU_IIC_SDA_3(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_3(1);
		MPU_IIC_Delay();
		MPU_IIC_SCL_3(0);
		break;
	default:
		break;
	}
}				 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void MPU_IIC_Send_Byte(int imuNum, u8 txd)
{
	u8 t;
	switch (imuNum)
	{
	case 0:
		MPU_SDA_OUT_0();
		MPU_IIC_SCL_0(0);
		for(t = 0;t < 8; t++)
		{              
			MPU_IIC_SDA_0((txd&0x80)>>7);
			txd<<=1;
			// MPU_IIC_Delay();
			MPU_IIC_SCL_0(1);
			MPU_IIC_Delay(); 
			MPU_IIC_SCL_0(0);
			MPU_IIC_Delay();
		}
		break;
	case 1:
		MPU_SDA_OUT_1();
		MPU_IIC_SCL_1(0);
		for(t = 0;t < 8; t++)
		{              
			MPU_IIC_SDA_1((txd&0x80)>>7);
			txd<<=1;
			// MPU_IIC_Delay();
			MPU_IIC_SCL_1(1);
			MPU_IIC_Delay(); 
			MPU_IIC_SCL_1(0);
			MPU_IIC_Delay();
		}
		break;
	case 2:
		MPU_SDA_OUT_2();
		MPU_IIC_SCL_2(0);
		for(t = 0;t < 8; t++)
		{              
			MPU_IIC_SDA_2((txd&0x80)>>7);
			txd<<=1;
			// MPU_IIC_Delay();
			MPU_IIC_SCL_2(1);
			MPU_IIC_Delay(); 
			MPU_IIC_SCL_2(0);
			MPU_IIC_Delay();
		}
		break;
	case 3:
		MPU_SDA_OUT_3();
		MPU_IIC_SCL_3(0);
		for(t = 0;t < 8; t++)
		{              
			MPU_IIC_SDA_3((txd&0x80)>>7);
			txd<<=1;
			// MPU_IIC_Delay();
			MPU_IIC_SCL_3(1);
			MPU_IIC_Delay(); 
			MPU_IIC_SCL_3(0);
			MPU_IIC_Delay();
		}
		break;
	default:
		break;
	} 
} 	    
// 读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 MPU_IIC_Read_Byte(int imuNum, unsigned char ack)
{
	u8 i = 0;
	unsigned char receive = 0;
	switch (imuNum)
	{
	case 0:
		MPU_SDA_IN_0();
		for(i = 0; i < 8; i++)
		{
			MPU_IIC_SCL_0(0);
			MPU_IIC_Delay();
			MPU_IIC_SCL_0(1);
			receive<<=1;
			if(MPU_READ_SDA_0)
				receive++;   
			MPU_IIC_Delay(); 
		}
		break;
	case 1:
		MPU_SDA_IN_1();
		for(i = 0; i < 8; i++)
		{
			MPU_IIC_SCL_1(0);
			MPU_IIC_Delay();
			MPU_IIC_SCL_1(1);
			receive<<=1;
			if(MPU_READ_SDA_1)
				receive++;   
			MPU_IIC_Delay(); 
		}
		break;
	case 2:
		MPU_SDA_IN_2();
		for(i = 0; i < 8; i++)
		{
			MPU_IIC_SCL_2(0);
			MPU_IIC_Delay();
			MPU_IIC_SCL_2(1);
			receive<<=1;
			if(MPU_READ_SDA_2)
				receive++;   
			MPU_IIC_Delay(); 
		}
		break;
	case 3:
		MPU_SDA_IN_3();
		for(i = 0; i < 8; i++)
		{
			MPU_IIC_SCL_3(0);
			MPU_IIC_Delay();
			MPU_IIC_SCL_3(1);
			receive<<=1;
			if(MPU_READ_SDA_3)
				receive++;   
			MPU_IIC_Delay(); 
		}
		break;
	default:
		break;
	}				 
    if (!ack)
        MPU_IIC_NAck(imuNum);		//发送nACK
    else
        MPU_IIC_Ack(imuNum);		//发送ACK   
    return receive;
}


















