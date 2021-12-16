#include "adc.h"
#include "delay.h"
#include "pulser.h"

u8 FIFO_buffer[1000];	//�洢STM32 ��ADC�������ݣ�1000�������㣻����ʹ��volatile���δ��Ա�֤ÿ�εĶ�ȡ���ǴӾ��Ե�ַ��������ֵ��������Ϊ��������������Ż����¶�ȡ����ֵ����ʵʱ��ADֵ
DMA_HandleTypeDef DMA_Init_Handle;
ADC_HandleTypeDef ADC_Handle;
ADC_ChannelConfTypeDef ADC_Config;


static void Rheostat_ADC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // ʹ�� GPIO ʱ��
    RHEOSTAT_ADC_GPIO_CLK_ENABLE();

    // ���� IO
    GPIO_InitStructure.Pin = RHEOSTAT_ADC_GPIO_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL ; //������������
    HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);
}




static void Rheostat_ADC_Mode_Config(void)
{
    //~�ȿ�DMA���ٿ�ADC����ADC����һ�����֮�󣬻����һ��DMA�������������ǱȽϺ�����

    // ------------------DMA Init �ṹ����� ��ʼ��--------------------------
    // ADC1ʹ��DMA2��������0��ͨ��0��������ֲ�̶�����
    // ����DMAʱ��
    RHEOSTAT_ADC_DMA_CLK_ENABLE();
    // ���ݴ���ͨ��
    DMA_Init_Handle.Instance = RHEOSTAT_ADC_DMA_STREAM;
    // ���ݴ��䷽��Ϊ���赽�洢��
    DMA_Init_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    // ����Ĵ���ֻ��һ������ַ���õ���
    DMA_Init_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    // �洢����ַ�̶�
    DMA_Init_Handle.Init.MemInc = DMA_MINC_ENABLE;
    // // �������ݴ�СΪһ���ֽ�
    DMA_Init_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    //	�洢�����ݴ�СΪһ���ֽڣ����������ݴ�С��ͬ
    DMA_Init_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    // ~ѭ������ģʽ,���δ��䣬һ�δ���ָ�����ֽ�������1000�������ڴ�����1000���ֽ�֮��ֹͣ���ȴ���һ�ο���ָ��
    DMA_Init_Handle.Init.Mode = DMA_NORMAL;
    // DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
    DMA_Init_Handle.Init.Priority = DMA_PRIORITY_HIGH;
    // ��ֹDMA FIFO	��ʹ��ֱ��ģʽ
    DMA_Init_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    // FIFO ��С��FIFOģʽ��ֹʱ�������������
//    DMA_Init_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
//    DMA_Init_Handle.Init.MemBurst = DMA_MBURST_SINGLE;
//    DMA_Init_Handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
    // ѡ�� DMA ͨ����ͨ������������
    DMA_Init_Handle.Init.Channel = RHEOSTAT_ADC_DMA_CHANNEL;
    //��ʼ��DMA�������൱��һ����Ĺܵ����ܵ������кܶ�ͨ��

    HAL_DMA_Init(&DMA_Init_Handle);

    HAL_DMA_Start (&DMA_Init_Handle,RHEOSTAT_ADC_DR_ADDR,(uint32_t)&FIFO_buffer,1000);

    // ����ADCʱ��
    RHEOSTAT_ADC_CLK_ENABLE();
    // -------------------ADC Init �ṹ�� ���� ��ʼ��------------------------
    // ADC1
    ADC_Handle.Instance = RHEOSTAT_ADC;
    // ʱ��Ϊfpclk 4��Ƶ��27MHz
		//~���ģ������� ʱ��Ϊfpclk 2��Ƶ������54MHz����������ԭ�ӵĽ̳̣�M7�ں˵�ADC���ֻ����36MHz�����Ǿ���AD2�豸���ԣ�����Ƶ216+fpclk 2��Ƶ����Ч���ȡ���Ƶ144MHz��fpclk 2��Ƶ����Ч�����á����ԣ��˴����ɣ�������
    ADC_Handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
		
    // ADC �ֱ���
    ADC_Handle.Init.Resolution = ADC_RESOLUTION_8B;
    // ��ֹɨ��ģʽ����ͨ���ɼ�����Ҫ
    ADC_Handle.Init.ScanConvMode = DISABLE;
    // ����ת��
    ADC_Handle.Init.ContinuousConvMode = ENABLE;
    // ������ת��
    ADC_Handle.Init.DiscontinuousConvMode = DISABLE;
    // ������ת������
    ADC_Handle.Init.NbrOfDiscConversion   = 0;
    //��ֹ�ⲿ���ش���
    ADC_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //ʹ�������������ⲿ�����������ã�ע�͵�����
    //ADC_Handle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
    //�����Ҷ���
    ADC_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //ת��ͨ�� 1��
    ADC_Handle.Init.NbrOfConversion = 1;
    //ʹ������ת������
    ADC_Handle.Init.DMAContinuousRequests = ENABLE;
    //ת����ɱ�־
    ADC_Handle.Init.EOCSelection          = DISABLE;
    // ��ʼ��ADC
    HAL_ADC_Init(&ADC_Handle);
    //---------------------------------------------------------------------------
    ADC_Config.Channel      = RHEOSTAT_ADC_CHANNEL;
    ADC_Config.Rank         = 1;
    // ����ʱ����
    ADC_Config.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    ADC_Config.Offset       = 0;
    // ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ3��ʱ������
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);
}


void Rheostat_Init(void)
{
    Rheostat_ADC_GPIO_Config();
    Rheostat_ADC_Mode_Config();

}
void FIFO_read(u16 times,u8 Ch_Num)
{
    HAL_ADC_Start_DMA(&ADC_Handle, (uint32_t*)&FIFO_buffer, times);
    CH_RX(Ch_Num);			//Change to receive mode //~��pulser�л�������״̬�����ջز��ź�
    delay_us(500);			//~HAL_ADC_Start_DMA()����ֻ�ǿ�����DMA��ADC����û�п�ʼ�ɼ��������ڹر�pulser�Ľ���״̬֮ǰ��Ҫ��һ����ʱ��������ʱʱ���ǡ���������/����Ƶ�ʡ���
}
