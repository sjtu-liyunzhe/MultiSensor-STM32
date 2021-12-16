//FreeRTOS���
#include "FreeRTOS.h"
#include "task.h"

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

PLN_FILER plnf[4]; 
int index_plnf;
int initial_PLF(PLN_FILER *x); //�����˲�����

// IMUģ��
short temp = 0;

//u8, u16, u32
//(unsigned): char 8, short 16, int 32, long 32, long long 64, float 32, double 64

/******************************************* FreeRTOS�������ã�start�� **************************************************/

/******* ����ʼ������ ***********/
//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
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
#define EMG_STK_SIZE 		50
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

/******* ���������Ϸ��͡����� ***********/


/******************************************* FreeRTOS�������ã�end�� **************************************************/

/******************************************* ��������start�� **************************************************/

int main(void)
{
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
	ADS1299Init();// ��ʼ������ADS1299оƬ����оƬIO���Ź���
//	setDev(0x04,0,2);// ����EMG�ɼ���0x04����1kHz������
	setDev(0x05,0,2);//0x05����500Hz������
	startDev(2);								
									
	/**************** IMU ���ֳ�ʼ�� *************************/
	// uart3_init(115200);				// ��ʼ������115200
	uart3_init(500000);
	// usmart_init(72);                // USMART��ʼ��
	printf("MPU6050 TEST\r\n");
	MPU_IIC_Init();					// ��ʼ��IIC����
	printf("after init");
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
	

	/**************** ������ʼ����(FreeRTOS) *********************/
	xTaskCreate((TaskFunction_t )start_task,            //������
							(const char*    )"start_task",          //��������
							(uint16_t       )START_STK_SIZE,        //�����ջ��С
							(void*          )NULL,                  //���ݸ��������Ĳ���
							(UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
							(TaskHandle_t*  )&Start_Task_Handler);   //������              
	vTaskStartScheduler();          //�����������
}
/******************************************* ��������end�� **************************************************/

/******************************************* FreeRTOS������ʵ�֣�start�� **************************************************/

//����ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���

    // ����US����
    xTaskCreate((TaskFunction_t )US_task,     	
                (const char*    )"US_task",   	
                (uint16_t       )US_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )US_TASK_PRIO,	
                (TaskHandle_t*  )&US_Task_Handler);
					
		// ����EMG����	
 		xTaskCreate((TaskFunction_t )EMG_task,     	
                 (const char*    )"EMG_task",   	
                 (uint16_t       )EMG_STK_SIZE, 
                 (void*          )NULL,				
                 (UBaseType_t    )EMG_TASK_PRIO,	
                 (TaskHandle_t*  )&EMG_Task_Handler);  
								
		// ����IMU����
		xTaskCreate((TaskFunction_t )IMU_task,     	
                (const char*    )"IMU_task",   	
                (uint16_t       )IMU_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )IMU_TASK_PRIO,	
                (TaskHandle_t*  )&IMU_Task_Handler);  						
								
		
    vTaskDelete(Start_Task_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


//��A���ɼ�����������
void US_task(void *pvParameters)
{
	while(1)
	{
		u8 Ch_Num;
		LED_W_Toggle;				//LED����˸����˸ʱ����Ϊ12.5ms*4=50ms����ָʾ��ǰwhileѭ����������
		
		for (Ch_Num = 1; Ch_Num < 5; Ch_Num++) //�����ͨ��Ch_Num < 5�����8ͨ����Ch_Num < 9�����ʹ����̫�����䣬��Ҫʹ��Ch_Num < 9��Ӧ������λ�������⣩
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
				vTaskDelay(1);// ��֤freeRTOS�ܹ�����ϵͳ���������л�
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
	while(1)
	{
		taskENTER_CRITICAL();           //�����ٽ���
		dataAcq();
		taskEXIT_CRITICAL();            //�˳��ٽ���
		led_blink_num++;
		delay_ms(1);// ��֤freeRTOS�ܹ���������ϵͳ����������л�����С��1ms�ļ��
		if(led_blink_num > 200)// 200ms������ģ���ϵ�ָʾ�ƻ���˸��˵����������
		{
			led_blink_num=0;
			LED_G_Toggle;
		}
	}
}

//��IMU �ɼ�����������
void IMU_task(void *pvParameters)
{
	u8 t = 0;
	struct mpuData data0, data1, data2, data3;
	while(1)
	{
		// �ɼ�����
		// taskENTER_CRITICAL();           //�����ٽ���
		// // GetMPUData(0, &data0);
		GetMPUData(1, &data1);
		GetMPUData(2, &data2);
		GetMPUData(3, &data3);
		// taskEXIT_CRITICAL();            //�˳��ٽ���
		delay_ms(1);// ��֤freeRTOS�ܹ���������ϵͳ����������л�����С��1ms�ļ��
//		// ��ӡ����
		// if((t % 2) == 0)
		// {
		// 	// PrintData(0, &data0);
		// 	PrintData(1, &data1);
		// 	PrintData(2, &data2);
		// 	PrintData(3, &data3);
		// }
		// �����ݷ��͸���λ��
		IMUSendData(1, &data1);
		IMUSendData(2, &data2);
		IMUSendData(3, &data3);
		t++;
	}
}

/******************************************* FreeRTOS������ʵ�֣�end�� **************************************************/


/*
ע�����
1������ɼ������л����ż��������ʽ�����������Ҫ������freeRTOS���������л����µģ����������Ƶ�ʸ���1kHzʱ����Ƚ����ԣ�������Ϊ500Hzʱ��������û���ˡ�
2��Ϊ�˱�֤freeRTOS�ܹ���������ϵͳ����������л����ñ�֤ÿ�������ж���������1��1ms���ϵ���ʱ�������delay_ms��ʱ������װ���˵���vTaskDelay������delay_ms(1) = vTaskDelay(1)����Ҫ���ֿյ���ѭ����䣬��while(�ж�)��
3���������ȼ�������Խ�����ȼ���Խ�ߣ�0Ϊ�������ȼ���Ҳ�����������񶼲�ִ�е�ʱ��Ż�ִ�����ȼ���Ϊ0���������Ϊ32�����û���������󣬾����Ѳɼ��������ó���ͬ���ȼ�����������ȼ�����ֱȽ����صĶ�������

*/
