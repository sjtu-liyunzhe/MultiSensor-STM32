#include "ethernet.h"
#include "delay.h"
//#include "fifo.h"
#include "led.h"
#include "adc.h"

//For CP2200
//Control Port:
//INT_E	    PD8		Input Floating Mode
//CS_E		PD9		Output Push Pull Mode
//WR_E		PD10	Output Push Pull Mode
//RD_E		PD11	Output Push Pull Mode
//RST_E 	PA15	Output Push Pull Mode or Input Floating Mode

//Address Port:
//A0_E		PC9		Output Push Pull Mode
//A1_E		PC8		Output Push Pull Mode
//A2_E		PC7		Output Push Pull Mode
//A3_E		PC6		Output Push Pull Mode
//A4_E		PD15	Output Push Pull Mode
//A5_E		PD14	Output Push Pull Mode
//A6_E		PD13	Output Push Pull Mode
//A7_E		PD12	Output Push Pull Mode

//Data Port:
//D0_E		PD1		Output Push Pull Mode or Input Floating Mode
//D1_E		PD0		Output Push Pull Mode or Input Floating Mode
//D2_E		PC12	Output Push Pull Mode or Input Floating Mode
//D3_E		PC11	Output Push Pull Mode or Input Floating Mode
//D4_E		PC10	Output Push Pull Mode or Input Floating Mode
//D5_E		PA12	Output Push Pull Mode or Input Floating Mode
//D6_E		PA11	Output Push Pull Mode or Input Floating Mode
//D7_E		PA8 	Output Push Pull Mode or Input Floating Mode
extern u8 FIFO_buffer[1000];
u8 MAC_Address[6];
/******************************************************************
 * @Function	Ethernet_IO_Init
 * @Brief		Initialize INT, CS, WR, RD, A0--A7
 * @Parameter	None
 ******************************************************************/
void Ethernet_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOD_CLK_ENABLE();     	//GPIOD clock 
	__HAL_RCC_GPIOC_CLK_ENABLE();     	//GPIOC clock   	
//	__HAL_RCC_GPIOG_CLK_ENABLE();  		//GPIOG clock

	//INT_E_	PD8
	GPIO_InitStructure.Pin = GPIO_PIN_8;				//PD8
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;  		//Input floating mode
	GPIO_InitStructure.Pull = GPIO_NOPULL;          	//No pull
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;     	//High speed
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);			//PD8	
	//CS, WR, RD, A4--A7
	GPIO_InitStructure.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;	//PD9,10,11,12,13,14,15
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  	//Output push pull mode
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);			//PD9,10,11,12,13,14,15
	//A0--A3
	GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;	//PC6,7,8,9
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);			
	
	/////////////////////////////////
//	GPIO_InitStructure.Pin = GPIO_PIN_3;				//PI3
//	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  //Output push pull mode
//	GPIO_InitStructure.Pull = GPIO_NOPULL;         		//No pull
//	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;    	 	//High speed
//	HAL_GPIO_Init(GPIOI, &GPIO_InitStructure);			//PI3
//	RST_E_WRITE(1);
}	

/******************************************************************
 * @Function	Ethernet_RST_Mode
 * @Brief		Initialize RST_E_ to input or output mode
 * @Parameter	mode: 0 for Output, 1 for Input
 ******************************************************************/
void Ethernet_RST_Mode(u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	__HAL_RCC_GPIOA_CLK_ENABLE();     					//GPIOA clock 
	
	if(mode == 1)
	{
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;  	//Input floating mode
	}
	else if(mode == 0)
	{
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  //Output push pull mode
	}
	
	GPIO_InitStructure.Pin = GPIO_PIN_15;				//PA15
	GPIO_InitStructure.Pull = GPIO_NOPULL;         		//No pull
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;    	 	//High speed
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);			//PA15	
}	

/******************************************************************
 * @Function	Ethernet_DataPort_Mode
 * @Brief		Initialize D0--D7 to read or write mode
 * @Parameter	mode: 0 for Output, 1 for Input
 ******************************************************************/
