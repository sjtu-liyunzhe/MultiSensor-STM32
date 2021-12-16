//FreeRTOS相关
#include "FreeRTOS.h"
#include "task.h"

//A超模块部分
#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h"
#include "key.h"
#include "bf.h"
#include "pulser.h"
#include "dac.h"
#include "timer.h"
#include "osc.h"
#include "fifo.h"
#include "ethernet.h"
#include "adc.h"
#include "exti.h"
#include "matrix.h"		//矩阵方法

#include "M8266HostIf.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"
#include "brd_cfg.h"
#include "wifi.h"

#include <stdio.h>
#include <stdlib.h>

#include "stm32f7xx_hal_rcc.h"


//肌电模块部分
#include "ads1299.h"
#include "EMGProcessing.h"

//IMU模块部分
// #include "usmart.h"
#include "mpu6050.h"
#include "inv_mpu_0.h"
#include "inv_mpu_1.h"
#include "inv_mpu_2.h"
#include "inv_mpu_3.h"
#include "inv_mpu_dmp_motion_driver_0.h"
#include "inv_mpu_dmp_motion_driver_1.h"
#include "inv_mpu_dmp_motion_driver_2.h"
#include "inv_mpu_dmp_motion_driver_3.h"
#include "IMUCollecting.h"

PLN_FILER plnf[4]; 
int index_plnf;
int initial_PLF(PLN_FILER *x); //声明滤波函数

// IMU模块
short temp = 0;

//u8, u16, u32
//(unsigned): char 8, short 16, int 32, long 32, long long 64, float 32, double 64

/******************************************* FreeRTOS参数设置（start） **************************************************/

/******* “开始”任务 ***********/
//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t Start_Task_Handler;
//任务函数
void start_task(void *pvParameters);

/******* “A超采集”任务 ***********/
//任务优先级
#define US_TASK_PRIO		3
//任务堆栈大小	
#define US_STK_SIZE 		512
//任务句柄
TaskHandle_t US_Task_Handler;
//任务函数
void US_task(void *pvParameters);

/******* “EMG采集”任务 ***********/

//任务优先级
#define EMG_TASK_PRIO		3
//任务堆栈大小	
#define EMG_STK_SIZE 		50
//任务句柄
TaskHandle_t EMG_Task_Handler;
//任务函数
void EMG_task(void *pvParameters);

/******* “IMU采集”任务 ***********/

//任务优先级
#define IMU_TASK_PRIO		3
//任务堆栈大小	
#define IMU_STK_SIZE 		512
//任务句柄
TaskHandle_t IMU_Task_Handler;
//任务函数
void IMU_task(void *pvParameters);

/******* “数据整合发送”任务 ***********/


/******************************************* FreeRTOS参数设置（end） **************************************************/

/******************************************* 主函数（start） **************************************************/

