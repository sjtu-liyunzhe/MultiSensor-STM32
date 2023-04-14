//FreeRTOS相关
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "semphr.h"

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

#include "cQueue.h"

#define CIRCLE_SIZE 50
#define EMG_QUEUE_SIZE 50
// #define EMG_QUEUE_SIZE 25
#define IMU_QUEUE_SIZE 10
// #define EMG_SEND_PACKAGE_SIZE 67
#define EMG_SEND_PACKAGE_SIZE 107

// #define IMU_SEND_PACKAGE_SIZE 67
#define IMU_SEND_PACKAGE_SIZE 107

// 全局变量
PLN_FILER plnf[4]; 
IIR_FILER iir_filter_array[4];
COMB_FILER comb_filter_array[4];
int index_plnf;
int initial_PLF(PLN_FILER *x); //声明滤波函数
unsigned char initial_IIR_Filter(IIR_FILER *);
unsigned char initial_COMB_Filter(COMB_FILER *);
cLinkQueue* EMGLinkQueueArray[4] = {NULL, NULL,NULL, NULL};
volatile cCircleQueue* EMGCircleQueueArray[4] = {NULL, NULL, NULL, NULL};
IMULinkQueue* imuLinkQueue_0, * imuLinkQueue_1, * imuLinkQueue_2, * imuLinkQueue_3;
IMULinkQueue** imuLinkQueueArray;
volatile IMUCircleQueue* IMUCircleQueueArray[4] = {NULL, NULL, NULL, NULL};
uint8_t sumEMGData = 0;
uint8_t packageNum = 0;
uint8_t serialPackage[26];
uint8_t errorMessage[66];
// IMULinkQueue** imuLinkQueueArray;
bool initQueues();
bool initCircleQueues();
// IMU模块
short temp = 0;

// EMG-IMU联合
void doInitEMGIMU();
uint8_t unionSerialPackage[26];
//u8, u16, u32
//(unsigned): char 8, short 16, int 32, long 32, long long 64, float 32, double 64

/******************************************* FreeRTOS参数设置（start） **************************************************/

// 互斥信号量句柄
SemaphoreHandle_t xSemaphore = NULL;
SemaphoreHandle_t xSemaphore_IMU = NULL;
/******* “开始”任务 ***********/
//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		1024
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
#define EMG_STK_SIZE 		1024
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

/******* “串口发送”任务 ***********/

//任务优先级
#define Serial_TASK_PRIO		3
//任务堆栈大小	
#define Serial_STK_SIZE 		1024 * 4
//任务句柄
TaskHandle_t Serial_Task_Handler;
//任务函数
void Serial_task(void *pvParameters);
void test_Serial_task(void *pvParameters);

/******* 定时器任务 ***********/
#define TIMERCONTROL_TASK_PRIO  3
#define TIMERCONTROL_STK_SIZE   1024
TaskHandle_t TimerControlTask_Handler;
void timerControl_task(void* pvParameters);

TimerHandle_t EMG_Timer_Handle;
void EMG_Timer_Callback(TimerHandle_t xTimer);


/******* “数据整合发送”任务 ***********/


/******* “IMU-EMG联合采集”任务 ***********/
// //任务优先级
// #define EMG_IMU_TASK_PRIO		3
// //任务堆栈大小	
// #define EMG_IMU_STK_SIZE 		1024 * 4
// //任务句柄
// TaskHandle_t EMG_IMU_Task_Handler;
// //任务函数
// void EMG_IMU_task(void *pvParameters);

/******************************************* FreeRTOS参数设置（end） **************************************************/

/******************************************* 主函数（start） **************************************************/