void Ethernet_DataPort_Mode(u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	__HAL_RCC_GPIOD_CLK_ENABLE();     					//GPIOD clock 
	__HAL_RCC_GPIOC_CLK_ENABLE();     					//GPIOC clock 
	__HAL_RCC_GPIOA_CLK_ENABLE();     					//GPIOA clock 
	
	if(mode == 1)
	{
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;  	//Input floating mode
	}
	else if(mode == 0)
	{
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  //Output push pull mode
	}
	//D0--D1
	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1;	//PD0,1
	GPIO_InitStructure.Pull = GPIO_NOPULL;         		//No pull
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;    	 	//High speed
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);			//PD0,1
	//D2--D4
	GPIO_InitStructure.Pin = GPIO_PIN_12 | GPIO_PIN_11 | GPIO_PIN_10;	//PC12,11,10
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);			
	//D5--D7
	GPIO_InitStructure.Pin = GPIO_PIN_12 | GPIO_PIN_11 | GPIO_PIN_8;	//PA12,11,8
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);			
}

/******************************************************************
 * @Function	Ethernet_Arrange_Address
 * @Brief		Set A0--A7 to write address to CP2200
 * @Parameter	raw_address: register address
 ******************************************************************/
void Ethernet_Arrange_Address(u8 raw_address)
{
	u8 Add_Temp = 0;
	u8 Add_Point = 0x01;	//Pointer for judging the value of each bit in raw_address
	//A0
	Add_Temp = raw_address;
	if(Add_Temp &= Add_Point)
		A0_E(1);
	else
		A0_E(0);
	Add_Point <<= 1;
	//A1
	Add_Temp = raw_address;
	if(Add_Temp &= Add_Point)
		A1_E(1);
	else
		A1_E(0);
	Add_Point <<= 1;
	//A2
	Add_Temp = raw_address;
	if(Add_Temp &= Add_Point)
		A2_E(1);
	else
		A2_E(0);
	Add_Point <<= 1;
	//A3
	Add_Temp = raw_address;
	if(Add_Temp &= Add_Point)
		A3_E(1);
	else
		A3_E(0);
	Add_Point <<= 1;
	//A4
	Add_Temp = raw_address;
	if(Add_Temp &= Add_Point)
		A4_E(1);
	else
		A4_E(0);
	Add_Point <<= 1;
	//A5
	Add_Temp = raw_address;
	if(Add_Temp &= Add_Point)
		A5_E(1);
	else
		A5_E(0);
	Add_Point <<= 1;
	//A6
	Add_Temp = raw_address;
	if(Add_Temp &= Add_Point)
		A6_E(1);
	else
		A6_E(0);
	Add_Point <<= 1;
	//A7
	Add_Temp = raw_address;
	if(Add_Temp &= Add_Point)
		A7_E(1);
	else
		A7_E(0);	
}

/******************************************************************
 * @Function	Ethernet_Arrange_Data
 * @Brief		Set D0--D7 to write data to CP2200
 * @Parameter	raw_data: data to be written
 ******************************************************************/
void Ethernet_Arrange_Data(u8 raw_data)
{
	u8 Data_Temp = 0;
	u8 Data_Point = 0x01;	//Pointer for judging the value of each bit in raw_data
	//D0
	Data_Temp = raw_data;
	if(Data_Temp &= Data_Point)
		D0_E_WRITE(1);
	else
		D0_E_WRITE(0);
	Data_Point <<= 1;
	//D1
	Data_Temp = raw_data;
	if(Data_Temp &= Data_Point)
		D1_E_WRITE(1);
	else
		D1_E_WRITE(0);
	Data_Point <<= 1;
	//D2
	Data_Temp = raw_data;
	if(Data_Temp &= Data_Point)
		D2_E_WRITE(1);
	else
		D2_E_WRITE(0);
	Data_Point <<= 1;
	//D3
	Data_Temp = raw_data;
	if(Data_Temp &= Data_Point)
		D3_E_WRITE(1);
	else
		D3_E_WRITE(0);
	Data_Point <<= 1;
	//D4
	Data_Temp = raw_data;
	if(Data_Temp &= Data_Point)
		D4_E_WRITE(1);
	else
		D4_E_WRITE(0);
	Data_Point <<= 1;
	//D5
	Data_Temp = raw_data;
	if(Data_Temp &= Data_Point)
		D5_E_WRITE(1);
	else
		D5_E_WRITE(0);
	Data_Point <<= 1;
	//D6
	Data_Temp = raw_data;
	if(Data_Temp &= Data_Point)
		D6_E_WRITE(1);
	else
		D6_E_WRITE(0);
	Data_Point <<= 1;
	//D7
	Data_Temp = raw_data;
	if(Data_Temp &= Data_Point)
		D7_E_WRITE(1);
	else
		D7_E_WRITE(0);
}

