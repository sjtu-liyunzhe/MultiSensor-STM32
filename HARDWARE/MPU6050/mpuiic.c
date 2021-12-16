#include "mpuiic.h"
#include "delay.h"
#include "sys.h"
// #include "key.h"
#include "usart.h"
// #include "usmart.h"
//MPU IIC ��ʱ����
void MPU_IIC_Delay(void)
{
	delay_us(2);
}

//��ʼ��IIC
void MPU_IIC_Init(void)
{
	printf("in iic init");
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOD_CLK_ENABLE();           // ����GPIODʱ��
	 
    GPIO_Initure.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  // �������
    GPIO_Initure.Pull = GPIO_PULLUP;          // ����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    	// ����
	
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);// �����	
	
	HAL_GPIO_Init(GPIOD, &GPIO_Initure);
 
}
//����IIC��ʼ�ź�
void MPU_IIC_Start(int imuNum)
{
	// MPU_SDA_OUT();     //sda�����
	// MPU_IIC_SDA=1;	  	  
	// MPU_IIC_SCL=1;
	// MPU_IIC_Delay();
 	// MPU_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	// MPU_IIC_Delay();
	// MPU_IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ��������

	// MPU_SDA_OUT();		// SDA�����

	// IMU 0 1 2 3 ˳�����PA��
	// int scl = 2 * imuNum + 1;
	// int sda = 2 * (imuNum + 1);
	// PAout(sda) = 1;
	// PAout(scl) = 1;
	// MPU_IIC_Delay();
	// PAout(sda) = 0;
	// MPU_IIC_Delay();
	// PAout(scl) = 0;
	// ������һ�����
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
//����IICֹͣ�ź�
void MPU_IIC_Stop(int imuNum)
{
	// MPU_SDA_OUT();//sda�����
	// MPU_IIC_SCL=0;
	// MPU_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	// MPU_IIC_Delay();
	// MPU_IIC_SCL=1;  
	// MPU_IIC_SDA=1;//����I2C���߽����ź�
	// MPU_IIC_Delay();

	switch (imuNum)
	{
	case 0:
		MPU_SDA_OUT_0();		// SDA�����
		MPU_IIC_SCL_0(0);
		MPU_IIC_SDA_0(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_0(1);
		MPU_IIC_SDA_0(1);
		MPU_IIC_Delay();
		break;
	case 1:
		MPU_SDA_OUT_1();		// SDA�����
		MPU_IIC_SCL_1(0);
		MPU_IIC_SDA_1(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_1(1);
		MPU_IIC_SDA_1(1);
		MPU_IIC_Delay();
		break;
	case 2:
		MPU_SDA_OUT_2();		// SDA�����
		MPU_IIC_SCL_2(0);
		MPU_IIC_SDA_2(0);
		MPU_IIC_Delay();
		MPU_IIC_SCL_2(1);
		MPU_IIC_SDA_2(1);
		MPU_IIC_Delay();
		break;
	case 3:
		MPU_SDA_OUT_3();		// SDA�����
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
// �ȴ�Ӧ���źŵ���
// ����ֵ��1������Ӧ��ʧ��
//         0������Ӧ��ɹ�
u8 MPU_IIC_Wait_Ack(int imuNum)
{
	u8 ucErrTime=0;
	switch (imuNum)
	{
	case 0:
		MPU_SDA_IN_0();      //SDA����Ϊ����
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
		MPU_SDA_IN_1();      //SDA����Ϊ����
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
		MPU_SDA_IN_2();      //SDA����Ϊ����
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
		MPU_SDA_IN_3();      //SDA����Ϊ����
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
// ����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
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
// ��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
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
        MPU_IIC_NAck(imuNum);		//����nACK
    else
        MPU_IIC_Ack(imuNum);		//����ACK   
    return receive;
}


















