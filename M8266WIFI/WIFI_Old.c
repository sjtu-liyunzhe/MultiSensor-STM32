#include "stdio.h"
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
#include "wifi.h"

/****************该程序用错了发送的函数所以丢帧严重**********************/

void Set_Socket(void)
{
	u16 status = 0;
	u8  link_no=0;

	// Test Type
	#define SEND_TYPE    		1		// 1 = Repeative Sending, 2 = Repeative Reception, 3 = Echo
	 
	// Set TCP/UDP Connections  
	#define CONNECTION_TYPE		2	    // 0=WIFI module as UDP, 1=WIFI module as TCP Client, 2=WIFI module as TCP Server

	#if (CONNECTION_TYPE==1)  	// TCP Client
		#define LOCAL_PORT  	0		// local port=0 then a random port will be generated each time. To avoid the rejection by some TCP server due to repeative same ip:port
	#else	 
		#define LOCAL_PORT  	4321
	#endif

	#if (CONNECTION_TYPE==2)  // TCP Server DO NOT need remote ip and port. below data has no usage, any value could be used for initialization. Will automatically changed to remote ip and port when connected by remote
	   #define REMOTE_ADDR      "1.1.1.1"
	   #define REMOTE_PORT  	1234
	#else
	   #define REMOTE_ADDR    	"192.168.4.1"   // "www.baidu.com"
	   #define REMOTE_PORT  	4321			// 80
	#endif			 

	// u8 M8266WIFI_SPI_Setup_Connection(u8 tcp_udp, u16 local_port, char remote_addr, u16 remote_port, u8 link_no, u8 timeout_in_s, u16* status);
	if(M8266WIFI_SPI_Setup_Connection(CONNECTION_TYPE, LOCAL_PORT, REMOTE_ADDR, REMOTE_PORT, link_no, 20, &status)==0)
	{		
		 while(1)
		 {}
	}
	else
	{
	#if (CONNECTION_TYPE == 0)  		// If config the wifi module as UDP, then the module could join a multicust group
	#if 0	//changed to "#if 1" if you hope to test multicust with UDP
			//u8 M8266WIFI_SPI_Set_Multicuast_Group(u8 join_not_leave, char multicust_group_ip[15+1], u16* status)
			if(M8266WIFI_SPI_Op_Multicuast_Group(0, "224.6.6.6", &status)==0)
			{
			   while(1)
			   {}
			}
			else
	#endif	//end of 	test multicust with UDP
	
	#elif (CONNECTION_TYPE == 2)  		// If config the wifi module as TCP server, then tcp server auto disconnection timeout could be set		
			//u8 M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout(u8 link_no, u16 timeout_in_s, u16* status)	
			if( M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout(link_no, 25, &status) == 0)
			{	}
			else
	#endif	// end of #if (CONNECTION_TYPE == 0) or #elif (CONNECTION_TYPE == 2)		 
		 {}	// this{} must be reserved !!!
	}
}

void Wifi_Send(void)
{
	u16 i;
	u16 status = 0;
	u8  link_no=0;

	#define SEND_DATA_SIZE   1000 //2048 // 1472 //1460	Byte

	u8  snd_data[SEND_DATA_SIZE];	// store the data to be sent
	u16 sent = 0;					// the Bytes of data actually sent out
	u32 total_sent = 0;
//	u32 MBytes = 0; 
	u16 batch;

	for(i=0; i<SEND_DATA_SIZE; i++) 	snd_data[i]=0x33; 	// write snd_data[]
		 
	link_no=0;	// send to connection with link_no=0
		
	for(batch = 0; batch < 1; batch++)				// times to send
	{
		snd_data[0] = 0x00;
		snd_data[1] = batch;
		snd_data[999] = 0xFF;

//		if(total_sent> 1024*1024)		// if 1MB data have been sent
//		{
//			 MBytes++;
//			 total_sent -= 1024*1024;	// clear the counter
//		}

		/////////////////////////////////////////////////////////
		// complex but high-efficiency transsmission test
		/////////////////////////////////////////////////////////
		//u16 M8266WIFI_SPI_Send_Data(u8 data[], u16 data_len, u8 link_no, u16* status)
		sent = M8266WIFI_SPI_Send_Data(snd_data, SEND_DATA_SIZE, link_no, &status);
		total_sent += sent;

		//M8266WIFI_Module_delay_ms(10);

		if( (sent!=SEND_DATA_SIZE) || ( (status&0xFF)!= 0 ) )	// data loss or send exception
		{
			if( (status&0xFF)==0x12 )				// 0x12 = Module send buffer full, and normally sent return with 0, i.e. this packet has not been sent anyway
			{                              
			  M8266HostIf_delay_us(250);       		// if 500Kbytes/s, need 2ms for 1Kbytes packet to be sent. If use 250us here, may repeat query 8- times, but in a small interval
				  //M8266WIFI_Module_delay_ms(1);	// if 500Kbytes/s, need 2ms for 1Kbytes packet to be sent. If use 1ms here,   may repeat query 2- times, but in a large interval
				  if(sent<SEND_DATA_SIZE)
					 total_sent += M8266WIFI_SPI_Send_Data(snd_data+sent, SEND_DATA_SIZE-sent, link_no, &status);  // try to resend it					
			}
			else if(  ((status&0xFF)==0x14)      	// 0x14 = connection of link_no not present
					  ||((status&0xFF)==0x15) )  	// 0x15 = connection of link_no closed
			{
				// do some work here, including re-establish the closed connect or link here and re-send the packet if any
				// additional work here
				  M8266HostIf_delay_us(249);	
			}
			else if	( (status&0xFF)==0x18)       	// 0x18 = TCP server in listening states and no tcp clients connecting to so far
			{
				  M8266HostIf_delay_us(251);	
			}				
			else                                 	// 0x10, 0x11 here may due to spi failure during sending, and normally packet has been sent partially, i.e. sent!=0
			{
				  M8266HostIf_delay_us(200);
				// do some work here					  
				  if(sent<SEND_DATA_SIZE)
						 total_sent += M8266WIFI_SPI_Send_Data(snd_data+sent, SEND_DATA_SIZE-sent, link_no, &status);  // try to resend the left packet
			}				
		}
				
	} // end of for(batch=0; ; batch++)
}