/******************************************************************
 * @Function	Ethernet_Read_Data
 * @Brief		Read D0--D7
 * @Parameter	Return the value of D0--D7
 ******************************************************************/
u8 Ethernet_Read_Data(void)
{
	u8 result = 0;
	result |= (D0_E_READ << 0);
	result |= (D1_E_READ << 1);
	result |= (D2_E_READ << 2);
	result |= (D3_E_READ << 3);
	result |= (D4_E_READ << 4);
	result |= (D5_E_READ << 5);
	result |= (D6_E_READ << 6);
	result |= (D7_E_READ << 7);
	return result;
}

/******************************************************************
 * @Function	Read_Ethernet
 * @Brief		Read the register value of CP2200
 * @Parameter	Reg_Address: register address, return the value of register
 ******************************************************************/
u8 Read_Ethernet(u8 Reg_Address)
{
	u8 Read_Data = 0;
	Ethernet_DataPort_Mode(1);	//Config data port to read
	CS_E_(1);	WR_E_(1);	RD_E_(1);	//Disable chip select, write and read operation	
	Ethernet_Arrange_Address(Reg_Address);	
	CS_E_(0);	RD_E_(0);		//Enable chip select and read operation	
	delay_us(1);				//Delay 0.25us	需要重新调整，不然影响传输速度，或者这里不需要延时？？？
	Read_Data = Ethernet_Read_Data();
	RD_E_(1);
	delay_us(1);				//Delay 0.25us	需要重新调整，不然影响传输速度，或者这里不需要延时？？？
	CS_E_(1);
	delay_us(1);				//Delay 0.25us	需要重新调整，不然影响传输速度，或者这里不需要延时？？？
	return Read_Data;
}

/******************************************************************
 * @Function	Write_Ethernet
 * @Brief		Write data to the registers of CP2200
* @Parameter	Reg_Address: register address, TX_Data: data to be written
 ******************************************************************/
void Write_Ethernet(u8 Reg_Address, u8 TX_Data)
{
	Ethernet_DataPort_Mode(0);	//Config data port to write
	CS_E_(1);	WR_E_(1);	RD_E_(1);	//Disable chip select, write and read operation
	Ethernet_Arrange_Address(Reg_Address);
	Ethernet_Arrange_Data(TX_Data);
	CS_E_(0);	WR_E_(0);		//Enable chip select and write operation
	delay_us(1);				//Delay 0.25us	需要重新调整，不然影响传输速度，或者这里不需要延时？？？
	WR_E_(1);
	delay_us(1);				//Delay 0.25us	需要重新调整，不然影响传输速度，或者这里不需要延时？？？
	CS_E_(1);
	delay_us(1);				//Delay 0.25us	需要重新调整，不然影响传输速度，或者这里不需要延时？？？
}

/******************************************************************
 * @Function	Ethernet_Transmit_Packet
 * @Brief		Transmit a packet using AutoWrite Interface
 * @Parameter	None
 ******************************************************************/
void Ethernet_Transmit_Packet(void)
{
	u16 Byte = 0;
	while (Read_Ethernet(0x54) == 1);	//Wait for the previous packet to complete(TXBUSY == 0)
	Write_Ethernet(0x5A, 0);			//Set TXSTARTL transmit buffer pointer to 0 (low byte)
	Write_Ethernet(0x59, 0);			//Set TXSTARTH transmit buffer pointer to 0 (high byte)
	for (Byte = 0; Byte < 1000; Byte++)
	{
		Write_Ethernet(0x03, FIFO_buffer[Byte]);	//Write all data bytes to TXAUTOWR register, one byte at a time
	}
	Write_Ethernet(0x5A, 0);			//Set TXSTARTL transmit buffer pointer to 0 (low byte)
	Write_Ethernet(0x59, 0);			//Set TXSTARTH transmit buffer pointer to 0 (high byte)
	Write_Ethernet(0x53, 1);			//Write 1 to TXGO bit (TXCN.0) to begin transmission, other bits are 0
	
}