int main(void)
{
	xSemaphore = xSemaphoreCreateMutex();
	xSemaphore_IMU = xSemaphoreCreateMutex();
	
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
	for(int i = 0; i < 4; ++i)
	{
		initial_IIR_Filter(&iir_filter_array[i]);
		initial_COMB_Filter(&comb_filter_array[i]);
	}
	ADS1299Init();// 初始化驱动ADS1299芯片的主芯片IO引脚功能
	setDev(0x04,0,2);// 用于EMG采集，0x04代表1kHz采样率
	// setDev(0x05,0,2);//0x05代表500Hz采样率
	startDev(2);
	initErrorMessage();							
									
	/**************** IMU 部分初始化 *************************/
	// uart3_init(115200);				// 初始化串口115200
	uart3_init(921600);
//	uart3_init(500000);
	// usmart_init(72);                // USMART初始化
	printf("MPU6050 TEST\r\n");
	// MPU_IIC_Init();					// 初始化IIC总线
	MPU_Init();
	// printf("after init");
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
	// while(mpu_dmp_init_2())
	// {
	//     printf("MPU6050_2 Error!!!\r\n");
	// 	delay_ms(500);
	// }
	// while(mpu_dmp_init_3())
	// {
	//     printf("MPU6050_3 Error!!!\r\n");
	// 	delay_ms(500);
	// }
    printf("MPU6050 OK\r\n");
	// if(!initQueues())
	// 	printf("init fails");
	if(!initCircleQueues())
		printf("init fails");
	printf("begin tasks");

	/**************** 创建开始任务(FreeRTOS) *********************/
	xTaskCreate((TaskFunction_t )start_task,            //任务函数
							(const char*    )"start_task",          //任务名称
							(uint16_t       )START_STK_SIZE,        //任务堆栈大小
							(void*          )NULL,                  //传递给任务函数的参数
							(UBaseType_t    )START_TASK_PRIO,       //任务优先级
							(TaskHandle_t*  )&Start_Task_Handler);   //任务句柄              
	
	vTaskStartScheduler();          //开启任务调度
	

	/**************** 创建定时器(FreeRTOS) *********************/
	
}
/******************************************* 主函数（end） **************************************************/

/******************************************* FreeRTOS任务函数实现（start） **************************************************/

//“开始任务”任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	// 定时器任务
	EMG_Timer_Handle = xTimerCreate(
					(const char*) 			  	"EMG_timer_task",
					(TickType_t) 			  	5,
					(UBaseType_t) 			  	pdTRUE,			// 周期定时器
					(void*)					  	1,
					(TimerCallbackFunction_t) 	EMG_Timer_Callback);


//	// 创建定时器任务
	xTaskCreate((TaskFunction_t )timerControl_task,     	
                (const char*    )"timerControl_task",   	
                (uint16_t       )TIMERCONTROL_STK_SIZE,
                (void*          )NULL,				
                (UBaseType_t    )TIMERCONTROL_TASK_PRIO,	
                (TaskHandle_t*  )&TimerControlTask_Handler);


    // 创建US任务
    xTaskCreate((TaskFunction_t )US_task,     	
                (const char*    )"US_task",   	
                (uint16_t       )US_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )US_TASK_PRIO,	
                (TaskHandle_t*  )&US_Task_Handler);
					
	// 创建EMG任务	
	// xTaskCreate((TaskFunction_t )EMG_task,     	
	// 			(const char*    )"EMG_task",   	
	// 			(uint16_t       )EMG_STK_SIZE, 
	// 			(void*          )NULL,				
	// 			(UBaseType_t    )EMG_TASK_PRIO,	
	// 			(TaskHandle_t*  )&EMG_Task_Handler);  
								
	// 创建IMU任务
	xTaskCreate((TaskFunction_t )IMU_task,     	
				(const char*    )"IMU_task",   	
				(uint16_t       )IMU_STK_SIZE, 
				(void*          )NULL,				
				(UBaseType_t    )IMU_TASK_PRIO,	
				(TaskHandle_t*  )&IMU_Task_Handler); 	

