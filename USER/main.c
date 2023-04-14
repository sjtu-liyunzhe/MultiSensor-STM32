//FreeRTOS���
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "semphr.h"

//A��ģ�鲿��
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
#include "matrix.h"		//���󷽷�

#include "M8266HostIf.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"
#include "brd_cfg.h"
#include "wifi.h"

#include <stdio.h>
#include <stdlib.h>

#include "stm32f7xx_hal_rcc.h"


//����ģ�鲿��
#include "ads1299.h"
#include "EMGProcessing.h"

//IMUģ�鲿��
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

// ȫ�ֱ���
PLN_FILER plnf[4]; 
IIR_FILER iir_filter_array[4];
COMB_FILER comb_filter_array[4];
int index_plnf;
int initial_PLF(PLN_FILER *x); //�����˲�����
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
// IMUģ��
short temp = 0;

// EMG-IMU����
void doInitEMGIMU();
uint8_t unionSerialPackage[26];
//u8, u16, u32
//(unsigned): char 8, short 16, int 32, long 32, long long 64, float 32, double 64

/******************************************* FreeRTOS�������ã�start�� **************************************************/

// �����ź������
SemaphoreHandle_t xSemaphore = NULL;
SemaphoreHandle_t xSemaphore_IMU = NULL;
/******* ����ʼ������ ***********/
//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		1024
//������
TaskHandle_t Start_Task_Handler;
//������
void start_task(void *pvParameters);

/******* ��A���ɼ������� ***********/
//�������ȼ�
#define US_TASK_PRIO		3
//�����ջ��С	
#define US_STK_SIZE 		512
//������
TaskHandle_t US_Task_Handler;
//������
void US_task(void *pvParameters);

/******* ��EMG�ɼ������� ***********/

//�������ȼ�
#define EMG_TASK_PRIO		3
//�����ջ��С	
#define EMG_STK_SIZE 		1024
//������
TaskHandle_t EMG_Task_Handler;
//������
void EMG_task(void *pvParameters);

/******* ��IMU�ɼ������� ***********/

//�������ȼ�
#define IMU_TASK_PRIO		3
//�����ջ��С	
#define IMU_STK_SIZE 		512
//������
TaskHandle_t IMU_Task_Handler;
//������
void IMU_task(void *pvParameters);

/******* �����ڷ��͡����� ***********/

//�������ȼ�
#define Serial_TASK_PRIO		3
//�����ջ��С	
#define Serial_STK_SIZE 		1024 * 4
//������
TaskHandle_t Serial_Task_Handler;
//������
void Serial_task(void *pvParameters);
void test_Serial_task(void *pvParameters);

/******* ��ʱ������ ***********/
#define TIMERCONTROL_TASK_PRIO  3
#define TIMERCONTROL_STK_SIZE   1024
TaskHandle_t TimerControlTask_Handler;
void timerControl_task(void* pvParameters);

TimerHandle_t EMG_Timer_Handle;
void EMG_Timer_Callback(TimerHandle_t xTimer);


/******* ���������Ϸ��͡����� ***********/


/******* ��IMU-EMG���ϲɼ������� ***********/
// //�������ȼ�
// #define EMG_IMU_TASK_PRIO		3
// //�����ջ��С	
// #define EMG_IMU_STK_SIZE 		1024 * 4
// //������
// TaskHandle_t EMG_IMU_Task_Handler;
// //������
// void EMG_IMU_task(void *pvParameters);

/******************************************* FreeRTOS�������ã�end�� **************************************************/

/******************************************* ��������start�� **************************************************/