int main(void)
{
//	Cache_Enable();                 //打开L1-Cache。使用STM32自带的ADC采样时，不要打开cache，否则会出现问题
	HAL_Init();				        //初始化HAL库
	Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
	delay_init(216);				//延时函数初始化，主频216MHz

	//例程默认200M主频，SLCK=25MHz。压力测试M8266WIFI_SPI_Interface_Communication_Stress_Test()不丢帧
	//用216M主频时，用例程的默认设置时SCLK=27MHz，过不了压力测试所以初始化失败。说明以插排针的方式连接时最高只是SCLK=25MHz
	
	
	/**************** A超部分初始化 *************************/
	//Initialize Beamformer			//再往上移动
	BF_IO_Init();
	BF_STOP;						//Immediately stop Beamformer
	BF_Init();						//beamformer的初始化，用于设置波形频率和方波的个数
	delay_ms(200);

	//外设
	uart_init(921600);				//串口初始化	921600是肌电模块要求的串口发送速率，注意一下
	LED_Init();		

	EXTI_Init();					//Initialize external interrupt	

	//Initialize Wifi chip
	Wifi_Init();					//Initialize M8266WIFI
	Set_Socket();					//Setup socket connection with PC
	
	//指示灯
	LED_W(On);
	LED_R(On);
	LED_G(On);
	delay_ms(5);

	//Initialize DAC 可能会产生供压不足的问题，具体放大电压需要以实测为准，总的放大增益计算公式见AD604的datasheet中P12/20
	DAC_Init();
	DAC_Set_Vol(1, 1.75);			//PA4	VGN_A	1.67V（4MHz探头） 2V（1MHz探头）
	delay_ms(100);	////////////
	DAC_Set_Vol(2, 2.5);			//PA5	VREF_A	2.5V（4MHz探头） 2.5（1MHz探头）
	delay_ms(100);
	
	 
	//Initialize Pulser
	Pulser_IO_Init();
	ALL_CH_HZ();
	delay_ms(100);

	//Initialize Timer
	TIM3_Init(125-1, 10800-1);		//Initialize Timer 3. Timer Clock(Ft) = 108MHz. Prescaler value = 10800-1
									//Auto reload value = 125-1. Tout = ((arr+1)*(psc+1))/Ft = 125*100us = 12.5ms
									//Auto reload value = 5000-1. Tout = 5000*100us = 500ms
			


	/**************** 肌电EMG部分初始化 *************************/
	// 初始化结构体数组，用于存储EMG数据
	
	for(index_plnf=0;index_plnf<4;index_plnf++)
	{
	  initial_PLF(&plnf[index_plnf]);
	}
	ADS1299Init();// 初始化驱动ADS1299芯片的主芯片IO引脚功能
//	setDev(0x04,0,2);// 用于EMG采集，0x04代表1kHz采样率
	setDev(0x05,0,2);//0x05代表500Hz采样率
	startDev(2);								
									
	/**************** IMU 部分初始化 *************************/
	// uart3_init(115200);				// 初始化串口115200
	uart3_init(500000);
	// usmart_init(72);                // USMART初始化
	printf("MPU6050 TEST\r\n");
	MPU_IIC_Init();					// 初始化IIC总线
	printf("after init");
	// MPU DMP初始化
    // while(mpu_dmp_init_0())			
	// {
	//     printf("MPU6050_0 Error!!!\r\n");
	// 	delay_ms(500);
	// }
	while(mpu_dmp_init_1())
	{
	    printf("MPU6050_1 Error!!!\r\n");
		delay_ms(500);
	}
	while(mpu_dmp_init_2())
	{
	    printf("MPU6050_2 Error!!!\r\n");
		delay_ms(500);
	}
	while(mpu_dmp_init_3())
	{
	    printf("MPU6050_3 Error!!!\r\n");
		delay_ms(500);
	}
    printf("MPU6050 OK\r\n");
	

	/**************** 创建开始任务(FreeRTOS) *********************/
	xTaskCreate((TaskFunction_t )start_task,            //任务函数
							(const char*    )"start_task",          //任务名称
							(uint16_t       )START_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                  //传递给任务函数的参数
							(UBaseType_t    )START_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&Start_Task_Handler);   //任务句柄              
	vTaskStartScheduler();          //开启任务调度
}
/******************************************* 主函数（end） **************************************************/

/******************************************* FreeRTOS任务函数实现（start） **************************************************/

//“开始任务”任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区

    // 创建US任务
    xTaskCreate((TaskFunction_t )US_task,     	
                (const char*    )"US_task",   	
                (uint16_t       )US_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )US_TASK_PRIO,	
                (TaskHandle_t*  )&US_Task_Handler);
					
		// 创建EMG任务	
 		xTaskCreate((TaskFunction_t )EMG_task,     	
                 (const char*    )"EMG_task",   	
                 (uint16_t       )EMG_STK_SIZE, 
                 (void*          )NULL,				
                 (UBaseType_t    )EMG_TASK_PRIO,	
                 (TaskHandle_t*  )&EMG_Task_Handler);  
								
		// 创建IMU任务
		xTaskCreate((TaskFunction_t )IMU_task,     	
                (const char*    )"IMU_task",   	
                (uint16_t       )IMU_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )IMU_TASK_PRIO,	
                (TaskHandle_t*  )&IMU_Task_Handler);  						
								
		
    vTaskDelete(Start_Task_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}