//	// 创建串口发送数据任务
	xTaskCreate((TaskFunction_t )test_Serial_task,
				(const char*    )"test_Serial_task",
				(uint16_t       )Serial_STK_SIZE,
				(void*          )NULL,
				(UBaseType_t    )Serial_TASK_PRIO,	
				(TaskHandle_t*  )&Serial_Task_Handler);
	
	// // EMG IMU联合采集
	// xTaskCreate((TaskFunction_t )EMG_IMU_task,
	// 			(const char*    )"EMG_IMU_task",
	// 			(uint16_t       )Serial_STK_SIZE,
	// 			(void*          )NULL,
	// 			(UBaseType_t    )EMG_IMU_TASK_PRIO,	
	// 			(TaskHandle_t*  )&EMG_IMU_Task_Handler);
							
		
    vTaskDelete(Start_Task_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

bool timerFlag = true;
void timerControl_task(void *pvParameters)
{
	while(1)
	{
		if(timerFlag)
		{
			if(EMG_Timer_Handle != NULL)
			{
				xTimerStart(EMG_Timer_Handle, 0);
				timerFlag = false;
			}
			else
				printf("Timer NULL\r\n");
		}
		delay_ms(1);
		// vTaskDelay(1 / portTICK_PERIOD_MS);	
	}
}

int count = 0;
u8 led_blink_num;
void EMG_Timer_Callback(TimerHandle_t xTimer)
{
	// dataAcq();
	dataAcq_16bit();
	// dataAcq_24bit();
	led_blink_num++;
	if(led_blink_num > 200)
	{
		led_blink_num = 0;
		LED_G_Toggle;
		// printf("timer!\r\n");
	}
}


//“A超采集任务”任务函数
void US_task(void *pvParameters)
{
	while(1)
	{
		u8 Ch_Num;
		LED_W_Toggle;				//LED灯闪烁（闪烁时间间隔为12.5ms*4=50ms），指示当前while循环正常工作
		
		for (Ch_Num = 1; Ch_Num < 5; ++Ch_Num) //针对四通道Ch_Num < 5；针对8通道，Ch_Num < 9；如果使用以太网传输，需要使用Ch_Num < 9（应该是上位机的问题）
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
				// vTaskDelay(1);// 保证freeRTOS能够进行系统级别任务切换
				delay_ms(1);
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
	struct mpuData data0, data1, data2, data3;
	int count = 0;
	while(1)
	{
		// taskENTER_CRITICAL();           //进入临界区
		// xSemaphoreTake(xSemaphore, portMAX_DELAY);		// 等待互斥量
		dataAcq();
		// printf("in");
		// if(count < 10)
			// printf("in %d", dataAcq());
		// taskEXIT_CRITICAL();            //退出临界区
		// delay_ms(2);
		// GetMPUData(1, &data1);
		// GetMPUData(2, &data2);
		// GetMPUData(3, &data3);
		// xSemaphoreGive(xSemaphore);		// 互斥信号量释放
		led_blink_num++;
		// delay_ms(1);// 保证freeRTOS能够正常进行系统级别的任务切换，最小是1ms的间隔
		// delay_ms(1);
		// delay_us(100);
		if(led_blink_num > 200)// 200ms，肌电模块上的指示灯会闪烁，说明工作正常
		{
			led_blink_num=0;
			LED_G_Toggle;
		}
		count++;
	}
}
// 测试一下delay_ms函数
// void IMU_task(void *pvParameters)
// {
// 	u8 led_blink_num;
// 	while(1)
// 	{
// 		led_blink_num++;
// 		delay_ms(1);
// 		if(led_blink_num > 200)// 200ms，肌电模块上的指示灯会闪烁，说明工作正常
// 		{
// 			led_blink_num=0;
// 			LED_G_Toggle;
// 		}
// 	}
// }
//“IMU 采集任务”任务函数
void IMU_task(void *pvParameters)
{
	u8 t = 0;
	volatile struct mpuData data0, data1, data2, data3;
	while(1)
	{
		// 采集数据
		// taskENTER_CRITICAL();           //进入临界区
		// delay_ms(100);
		// // GetMPUData(0, &data0);
		GetMPUData(1, &data1);
		// delay_ms(1);
		GetMPUData(2, &data2);
		GetMPUData(3, &data3);
		// taskEXIT_CRITICAL();            //退出临界区
		// printf("imu\r\n");
//		// 打印数据
		// if((t % 2) == 0)
		// {
		// 	PrintData(0, &data0);
		// 	PrintData(1, &data1);
		// 	PrintData(2, &data2);
		// 	PrintData(3, &data3);
		// }
		// 将数据发送给上位机
		// IMUSendData(1, &data1);
		// IMUSendData(2, &data2);
		// IMUSendData(3, &data3);
		// xSemaphoreTake(xSemaphore_IMU, portMAX_DELAY);		// 等待互斥量
		IMUSaveData(0, &data1);
		IMUSaveData(1, &data2);
		IMUSaveData(2, &data3);
		// xSemaphoreGive(xSemaphore_IMU);		// 互斥信号量释放
		// delay_ms(1);// 保证freeRTOS能够正常进行系统级别的任务切换，最小是1ms的间隔
		vTaskDelay(1);
		t++;
	}
}

void Serial_task(void *pvParameters)
{
	int16_t EMGdata[4] = {0, 0, 0, 0};
	uint16_t EMGdata2[4] = {0, 0, 0, 0};
	int16_t result = 0;
	while(1)
	{

		if(EMGCircleQueueArray[0]->size > EMG_QUEUE_SIZE)
		{
			for(int i = 0; i < EMG_QUEUE_SIZE; ++i)
			{
				for(u8 emgIndex = 0; emgIndex != 4; ++emgIndex)
				{
					EMGdata[emgIndex] = popCircleQueue(EMGCircleQueueArray[emgIndex]);
					// IIR_Filter(&iir_filter_array[emgIndex], EMGdata[emgIndex], &result);
					// EMGdata2[emgIndex] = (uint16_t)(EMGdata[emgIndex] & 0xFFF);
					EMGdata2[emgIndex] = (uint16_t)(result & 0xFFF);
				}
				sendPackage(EMGdata2);
			}
			vTaskDelay(1);
			// printf("");
		}
		
	}
}

// void Serial_task(void *pvParameters)
// {
// 	serialPackage[0] = 0x0D;
// 	serialPackage[1] = 0x0A;
// 	int16_t tempRoll, tempPitch, tempYaw;
// 	int count = 0;
// 	// delay_ms(1000);
// 	while(1)
// 	{
// 		// taskENTER_CRITICAL();           //进入临界区
// 		xSemaphoreTake(xSemaphore, portMAX_DELAY);		// 等待互斥信号量
// 		printf("task\n");
// 		// pushLinkQueue(EMGLinkQueueArray[0], (int16_t)count);
// 		// popLinkQueue(EMGLinkQueueArray[0]);
// 		for(int i = 0; i < 4; ++i)
// 		{
// 			// pushLinkQueue(EMGLinkQueueArray[i], 88);
// 			// if(!isLinkQueueEmpty(EMGLinkQueueArray[i]))
// 			// 	// serialPackage[2 + i] = EMGLinkQueueArray[i]->front->data;
// 			// 	// popLinkQueue(EMGLinkQueueArray[i]);
// 			// 	serialPackage[2 + i] = (((unsigned short)popLinkQueue(EMGLinkQueueArray[i])) >> 4) & 0xFF;

// 			if(!isCircleQueueEmpty(EMGCircleQueueArray[i]))
// 				// serialPackage[2 + i] = EMGLinkQueueArray[i]->front->data;
// 				// popLinkQueue(EMGLinkQueueArray[i]);
// 				serialPackage[2 + i] = (((unsigned short)popCircleQueue(EMGCircleQueueArray[i])) >> 4) & 0xFF;
			

// 			sumEMGData += serialPackage[2 + i];
// 		}
// 		printf("maxsize:%d  ", EMGCircleQueueArray[0]->maxSize);
// 		count++;
// 		printf("data:%d\n  ", serialPackage[2]);
// 		printf("size:%d\n  ", EMGCircleQueueArray[0]->size);
// 		// taskEXIT_CRITICAL();            //退出临界区
// 		xSemaphoreGive(xSemaphore);
// 		delay_ms(1);
		
// 		serialPackage[6] = sumEMGData;
// 		serialPackage[7] = packageNum;
// 		// xSemaphoreTake(xSemaphore_IMU, portMAX_DELAY);		// 等待互斥量
// 		// for(int i = 1; i < 4; ++i)	// 这里只用了1-3号IMU，没有用0
// 		// {

// 		// 	if(!isLinkQueueEmpty(imuLinkQueueArray[i]->rowQueue))
// 		// 		tempRoll = popLinkQueue(imuLinkQueueArray[i]->rowQueue);
// 		// 	if(!isLinkQueueEmpty(imuLinkQueueArray[i]->pitchQueue))
// 		// 		tempPitch = popLinkQueue(imuLinkQueueArray[i]->pitchQueue);
// 		// 	if(!isLinkQueueEmpty(imuLinkQueueArray[i]->yawQueue))
// 		// 		tempYaw = popLinkQueue(imuLinkQueueArray[i]->yawQueue);
// 		// 	serialPackage[6 * i + 2] = (tempRoll >> 8) & 0xFF;	// rollHigh
// 		// 	serialPackage[6 * i + 3] = tempRoll & 0xFF;			// rollLow
// 		// 	serialPackage[6 * i + 4] = (tempPitch >> 8) & 0xFF;
// 		// 	serialPackage[6 * i + 5] = tempPitch & 0xFF;
// 		// 	serialPackage[6 * i + 6] = (tempYaw >> 8) & 0xFF;
// 		// 	serialPackage[6 * i + 7] = tempYaw & 0xFF;
// 		// }
// 		// xSemaphoreGive(xSemaphore_IMU);		// 互斥信号量释放
		
// 		// usart3_SendPackage(serialPackage, 26);
// 		packageNum++;
// 		if(packageNum > 256)
// 			packageNum = 0;
// 		delay_ms(1);
// 	}
	
// }
uint8_t testSendPackage[EMG_SEND_PACKAGE_SIZE];
uint8_t imuRowSendPackage[EMG_SEND_PACKAGE_SIZE];
uint8_t imuPitchSendPackage[EMG_SEND_PACKAGE_SIZE];
uint8_t imuYawSendPackage[EMG_SEND_PACKAGE_SIZE];
uint8_t imuTotalSendPackage[IMU_SEND_PACKAGE_SIZE];
void test_Serial_task(void *pvParameters)
{
	u8 count = 0;
	int16_t tempRoll, tempPitch, tempYaw;
	uint8_t EMGPackageNum = 0;
	uint8_t IMUPackageNum = 0;
	testSendPackage[0] = 0x0D;
	testSendPackage[2] = 0x32;		// 数据长度为50
	testSendPackage[EMG_SEND_PACKAGE_SIZE - 1] = 0x0A;
	testSendPackage[EMG_SEND_PACKAGE_SIZE - 2] = 0xFF;
	testSendPackage[EMG_SEND_PACKAGE_SIZE - 3] = 0x88;
	imuRowSendPackage[0] = 0x0E;
	imuRowSendPackage[EMG_SEND_PACKAGE_SIZE - 1] = 0x0B;
	imuRowSendPackage[EMG_SEND_PACKAGE_SIZE - 2] = 0xFF;
	imuRowSendPackage[EMG_SEND_PACKAGE_SIZE - 3] = 0x88;
	imuPitchSendPackage[0] = 0x0F;
	imuPitchSendPackage[EMG_SEND_PACKAGE_SIZE - 1] = 0x0C;
	imuPitchSendPackage[EMG_SEND_PACKAGE_SIZE - 2] = 0xFF;
	imuPitchSendPackage[EMG_SEND_PACKAGE_SIZE - 3] = 0x88;
	imuYawSendPackage[0] = 0x10;
	imuYawSendPackage[EMG_SEND_PACKAGE_SIZE - 1] = 0x0D;
	imuYawSendPackage[EMG_SEND_PACKAGE_SIZE - 2] = 0xFF;
	imuYawSendPackage[EMG_SEND_PACKAGE_SIZE - 3] = 0x88;

	imuTotalSendPackage[0] = 0x0B;
	imuTotalSendPackage[2] = 0x3C;		// 数据长度为60
	imuTotalSendPackage[IMU_SEND_PACKAGE_SIZE - 1] = 0x0E;
	imuTotalSendPackage[IMU_SEND_PACKAGE_SIZE - 2] = 0xFF;
	imuTotalSendPackage[IMU_SEND_PACKAGE_SIZE - 3] = 0x88;

	while(1)
	{
		// printf("int serial\n");
		// for(u8 emgIndex = 0; emgIndex != 4; ++emgIndex)
		// {
		// 	if(EMGCircleQueueArray[emgIndex]->size > EMG_QUEUE_SIZE)
		// 	{
		// 		testSendPackage[1] = emgIndex;
		// 		for(int i = 0; i < EMG_QUEUE_SIZE; ++i)
		// 		{
		// 			testSendPackage[3 + i] = popCircleQueue(EMGCircleQueueArray[emgIndex]);
		// 		}
		// 		testSendPackage[EMG_SEND_PACKAGE_SIZE - 4] = EMGPackageNum;
		// 		usart3_SendPackage(testSendPackage, EMG_SEND_PACKAGE_SIZE);
		// 		++EMGPackageNum;
		// 		if(EMGPackageNum > 256)
		// 			EMGPackageNum = 0;
		// 		delay_ms(1);
		// 		// printf("");
		// 	}
		// }

		// 传输16bit的EMG数据
		for(u8 emgIndex = 0; emgIndex < 4; ++emgIndex)
		{
			if(EMGCircleQueueArray[emgIndex]->size > EMG_QUEUE_SIZE)
			{
				testSendPackage[1] = emgIndex;
				for(int i = 0; i < EMG_QUEUE_SIZE; ++i)
				{
					int16_t tmp = popCircleQueue(EMGCircleQueueArray[emgIndex]);
					testSendPackage[3 + 2 * i] = (tmp >> 8) & 0xFF;
					testSendPackage[4 + 2 * i] = tmp & 0xFF;
				}
				testSendPackage[EMG_SEND_PACKAGE_SIZE - 4] = EMGPackageNum;
				usart3_SendPackage(testSendPackage, EMG_SEND_PACKAGE_SIZE);
				++EMGPackageNum;
				if(EMGPackageNum > 256)
					EMGPackageNum = 0;
				delay_ms(1);
			}
		}

		for(u8 imuIndex = 0; imuIndex != 3; ++imuIndex)
		{
			if(IMUCircleQueueArray[imuIndex]->rowQueue->size > IMU_QUEUE_SIZE &&
			   IMUCircleQueueArray[imuIndex]->pitchQueue->size > IMU_QUEUE_SIZE &&
			   IMUCircleQueueArray[imuIndex]->yawQueue->size > IMU_QUEUE_SIZE)
			{
				imuTotalSendPackage[1] = imuIndex;
				for(u8 i = 0; i != IMU_QUEUE_SIZE; ++i)
				{
					tempRoll = popCircleQueue(IMUCircleQueueArray[imuIndex]->rowQueue);
					imuTotalSendPackage[2 * i + 3] = (tempRoll >> 8) & 0xFF;		// rollHigh
					imuTotalSendPackage[2 * i + 4] = tempRoll & 0xFF;
				}
				for(u8 i = 0; i != IMU_QUEUE_SIZE; ++i)
				{
					tempPitch = popCircleQueue(IMUCircleQueueArray[imuIndex]->pitchQueue);
					imuTotalSendPackage[2 * i + 23] = (tempPitch >> 8) & 0xFF;
					imuTotalSendPackage[2 * i + 24] = tempPitch & 0xFF;
				}
				for(u8 i = 0; i != IMU_QUEUE_SIZE; ++i)
				{
					tempYaw = popCircleQueue(IMUCircleQueueArray[imuIndex]->yawQueue);
					imuTotalSendPackage[2 * i + 43] = (tempYaw >> 8) & 0xFF;
					imuTotalSendPackage[2 * i + 44] = tempYaw & 0xFF;
				}
				imuTotalSendPackage[IMU_SEND_PACKAGE_SIZE - 4] = IMUPackageNum;
				usart3_SendPackage(imuTotalSendPackage, IMU_SEND_PACKAGE_SIZE);
				++IMUPackageNum;
				if(IMUPackageNum > 256)
					IMUPackageNum = 0;
				delay_ms(1);
			}
		}
		// for(u8 imuIndex = 0; imuIndex != 3; ++imuIndex)
		// {
		// 	if(IMUCircleQueueArray[imuIndex]->rowQueue->size > IMU_QUEUE_SIZE)
		// 	{
		// 		imuRowSendPackage[1] = imuIndex;
		// 		for(u8 i = 0; i != IMU_QUEUE_SIZE; ++i)
		// 		{
		// 			tempRoll = popCircleQueue(IMUCircleQueueArray[imuIndex]->rowQueue);
		// 			imuRowSendPackage[2 * (i + 1)] = (tempRoll >> 8) & 0xFF;		// rollHigh
		// 			imuRowSendPackage[2 * (i + 1) + 1] = tempRoll & 0xFF;			// rollLow
		// 		}
		// 		usart3_SendPackage(imuRowSendPackage, EMG_SEND_PACKAGE_SIZE);
		// 		// printf("");
		// 	}
		// 	if(IMUCircleQueueArray[imuIndex]->pitchQueue->size > IMU_QUEUE_SIZE)
		// 	{
		// 		imuPitchSendPackage[1] = imuIndex;
		// 		for(u8 i = 0; i != IMU_QUEUE_SIZE; ++i)
		// 		{
		// 			tempPitch = popCircleQueue(IMUCircleQueueArray[imuIndex]->pitchQueue);
		// 			imuPitchSendPackage[2 * (i + 1)] = (tempPitch >> 8) & 0xFF;
		// 			imuPitchSendPackage[2 * (i + 1) + 1] = tempPitch & 0xFF;
		// 		}
		// 		usart3_SendPackage(imuPitchSendPackage, EMG_SEND_PACKAGE_SIZE);
		// 		// printf("");
		// 	}
		// 	if(IMUCircleQueueArray[imuIndex]->yawQueue->size > IMU_QUEUE_SIZE)
		// 	{
		// 		imuYawSendPackage[1] = imuIndex;
		// 		for(u8 i = 0; i != IMU_QUEUE_SIZE; ++i)
		// 		{
		// 			tempYaw = popCircleQueue(IMUCircleQueueArray[imuIndex]->yawQueue);
		// 			imuYawSendPackage[2 * (i + 1)] = (tempYaw >> 8) & 0xFF;
		// 			imuYawSendPackage[2 * (i + 1) + 1] = tempYaw & 0xFF;
		// 		}
		// 		usart3_SendPackage(imuYawSendPackage, EMG_SEND_PACKAGE_SIZE);
		// 		// printf("");
		// 	}
		// }
		// if(++count > 200)
		// {
		// 	for(int emgIndex = 0; emgIndex != 4; ++emgIndex)
		// 	{
		// 		printf("emg[%d]now size: %d  ", emgIndex, EMGCircleQueueArray[emgIndex]->size);
		// 	}
		// 	for(int imuIndex = 0; imuIndex != 3; ++imuIndex)
		// 	{
		// 		printf("imuRoll[%d]now size: %d  ", imuIndex, IMUCircleQueueArray[imuIndex]->rowQueue->size);
		// 		printf("imuPitch[%d]now size: %d  ", imuIndex, IMUCircleQueueArray[imuIndex]->pitchQueue->size);
		// 		printf("imuYaw[%d]now size: %d  ", imuIndex, IMUCircleQueueArray[imuIndex]->yawQueue->size);
		// 	}
		// 	// printf("[1]now size: %d  ", EMGCircleQueueArray[1]->size);
		// 	count = 0;
		// }
		delay_ms(1);
	}
}

// 全局变量初始化
bool initQueues()
{
	bool flag = true;
	// EMGLinkQueueArray = (cLinkQueue**)malloc(sizeof(cLinkQueue*));
	// if(!EMGLinkQueueArray)
	// 	flag = false;
	for(int i = 0; i < 4; ++i)
	{
		EMGLinkQueueArray[i] = (cLinkQueue*)malloc(sizeof(cLinkQueue));
		if(!EMGLinkQueueArray[i])
			flag = false;
		initLinkQueue(EMGLinkQueueArray[i]);
	}
	// imuLinkQueue_0 = imuLinkQueue_1 = imuLinkQueue_2 = imuLinkQueue_3 = (IMULinkQueue*)malloc(sizeof(IMULinkQueue));
	// initIMULinkQueue(imuLinkQueue_0);
	// initIMULinkQueue(imuLinkQueue_1);
	// initIMULinkQueue(imuLinkQueue_2);
	// initIMULinkQueue(imuLinkQueue_3);
	imuLinkQueueArray = (IMULinkQueue**)malloc(4 * sizeof(IMULinkQueue*));
	if(!imuLinkQueueArray)
		flag = false;
	for(int i = 0; i < 4; i++)
	{
		imuLinkQueueArray[i] = (IMULinkQueue*)malloc(sizeof(IMULinkQueue));
		if(!imuLinkQueueArray[i])
			flag = false;
		flag = initIMULinkQueue(imuLinkQueueArray[i]);
	}
	return flag;
}
bool initCircleQueues()
{
	bool flag = true;
	for(int i = 0; i < 4; ++i)
	{
		EMGCircleQueueArray[i] = (cCircleQueue*)pvPortMalloc(sizeof(cCircleQueue));
		if(!EMGCircleQueueArray[i])
			flag = false;
		flag = initCircleQueue(EMGCircleQueueArray[i], 50);
	}
	for(int i = 0; i != 4; ++i)
	{
		IMUCircleQueueArray[i] = (IMUCircleQueue*)pvPortMalloc(sizeof(IMUCircleQueue));
		if(!IMUCircleQueueArray[i])
			flag = false;
		flag = initIMUCircleQueue(IMUCircleQueueArray[i], CIRCLE_SIZE);
	}
	return flag;
}
/******************************************* FreeRTOS任务函数实现（end） **************************************************/

void doInitEMGIMU()
{

}
void EMG_IMU_task(void *pvParameters)
{
	unionSerialPackage[0] = 0x0D;
	unionSerialPackage[1] = 0x0A;
	int16_t tempRoll, tempPitch, tempYaw;
	mpuData data0, data1, data2, data3;
	uint8_t led_blink_num;
	uint8_t sumEMGData = 0;
	unsigned short* EMGPointer = NULL;
	mpuData* imuDataArray[3] = {NULL, NULL, NULL};
	int packageNum = 0;
	while(1)
	{
		// printf("in union task    ");
		// EMGPointer = (unsigned short*)pvPortMalloc(8 * sizeof(unsigned short));
		// EMGPointer = (unsigned short*)malloc(8 * sizeof(unsigned short));
		// if(EMGPointer)
		// 	EMGPointer = EMGdataAcq();
		EMGPointer = EMGdataAcq();
		// else
		// 	printf("EMGPointer malloc fails");
		for(int i = 0; i != 4; ++i)
		{
			unionSerialPackage[2 + i] = (*(EMGPointer + i) >> 4) & 0xFF;
			sumEMGData += EMGPointer[i];
		}
		unionSerialPackage[6] = sumEMGData;
		unionSerialPackage[7] = packageNum;
		// vPortFree(EMGPointer);
		// free(EMGPointer);
		// GetMPUData(0, &data0);
		GetMPUData(1, &data1);
		GetMPUData(2, &data2);
		GetMPUData(3, &data3);
		// for(int i = 0; i != 3; ++i)
		// {
		// 	// imuDataArray[i] = (mpuData*)pvPortMalloc(sizeof(mpuData));
		// 	imuDataArray[i] = (mpuData*)malloc(sizeof(mpuData));
		// 	if(!imuDataArray[i])
		// 		printf("imuDataArray[%d] malloc fails   ", i);
		// }
		imuDataArray[0] = &data1; imuDataArray[1] = &data2; imuDataArray[2] = &data3;
		for(int i = 1; i != 4; ++i)
		{
			tempRoll = (int16_t)(100 * imuDataArray[i - 1]->roll);
			tempPitch = (int16_t)(100 * imuDataArray[i - 1]->pitch);
			tempYaw = (int16_t)(100 * imuDataArray[i - 1]->yaw);
			unionSerialPackage[6 * i + 2] = (tempRoll >> 8) & 0xFF;	// rollHigh
		 	unionSerialPackage[6 * i + 3] = tempRoll & 0xFF;			// rollLow
			unionSerialPackage[6 * i + 4] = (tempPitch >> 8) & 0xFF;
		 	unionSerialPackage[6 * i + 5] = tempPitch & 0xFF;
		 	unionSerialPackage[6 * i + 6] = (tempYaw >> 8) & 0xFF;
		 	unionSerialPackage[6 * i + 7] = tempYaw & 0xFF;
		}
		// for(int i = 8; i < 26; ++i)
		// {
		// 	unionSerialPackage[i] = 0;
		// }

		// for(int i = 0; i != 3; ++i)
		// {
		// 	// vPortFree(imuDataArray[i]);
		// 	free(imuDataArray[i]);
		// }

		usart3_SendPackage(unionSerialPackage, 26);

		++led_blink_num;
		++packageNum;
		delay_ms(1);
		if(led_blink_num > 200)
		{
			led_blink_num = 0;
			LED_G_Toggle;
		}
	}
}
/*
注意事项：
1、肌电采集过程中会出现偶尔的脉冲式噪声，这个主要是由于freeRTOS进行任务切换导致的，当肌电采样频率高于1kHz时，会比较明显，当设置为500Hz时，几乎就没有了。
2、为了保证freeRTOS能够正常进行系统级别的任务切换，得保证每个任务中都得有至少1个1ms以上的延时。这里把delay_ms延时函数包装成了调用vTaskDelay。所以delay_ms(1) = vTaskDelay(1)。不要出现空的死循环语句，如while(判断)；
3、任务优先级的数字越大，优先级别越高，0为空闲优先级，也就是所有任务都不执行的时候才会执行优先级别为0的任务。最大为32。如果没有特殊需求，尽量把采集任务都设置成相同优先级，否则低优先级会出现比较严重的丢包问题

*/