/******************************************************************
 * @Function	Ethernet_Controller_Init
 * @Brief		Initialize and configure CP2200
 * @Parameter	None
 ******************************************************************/
void Ethernet_Controller_Init(void)
{
	u8 init_finish = 0;
//	u8 connect = 0; 
	CS_E_(1);	//Not select
	WR_E_(1);	//Disable write operation
	RD_E_(1);	//Disable read operation
	
	//1. Use RST pin to reset CP2200
	Ethernet_RST_Mode(0);	//Set RST pin to output mode
	RST_E_WRITE(0);			//Reset CP2200
	delay_ms(125);			//Pull down RST for at least 15us	需要这么125ms长的延时？？？
	RST_E_WRITE(1);
	Ethernet_RST_Mode(1);	//Set RST pin to input mode			
	while(!RST_E_READ)		//Wait for RST pin to rise
	{
//		LED_R(On);
	}
	LED_R(0);				//Blink LED_G after reset
//	delay_ms(500);
//	LED_G(1);
	
	//2. Check Initialization Completion
	/*Description of Interrupt Status Register 0(INT0) (0x63)*/ 
	//Reading this register will clear all INT0 interrupt flags.(Self-Clearing)
	//bit 4: Oscillator Initialization Complete Interrupt Flag
	//bit 5: Self Initialization Complete Interrupt Flag
lp:	
	delay_ms(375);
	init_finish = Read_Ethernet(0x63);	//Interrupt Status Register 0 (Self Clearing)
										//Bit 4==1 indicates Oscillator Initialization Complete
										//Bit 5==1 indicates Self Initialization Complete 
	if (init_finish &= 0x20)			//0x20 = 0b0010 0000    可以把bit4也判断了
	{
//		LED_R(On);			//Blink LED_R when finishing initialization
//		delay_ms(500);
//		LED_R(Off);
//		delay_ms(500);
	}
	else
	{
		goto lp;
	}
	
	//3. Disable interrupt (INT0EN, INT1EN)
	//All interrupt are enabled after every reset. Need to disable interrupt that will not be handled.
	/*Description of INT0EN register (0x64)*/
	//bit 0: 0 = Disable Packet Received Interrupt
	//bit 1: 0 = Disable Receive FIFO Full Interrupt
	//bit 2: 0 = Disable Packet Transmitted Interrupt
	//bit 3: 0 = Disable Flash Write/Erase Operation Complete Interrupt
	//bit 4: 0 = Disable Oscillator Initialization Complete Interrupt
	//bit 5: 0 = Disable Self Initialization Complete Interrupt
	//bit 6: 0 = Disable Receive FIFO Empty Interrupt
	//bit 7: 0 = Disable End of Packet Interrupt
	Write_Ethernet(0x64, 0x00);		//Interrupt Enable Register 0	不用清理INT0的Status？
	delay_ms(13);		
	
	/*Description of Interrupt Status Register 1(INT1) (0x7F)*/ 
	//This register contains some interrupt flags. 
	//Reading this register will clear all INT1 interrupt flags.(Self-Clearing)
	Write_Ethernet(0x7F, 0x00);		//Interrupt Status Register 1 (Self Clearing)	原本是清理这个
	delay_ms(13);
	
//	/*Description of INT1EN register (0x7D)*/
//	//bit 0: 0 = Disable Auto-Negotiation Complete Interrupt
//	//bit 2: 0 = Disable Auto-Negotiation Failed Interrupt
//	Write_Ethernet(0x7D, 0x00);		//Interrupt Enable Register 1	原本没有这句
//	delay_ms(13);	

//====================Initialize physical layer Version 1: Original version by Yang ==========================//
	//4. Initialize physical layer
	/*Description of PHYCN register (0x78)*/
	//bit 3: Lookback mode. 0 for disable, 1 for enable
	//bit 4: 0 for Half-duplex mode, 1 for Full-duplex mode. This bit is read-only when Auto-Negotiation is enabled
	//bit 5: RXEN: Receiver Enable. 1 for enable
	//bit 6: TXEN: Transmitter Enable. 1 for enable
	//bit 7: PHYEN: Physical Layer Enable. 1 for enable
	Write_Ethernet(0x78, 0x00);		//Firstly, disable physical layer. 
	delay_ms(13);
	Write_Ethernet(0x78, 0x00);		//Bit 3 = 0: Disable Lookback mode. Bit 4 = 0: Select Half-duplex mode 
	delay_ms(13);
	
	/*Description of PHYCF register (0x79)*/
	//bit 1: Automatic Receiver Polarity Correction. 0 for disable, 1 for enable.
	//bit 4: Auto-Negotiation function. 1 for enable.
	//bit 5: Jabber Protection Function. 1 for enable. Here no use.
	//bit 6: Link Integrity Function. 1 for enable.
	//bit 7: Receiver Smart Squelch. 1 for enable. Here no use
	Write_Ethernet(0x79, 0x52);		//0x52 = 0b0101 0010. Enable link integrity, auto-negotiation and automatic receiver polarity correction
	delay_ms(13);
	
	/*Description of Transmitter Power Register(TXPWR) (0x7A)*/
	//bit 7 : Transmitter Power Save Mode Disable Bit. 1 = Disable transmitter power saving mode
	//bit 0-6: Reserved.
	Write_Ethernet(0x7A, 0x80);		//0x80 = 0b1000 0000. Disable transmitter power saving move
	delay_ms(13);
	Write_Ethernet(0x78, 0x80);		//0x80 = 0b1000 0000. Enable physical layer. (PHYEN = 1)
	delay_ms(100);					//Wait for physical layer to power up, at least 1ms here 100ms
	Write_Ethernet(0x78, 0xE0);		//0xE0 = 0b1110 0000. Enable transmitter and receiver (TXEN=1, RXEN=1)
	delay_ms(13);	
//=============================================End of Version 1============================================//
	
////=============Initialize physical layer Version 2: Add the auto-negotiation synchronization procedure(Page 88) ============//
////实测感觉两个版本差异不大，可能要上位机对应修改抓包的频率，才能保证能够稳定抓到包
////Version 2 lp2 的逻辑还需要细微调整，看看效果如果。不过也可以调整上位机
//	//4. Initialize physical layer
//	//
//	//enable link integrity, turn off auto-negotiation
//	//disable power save mode , set physical option(phyCF)
//	//wait phy power up
//	//enable TXEN RXEN
//	//WAKEINT
//	Write_Ethernet(0x78, 0x00);		//Firstly, disable physical layer. 
//	delay_ms(13);
//	Write_Ethernet(0x78, 0x00);		//Bit 3 = 0: Disable Lookback mode. Bit 4 = 0: Select Half-duplex mode 
//	delay_ms(13);	
//	Write_Ethernet(0x79, 0x42);		//0x42 = 0b0100 0010. Enable link integrity and automatic receiver polarity correction, but disable auto-negotiation
//	delay_ms(13);
//	Write_Ethernet(0x7A, 0x80);		//0x80 = 0b1000 0000. Disable transmitter power saving move
//	delay_ms(13);
//	Write_Ethernet(0x78, 0x80);		//0x80 = 0b1000 0000. Enable physical layer. (PHYEN = 1)
//	delay_ms(100);					//Wait for physical layer to power up, at least 1ms here 100ms
//	Write_Ethernet(0x78, 0xE0);		//0xE0 = 0b1110 0000. Enable transmitter and receiver (TXEN=1, RXEN=1)
//	delay_ms(13);

//lp2:
//	connect = Read_Ethernet(0x7F);	//Check Wake-on-LAN interrupt flag to detect if a link partner is present
//	if (connect &= 0x20)			//0x20 = 0b0010 0000
//	{	
//		//If there is a connection signal
//		delay_ms(250);				//Wait 250ms
//		Write_Ethernet(0x79, 0x52);	//0x52 = 0b0101 0010. Begin auto-negotiation and enable link integrity, automatic receiver polarity correction
//		delay_ms(13);	
//		LED_R(0);
//		LED_G(1);					//Turn off LED_G and turn on LED_R when LAN connection establish
//	}
//	else
//	{
//		//If there is no signal
//		LED_G(0);					//Turn on LED_G to wait LAN connection
//		goto lp2;
////		delay_ms(1500);				//Wait 1.5s
////		Write_Ethernet(0x79, 0x52);	//0x52 = 0b0101 0010. Begin auto-negotiation and enable link integrity, automatic receiver polarity correction
////		delay_ms(13);
//	}		
////============================================================End of Version 2========================================================//

	//5. Enable Activity and Link LEDs
	/*Description of Port Input/Output Power Register(IOPWR) (0x70)*/
	//bit 1: Weak Pull-up Disable Bit. 1 = Disable Weak pull-up
	//bit 2: Link LED. 1 for enable.
	//bit 3: Activity LED. 1 for enable.
	Write_Ethernet(0x70, 0x0E);		//0x0E = 0b0000 1110
	delay_ms(250);
	
	//6. Initialize media access controller(MAC)
	//MAC is configured through nine 16-bit indirect MAC registers. Use 0x0A,0x0B,0x0C,0x0D to access them.
	/*Description of four direct mapped register: 0x0A,0x0B,0x0C,0x0D*/
	//0x0A: Holds the targeted indirect MAC register address
	//0x0B: Holds the High Byte of MAC data to read or write
	//0x0C: Holds the Low Byte of MAC data to read or write
	//0x0D: MAC Read/Write Initiate. 
	//Write: Write any value to 0x0D to transfer the contents of 0x0B and 0x0C to targeted indirect MAC register
	//Read: Perform a read on 0x0D, then read 0x0B and 0x0C 
	//The MAC must be set to the same duplex mode as the physical layer before sending or receiving any packets !!!!!!
	
	/*Description of MAC Configuration Register(MACCF) (0x01)*/
	//bit 0: 0 = MAC operates in Half-duplex mode
	//bit 1: Frame Length Checking Enable Bit. 1 = Transmit and receive frame lengths are compared to Length/Type field
	//bit 4: CRC Enable Bit. 1 = Enable CRC
	//bit 14: Abort Disable Bit. 1 = MAC will attempt to transmit indefinitely 一直发
	Write_Ethernet(0x0A, 0x01);		
	delay_ms(13);	
	Write_Ethernet(0x0C, 0x12);		//0x12 = 0b0001 0010 Low Byte
	delay_ms(13);
	Write_Ethernet(0x0B, 0x40);		//0x40 = 0b0100 0000 High Byte
	delay_ms(13);		
	Write_Ethernet(0x0D, 0x03);		//Trigger write operation(Write any value to 0x0D)
	delay_ms(13);	
	
	/***********************************下面的代码暂时用不上，用于配置TCP/IP*****************************/
	/*Description of IPGT (0x02)*/
	//bit 0-6: Sets the minimum delay between the end of any transmitted packet and the start of a new packet
	//In Half-Duplex mode, the register value should be set to the desired number of time units (each time unit is 0.46us) minus 6.
	//The recommended setting is 0x12 (18d), which yields 9.6us
	Write_Ethernet(0x0A, 0x02);		
	delay_ms(13);	
	Write_Ethernet(0x0C, 0x12);		//0x12 = 0b0001 0010 Low Byte. 9.6us delay between packet
	delay_ms(13);	
	Write_Ethernet(0x0B, 0x00);		//0x00 = 0b0000 0000 High Byte.	Must write 0x00
	delay_ms(13);	
	Write_Ethernet(0x0D, 0x03);		
	delay_ms(13);

	/*Description of IPGR (0x03)*/
	//bit 0-6: Sets the Non-Back-to-Back Inter-Packet Gap. Recommended value: 0x12 = a minimum inter-packet gap of 9.6us
	//bit 8-15: Sets the optional carrier sense window. The recommended value is 0x0C.
	Write_Ethernet(0x0A, 0x03);		
	delay_ms(13);	
	Write_Ethernet(0x0C, 0x12);		//0x12 = 0b0001 0010 Low Byte
	delay_ms(13);	
	Write_Ethernet(0x0B, 0x0C);		//0x0C = 0b0000 1100 High Byte
	delay_ms(13);	
	Write_Ethernet(0x0D, 0x03);		
	delay_ms(13);
	
	/*Description of Maximum Frame Length Register(MAXLEX) (0x05)*/
	//bit 0-15: Maximum Frame Length
	//Specifies the maximum length of a receive frame. The default value is 0x600 (1536 octets). 
	//This register should be programmed if a shorter maximum length restriction is desired. 
	//Examples of shorter frame lengths are untagged (1518 octets) and tagged (1522 octets).
	//If a proprietary header is allowed, this field should be adjusted accordingly
	//Here, write 0x05EE to MAXLEN
	Write_Ethernet(0x0A, 0x05);		
	delay_ms(13);	
	Write_Ethernet(0x0C, 0xEE);		//0xEE = 0b1110 1110 Low Byte
	delay_ms(13);	
	Write_Ethernet(0x0B, 0x05);		//0x05 = 0b0000 0101 High Byte
	delay_ms(13);	
	Write_Ethernet(0x0D, 0x03);		
	delay_ms(13);
	
	//Set MAC address
	//Program the 48-bit Ethernet MAC Address by writing to MACAD0:MACAD1:MACAD2
	/*Description of Flash Address Register High and Low Bytes*/
	//16-bit Address used for Flash operations
	Write_Ethernet(0x69, 0x1F);		//FLASHADDRH
	Write_Ethernet(0x68, 0xFA);		//FLASHADDRL
	
	/*Description of Flash AutoRead Data Register (0x05)*/
	//Data register used for reading a block of sequential data stored in Flash.
	//Each read from this register increments the Flash address register by 1
	MAC_Address[0] = Read_Ethernet(0x05);
	MAC_Address[1] = Read_Ethernet(0x05);
	MAC_Address[2] = Read_Ethernet(0x05);
	MAC_Address[3] = Read_Ethernet(0x05);
	MAC_Address[4] = Read_Ethernet(0x05);
	MAC_Address[5] = Read_Ethernet(0x05);
	
	/*Set the MAC address of the local device*/
	//MACAD0(0x10)	MACAD1(0x11)	MACAD2(0x12)
	Write_Ethernet(0x0A, 0x10);				//MAC_AD 0
	delay_ms(13);	
	Write_Ethernet(0x0C, MAC_Address[4]);	//Low Byte
	//MAC_Address[6] = Read_Ethernet(0x0C);	//原本是没有注释的，这里说超出数组范围。为什么在这里读？？？
	delay_ms(13);	
	Write_Ethernet(0x0B, MAC_Address[5]);	//High Byte
	delay_ms(13);	
	Write_Ethernet(0x0D, 0x03);		
	delay_ms(13);
	
	Write_Ethernet(0x0A, 0x11);				//MAC_AD 1
	delay_ms(13);	
	Write_Ethernet(0x0C, MAC_Address[2]);	//Low Byte
	delay_ms(13);	
	Write_Ethernet(0x0B, MAC_Address[3]);	//High Byte
	delay_ms(13);	
	Write_Ethernet(0x0D, 0x03);		
	delay_ms(13);
	
	Write_Ethernet(0x0A, 0x12);				//MAC_AD 2
	delay_ms(13);	
	Write_Ethernet(0x0C, MAC_Address[0]);	//Low Byte
	delay_ms(13);	
	Write_Ethernet(0x0B, MAC_Address[1]);	//High Byte
	delay_ms(13);	
	Write_Ethernet(0x0D, 0x03);		
	delay_ms(13);	
	
	/*Description of MAC Control Register(MACCN) (0x00)*/
	//bit 0: Receive enable. 1 = The MAC allows received packets to reach the receive interface.
	//Write 0x0001 to MACCN to enable reception.
	//If loopback mode or flow control is desired, set the appropriate bits to enable these functions.
	Write_Ethernet(0x0A, 0x00);		
	delay_ms(13);	
	Write_Ethernet(0x0C, 0x01);		//Low Byte
	delay_ms(13);	
	Write_Ethernet(0x0B, 0x00);		//High Byte
	delay_ms(13);	
	Write_Ethernet(0x0D, 0x03);		
	delay_ms(13);
	
	//7. Configure receive filter and Hash tables
	//Here no use	
}