//“A超采集任务”任务函数
void US_task(void *pvParameters)
{
	while(1)
	{
		u8 Ch_Num;
		LED_W_Toggle;				//LED灯闪烁（闪烁时间间隔为12.5ms*4=50ms），指示当前while循环正常工作
		
		for (Ch_Num = 1; Ch_Num < 5; Ch_Num++) //针对四通道Ch_Num < 5；针对8通道，Ch_Num < 9；如果使用以太网传输，需要使用Ch_Num < 9（应该是上位机的问题）
		{	
			//初始化ADC1通道1,以及初始化DMA
			Rheostat_Init();
			
			Start_TIM3();			//Start Timer 3
			 
			//Generate excitation pulses
			CH_TX(Ch_Num);			//Change to transmit mode 首先将pulser切换到发射状态，产生±50V的方波激励
			BF_FIRE;				//beamformer产生方波信号，传给pulser，用于激励方波
//			delay_us(1);			//Fire for 1us at 5MHz, totally 5 pulses(可用于5Mhz的超声探头 actually 4 pulses because of the excitation waveform)
			delay_us(3.2);			//Fire for 3.2us at 1.25MHz, totally 4 pulses 可用于1MHz的超声探头
			BF_STOP;				//beamformer停止产生方波信号
//			delay_us(1);			//尽量短 保证可以接收到回波信号，如果延时较长，则无法接收到完整的回波信号？这句注释掉好像没啥影响

      
			//开启pulser的接收模式，和带DMA的ADC采集功能
			FIFO_read(1000,Ch_Num);

			ALL_CH_HZ();			//Close Pulser to HZ state 关闭pulser至HZ状态，非钳位状态
			
			
			//标准化数据传输格式，增加包头包尾和通道号
			FIFO_buffer[0] = 0x00;		//Packet header 此处的采样精度为8位
			FIFO_buffer[1] = Ch_Num;
			FIFO_buffer[999] = 0xFF;	//Packet tail	16进制表示的的8位，即1111 1111
			
			
			//Transmit echo data by Wifi
//			taskENTER_CRITICAL();           //进入临界区
			Wifi_Send_Packet();	
//			taskEXIT_CRITICAL();            //退出临界区
		
			while(!Ch_Flag)
			{
				vTaskDelay(1);// 保证freeRTOS能够进行系统级别任务切换
			}			//等待标志位，保证每个通道消耗的时间都是12.5ms
			Ch_Flag = 0;				//Reset the flag of time control for each channel 
			Stop_TIM3();
		}	//采样一帧，共4个通道	
	}
}

//“EMG采集任务”任务函数
void EMG_task(void *pvParameters)
{
	u8 led_blink_num;
	while(1)
	{
		taskENTER_CRITICAL();           //进入临界区
		dataAcq();
		taskEXIT_CRITICAL();            //退出临界区
		led_blink_num++;
		delay_ms(1);// 保证freeRTOS能够正常进行系统级别的任务切换，最小是1ms的间隔
		if(led_blink_num > 200)// 200ms，肌电模块上的指示灯会闪烁，说明工作正常
		{
			led_blink_num=0;
			LED_G_Toggle;
		}
	}
}

//“IMU 采集任务”任务函数
void IMU_task(void *pvParameters)
{
	u8 t = 0;
	struct mpuData data0, data1, data2, data3;
	while(1)
	{
		// 采集数据
		// taskENTER_CRITICAL();           //进入临界区
		// // GetMPUData(0, &data0);
		GetMPUData(1, &data1);
		GetMPUData(2, &data2);
		GetMPUData(3, &data3);
		// taskEXIT_CRITICAL();            //退出临界区
		delay_ms(1);// 保证freeRTOS能够正常进行系统级别的任务切换，最小是1ms的间隔
//		// 打印数据
		// if((t % 2) == 0)
		// {
		// 	// PrintData(0, &data0);
		// 	PrintData(1, &data1);
		// 	PrintData(2, &data2);
		// 	PrintData(3, &data3);
		// }
		// 将数据发送给上位机
		IMUSendData(1, &data1);
		IMUSendData(2, &data2);
		IMUSendData(3, &data3);
		t++;
	}
}

/******************************************* FreeRTOS任务函数实现（end） **************************************************/


/*
注意事项：
1、肌电采集过程中会出现偶尔的脉冲式噪声，这个主要是由于freeRTOS进行任务切换导致的，当肌电采样频率高于1kHz时，会比较明显，当设置为500Hz时，几乎就没有了。
2、为了保证freeRTOS能够正常进行系统级别的任务切换，得保证每个任务中都得有至少1个1ms以上的延时。这里把delay_ms延时函数包装成了调用vTaskDelay。所以delay_ms(1) = vTaskDelay(1)。不要出现空的死循环语句，如while(判断)；
3、任务优先级的数字越大，优先级别越高，0为空闲优先级，也就是所有任务都不执行的时候才会执行优先级别为0的任务。最大为32。如果没有特殊需求，尽量把采集任务都设置成相同优先级，否则低优先级会出现比较严重的丢包问题

*/