int main(void)
{
	xSemaphore = xSemaphoreCreateMutex();
	xSemaphore_IMU = xSemaphoreCreateMutex();
	
//	Cache_Enable();                 //��L1-Cache��ʹ��STM32�Դ���ADC����ʱ����Ҫ��cache��������������
	HAL_Init();				        //��ʼ��HAL��
	Stm32_Clock_Init(432,25,2,9);   //����ʱ��,216Mhz 
	delay_init(216);				//��ʱ������ʼ������Ƶ216MHz

	//����Ĭ��200M��Ƶ��SLCK=25MHz��ѹ������M8266WIFI_SPI_Interface_Communication_Stress_Test()����֡
	//��216M��Ƶʱ�������̵�Ĭ������ʱSCLK=27MHz��������ѹ���������Գ�ʼ��ʧ�ܡ�˵���Բ�����ķ�ʽ����ʱ���ֻ��SCLK=25MHz
	
	
	/**************** A�����ֳ�ʼ�� *************************/
	//Initialize Beamformer			//�������ƶ�
	BF_IO_Init();
	BF_STOP;						//Immediately stop Beamformer
	BF_Init();						//beamformer�ĳ�ʼ�����������ò���Ƶ�ʺͷ����ĸ���
	delay_ms(200);

	//����
	uart_init(921600);				//���ڳ�ʼ��	921600�Ǽ���ģ��Ҫ��Ĵ��ڷ������ʣ�ע��һ��
	LED_Init();		

	EXTI_Init();					//Initialize external interrupt	

	//Initialize Wifi chip
	Wifi_Init();					//Initialize M8266WIFI
	Set_Socket();					//Setup socket connection with PC
	
	//ָʾ��
	LED_W(On);
	LED_R(On);
	LED_G(On);
	delay_ms(5);

	//Initialize DAC ���ܻ������ѹ��������⣬����Ŵ��ѹ��Ҫ��ʵ��Ϊ׼���ܵķŴ�������㹫ʽ��AD604��datasheet��P12/20
	DAC_Init();
	DAC_Set_Vol(1, 1.75);			//PA4	VGN_A	1.67V��4MHz̽ͷ�� 2V��1MHz̽ͷ��
	delay_ms(100);	////////////
	DAC_Set_Vol(2, 2.5);			//PA5	VREF_A	2.5V��4MHz̽ͷ�� 2.5��1MHz̽ͷ��
	delay_ms(100);
	
	 
	//Initialize Pulser
	Pulser_IO_Init();
	ALL_CH_HZ();
	delay_ms(100);

	//Initialize Timer
	TIM3_Init(125-1, 10800-1);		//Initialize Timer 3. Timer Clock(Ft) = 108MHz. Prescaler value = 10800-1
									//Auto reload value = 125-1. Tout = ((arr+1)*(psc+1))/Ft = 125*100us = 12.5ms
									//Auto reload value = 5000-1. Tout = 5000*100us = 500ms
			


	/**************** ����EMG���ֳ�ʼ�� *************************/
	// ��ʼ���ṹ�����飬���ڴ洢EMG����
	
	for(index_plnf=0;index_plnf<4;index_plnf++)
	{
	  initial_PLF(&plnf[index_plnf]);
	}
	for(int i = 0; i < 4; ++i)
	{
		initial_IIR_Filter(&iir_filter_array[i]);
		initial_COMB_Filter(&comb_filter_array[i]);
	}
	ADS1299Init();// ��ʼ������ADS1299оƬ����оƬIO���Ź���
	setDev(0x04,0,2);// ����EMG�ɼ���0x04����1kHz������
	// setDev(0x05,0,2);//0x05����500Hz������
	startDev(2);
	initErrorMessage();							
									
	/**************** IMU ���ֳ�ʼ�� *************************/
	// uart3_init(115200);				// ��ʼ������115200
	uart3_init(921600);
//	uart3_init(500000);
	// usmart_init(72);                // USMART��ʼ��
	printf("MPU6050 TEST\r\n");
	// MPU_IIC_Init();					// ��ʼ��IIC����
	MPU_Init();
	// printf("after init");
	// MPU DMP��ʼ��
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

	/**************** ������ʼ����(FreeRTOS) *********************/
	xTaskCreate((TaskFunction_t )start_task,            //������
							(const char*    )"start_task",          //��������
							(uint16_t       )START_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                  //���ݸ��������Ĳ���
							(UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&Start_Task_Handler);   //������              
	
	vTaskStartScheduler();          //�����������
	

	/**************** ������ʱ��(FreeRTOS) *********************/
	
}
/******************************************* ��������end�� **************************************************/

/******************************************* FreeRTOS������ʵ�֣�start�� **************************************************/

//����ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
	// ��ʱ������
	EMG_Timer_Handle = xTimerCreate(
					(const char*) 			  	"EMG_timer_task",
					(TickType_t) 			  	5,
					(UBaseType_t) 			  	pdTRUE,			// ���ڶ�ʱ��
					(void*)					  	1,
					(TimerCallbackFunction_t) 	EMG_Timer_Callback);


//	// ������ʱ������
	xTaskCreate((TaskFunction_t )timerControl_task,     	
                (const char*    )"timerControl_task",   	
                (uint16_t       )TIMERCONTROL_STK_SIZE,
                (void*          )NULL,				
                (UBaseType_t    )TIMERCONTROL_TASK_PRIO,	
                (TaskHandle_t*  )&TimerControlTask_Handler);


    // ����US����
    xTaskCreate((TaskFunction_t )US_task,     	
                (const char*    )"US_task",   	
                (uint16_t       )US_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )US_TASK_PRIO,	
                (TaskHandle_t*  )&US_Task_Handler);
					
	// ����EMG����	
	// xTaskCreate((TaskFunction_t )EMG_task,     	
	// 			(const char*    )"EMG_task",   	
	// 			(uint16_t       )EMG_STK_SIZE, 
	// 			(void*          )NULL,				
	// 			(UBaseType_t    )EMG_TASK_PRIO,	
	// 			(TaskHandle_t*  )&EMG_Task_Handler);  
								
	// ����IMU����
	xTaskCreate((TaskFunction_t )IMU_task,     	
				(const char*    )"IMU_task",   	
				(uint16_t       )IMU_STK_SIZE, 
				(void*          )NULL,				
				(UBaseType_t    )IMU_TASK_PRIO,	
				(TaskHandle_t*  )&IMU_Task_Handler); 	

//	// �������ڷ�����������
	xTaskCreate((TaskFunction_t )test_Serial_task,
				(const char*    )"test_Serial_task",
				(uint16_t       )Serial_STK_SIZE,
				(void*          )NULL,
				(UBaseType_t    )Serial_TASK_PRIO,	
				(TaskHandle_t*  )&Serial_Task_Handler);
	
	// // EMG IMU���ϲɼ�
	// xTaskCreate((TaskFunction_t )EMG_IMU_task,
	// 			(const char*    )"EMG_IMU_task",
	// 			(uint16_t       )Serial_STK_SIZE,
	// 			(void*          )NULL,
	// 			(UBaseType_t    )EMG_IMU_TASK_PRIO,	
	// 			(TaskHandle_t*  )&EMG_IMU_Task_Handler);
							
		
    vTaskDelete(Start_Task_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
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


//��A���ɼ�����������
void US_task(void *pvParameters)
{
	while(1)
	{
		u8 Ch_Num;
		LED_W_Toggle;				//LED����˸����˸ʱ����Ϊ12.5ms*4=50ms����ָʾ��ǰwhileѭ����������
		
		for (Ch_Num = 1; Ch_Num < 5; ++Ch_Num) //�����ͨ��Ch_Num < 5�����8ͨ����Ch_Num < 9�����ʹ����̫�����䣬��Ҫʹ��Ch_Num < 9��Ӧ������λ�������⣩
		{	
			//��ʼ��ADC1ͨ��1,�Լ���ʼ��DMA
			Rheostat_Init();
			
			Start_TIM3();			//Start Timer 3
			 
			
			//Generate excitation pulses
			CH_TX(Ch_Num);			//Change to transmit mode ���Ƚ�pulser�л�������״̬��������50V�ķ�������
			BF_FIRE;				//beamformer���������źţ�����pulser�����ڼ�������
//			delay_us(1);			//Fire for 1us at 5MHz, totally 5 pulses(������5Mhz�ĳ���̽ͷ actually 4 pulses because of the excitation waveform)
			delay_us(3.2);			//Fire for 3.2us at 1.25MHz, totally 4 pulses ������1MHz�ĳ���̽ͷ
			BF_STOP;				//beamformerֹͣ���������ź�
//			delay_us(1);			//������ ��֤���Խ��յ��ز��źţ������ʱ�ϳ������޷����յ������Ļز��źţ����ע�͵�����ûɶӰ��

      
			//����pulser�Ľ���ģʽ���ʹ�DMA��ADC�ɼ�����
			FIFO_read(1000,Ch_Num);

			ALL_CH_HZ();			//Close Pulser to HZ state �ر�pulser��HZ״̬����ǯλ״̬
			
			
			//��׼�����ݴ����ʽ�����Ӱ�ͷ��β��ͨ����
			FIFO_buffer[0] = 0x00;		//Packet header �˴��Ĳ�������Ϊ8λ
			FIFO_buffer[1] = Ch_Num;
			FIFO_buffer[999] = 0xFF;	//Packet tail	16���Ʊ�ʾ�ĵ�8λ����1111 1111
			
			
			//Transmit echo data by Wifi
//			taskENTER_CRITICAL();           //�����ٽ���
			Wifi_Send_Packet();	
//			taskEXIT_CRITICAL();            //�˳��ٽ���
		
			while(!Ch_Flag)
			{
				// vTaskDelay(1);// ��֤freeRTOS�ܹ�����ϵͳ���������л�
				delay_ms(1);
			}			//�ȴ���־λ����֤ÿ��ͨ�����ĵ�ʱ�䶼��12.5ms
			Ch_Flag = 0;				//Reset the flag of time control for each channel 
			Stop_TIM3();
		}	//����һ֡����4��ͨ��	
	}
}

//��EMG�ɼ�����������
void EMG_task(void *pvParameters)
{
	u8 led_blink_num;
	struct mpuData data0, data1, data2, data3;
	int count = 0;
	while(1)
	{
		// taskENTER_CRITICAL();           //�����ٽ���
		// xSemaphoreTake(xSemaphore, portMAX_DELAY);		// �ȴ�������
		dataAcq();
		// printf("in");
		// if(count < 10)
			// printf("in %d", dataAcq());
		// taskEXIT_CRITICAL();            //�˳��ٽ���
		// delay_ms(2);
		// GetMPUData(1, &data1);
		// GetMPUData(2, &data2);
		// GetMPUData(3, &data3);
		// xSemaphoreGive(xSemaphore);		// �����ź����ͷ�
		led_blink_num++;
		// delay_ms(1);// ��֤freeRTOS�ܹ���������ϵͳ����������л�����С��1ms�ļ��
		// delay_ms(1);
		// delay_us(100);
		if(led_blink_num > 200)// 200ms������ģ���ϵ�ָʾ�ƻ���˸��˵����������
		{
			led_blink_num=0;
			LED_G_Toggle;
		}
		count++;
	}
}
// ����һ��delay_ms����
// void IMU_task(void *pvParameters)
// {
// 	u8 led_blink_num;
// 	while(1)
// 	{
// 		led_blink_num++;
// 		delay_ms(1);
// 		if(led_blink_num > 200)// 200ms������ģ���ϵ�ָʾ�ƻ���˸��˵����������
// 		{
// 			led_blink_num=0;
// 			LED_G_Toggle;
// 		}
// 	}
// }
//��IMU �ɼ�����������
void IMU_task(void *pvParameters)
{
	u8 t = 0;
	volatile struct mpuData data0, data1, data2, data3;
	while(1)
	{
		// �ɼ�����
		// taskENTER_CRITICAL();           //�����ٽ���
		// delay_ms(100);
		// // GetMPUData(0, &data0);
		GetMPUData(1, &data1);
		// delay_ms(1);
		GetMPUData(2, &data2);
		GetMPUData(3, &data3);
		// taskEXIT_CRITICAL();            //�˳��ٽ���
		// printf("imu\r\n");
//		// ��ӡ����
		// if((t % 2) == 0)
		// {
		// 	PrintData(0, &data0);
		// 	PrintData(1, &data1);
		// 	PrintData(2, &data2);
		// 	PrintData(3, &data3);
		// }
		// �����ݷ��͸���λ��
		// IMUSendData(1, &data1);
		// IMUSendData(2, &data2);
		// IMUSendData(3, &data3);
		// xSemaphoreTake(xSemaphore_IMU, portMAX_DELAY);		// �ȴ�������
		IMUSaveData(0, &data1);
		IMUSaveData(1, &data2);
		IMUSaveData(2, &data3);
		// xSemaphoreGive(xSemaphore_IMU);		// �����ź����ͷ�
		// delay_ms(1);// ��֤freeRTOS�ܹ���������ϵͳ����������л�����С��1ms�ļ��
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
// 		// taskENTER_CRITICAL();           //�����ٽ���
// 		xSemaphoreTake(xSemaphore, portMAX_DELAY);		// �ȴ������ź���
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
// 		// taskEXIT_CRITICAL();            //�˳��ٽ���
// 		xSemaphoreGive(xSemaphore);
// 		delay_ms(1);
		
// 		serialPackage[6] = sumEMGData;
// 		serialPackage[7] = packageNum;
// 		// xSemaphoreTake(xSemaphore_IMU, portMAX_DELAY);		// �ȴ�������
// 		// for(int i = 1; i < 4; ++i)	// ����ֻ����1-3��IMU��û����0
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
// 		// xSemaphoreGive(xSemaphore_IMU);		// �����ź����ͷ�
		
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
	testSendPackage[2] = 0x32;		// ���ݳ���Ϊ50
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
	imuTotalSendPackage[2] = 0x3C;		// ���ݳ���Ϊ60
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

		// ����16bit��EMG����
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

// ȫ�ֱ�����ʼ��
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
/******************************************* FreeRTOS������ʵ�֣�end�� **************************************************/

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
ע�����
1������ɼ������л����ż��������ʽ�����������Ҫ������freeRTOS���������л����µģ����������Ƶ�ʸ���1kHzʱ����Ƚ����ԣ�������Ϊ500Hzʱ��������û���ˡ�
2��Ϊ�˱�֤freeRTOS�ܹ���������ϵͳ����������л����ñ�֤ÿ�������ж���������1��1ms���ϵ���ʱ�������delay_ms��ʱ������װ���˵���vTaskDelay������delay_ms(1) = vTaskDelay(1)����Ҫ���ֿյ���ѭ����䣬��while(�ж�)��
3���������ȼ�������Խ�����ȼ���Խ�ߣ�0Ϊ�������ȼ���Ҳ�����������񶼲�ִ�е�ʱ��Ż�ִ�����ȼ���Ϊ0���������Ϊ32�����û���������󣬾����Ѳɼ��������ó���ͬ���ȼ�����������ȼ�����ֱȽ����صĶ�������

*/
