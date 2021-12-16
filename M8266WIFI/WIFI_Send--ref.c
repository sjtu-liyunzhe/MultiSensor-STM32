/********************************************************************
 * WIFI_Send.c
 * .Description
 *     Send ultrasound data via wifi 
 ********************************************************************/
 
#include "stdio.h"
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
//#include "led.h"

void WIFI_Send(void)
{
	 u16 i;
	 u16 status = 0;
	 u8  link_no=0;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	 SETUP  SOCKET CONNECTIONS 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
	/********************************************设置测试模式和套接字参数**************************************************/ 
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

	/****************************************************这里建立服务器****************************************************/
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
		 {}	//这对括号需要保留！！！
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	#if (SEND_TYPE==1) 		// Repeative sending mode: to test send data to remote constantly
	{
		#define SEND_DATA_SIZE   1024 //2048 // 1472 //1460	Byte

		u8  snd_data[SEND_DATA_SIZE];	// store the data to be sent
		u16 sent = 0;					// the Bytes of data actually sent out
		u32 total_sent = 0, MBytes = 0; 
		u16 batch;

		for(i=0; i<SEND_DATA_SIZE; i++) snd_data[i]=i; 	// write snd_data[]
			 
		link_no=0;	// send to connection with link_no=0
			
		for(batch = 0; ; batch++)				// 设置发送的循环次数
		{
			snd_data[0]= batch>>8;
			snd_data[1]= batch&0xFF;

			if(total_sent> 1024*1024)		// if 1MB data have been sent
			{
				 MBytes++;
				 total_sent -= 1024*1024;	// clear the counter
			}

			#if 0  // if use simple but less-efficiency transmission, changed to "#if 1"
			//u32 M8266WIFI_SPI_Send_Data_Block(u8 Data[], u32 Data_len, u16 tcp_packet_size, u8 link_no, u16* status); 
			sent = M8266WIFI_SPI_Send_Data_Block(snd_data, SEND_DATA_SIZE, 1460, link_no, &status);		//just use 1460
			total_sent += sent;
			  
			if( (sent!=SEND_DATA_SIZE) || ( (status&0xFF)!= 0 ) ) 	// 如果丢数据或者发送失败
			{
				if(  ((status&0xFF)==0x14)      // 0x14 = connection of link_no not present
				   ||((status&0xFF)==0x15) )    // 0x15 = connection of link_no closed
				{
					// do some work here, including re-establish the closed connect or link here and re-send the packet if any
					// additional work here
					  M8266HostIf_delay_us(249);	
				}
				else if	( (status&0xFF)==0x18)  // 0x18 = TCP server in listening states and no tcp clients connecting to so far
				{
					  M8266HostIf_delay_us(250);	
				}
				else if	( (status&0xFF)==0x1E)  // 0x1E: too many errors ecountered during sending can not fixed
				{
					  M8266HostIf_delay_us(251);	
				}
				else                            // 0x10, 0x11 here may due to spi failure during sending, and normally packet has been sent partially, i.e. sent!=0
				{
				      M8266HostIf_delay_us(200);
					// do some work here					  
					  if(sent<SEND_DATA_SIZE)
							 total_sent += M8266WIFI_SPI_Send_Data(snd_data+sent, SEND_DATA_SIZE-sent, link_no, &status);  // try to resend the left packet
				}
			}

			#else 	// complex but high-efficiency transsmission test
			//u16 M8266WIFI_SPI_Send_Data(u8 data[], u16 data_len, u8 link_no, u16* status)
			sent = M8266WIFI_SPI_Send_Data(snd_data, SEND_DATA_SIZE, link_no, &status);
			total_sent += sent;

			//M8266WIFI_Module_delay_ms(10);

			if( (sent!=SEND_DATA_SIZE) || ( (status&0xFF)!= 0 ) )	// 如果丢数据或者发送失败 
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
			#endif		
		} // end of for(batch=0; ; batch++)
	 }
		
	 
//	//////////////////////////////////////////////////////////////////////////////////////////
//	//////////////////////////////////////////////////////////////////////////////////////////
//	 #elif (SEND_TYPE==2) 	 // Repeative reception mode: to test constant reception from remote
//	{
//		#define  RECV_DATA_MAX_SIZE  2048

//		u8  RecvData[RECV_DATA_MAX_SIZE];
//		u16 received = 0;
//		u32 total_received = 0;
//		u32 MBytes = 0;
//			
//		for(i=0; i<RECV_DATA_MAX_SIZE; i++) 	RecvData[i]=0xFF-i; 	
//		M8266WIFI_SPI_Send_Data(RecvData, 1024, link_no, &status);  // just send to check the ip address on TCP tester. 
//			
//			 while(1) // TCP工具连续循环发TCP包时，可能会出现粘包拆包现象。例如，两个1024字节的包，可能会被重新打包成1460+588两个包发送
//			 {
//					if(M8266WIFI_SPI_Has_DataReceived())
//					{
//						//u16 M8266WIFI_SPI_RecvData(u8 data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u16* status);
//						received = M8266WIFI_SPI_RecvData(RecvData, RECV_DATA_MAX_SIZE, 5*1000, &link_no, &status);

//						if(  (status&0xFF)!= 0 )  
//						{
//							if( (status&0xFF)==0x22 )      // 0x22 = Module buffer has no data received
//							{  
//								M8266HostIf_delay_us(250); 
//							   //M8266WIFI_Module_delay_ms(1);
//							}
//							else if( (status&0xFF)==0x23 )   
//							{ 		// the packet had not been received completed in the last call of M8266WIFI_SPI_RecvData()
//									// and has continued to be received in this call of M8266WIFI_SPI_RecvData()
//									// do some work here if necessary
//							}
//							else if(  (status&0xFF)==0x24)   
//							{ 
//									// the packet is large in size than max_len specified and received only the max_len // TCP上的粘包和拆包现象
//									// normally caused by the burst transmision by the routers after some block. 
//									// Suggest to stop the TCP transmission for some time
//									// do some work here if necessary
//							}
//							else
//							{
//									// do some work here if necessary
//							}
//						}
//						
//						#if 0 // commented it if to avoid delay by printf
//						if((status&0xFF)!=0)
//						{					 
//							printf("\r\n Received data from from M8266 with an error, status = 0x%04X\r\n", status);			 
//						}
//						else
//						{					 
//							printf("\r\n Received data from from M8266 with no error\r\n");			 
//						}
//						printf(" Received Data[%d] from M8266 = ", received);
//						for(i=0; i<received; i++)
//						{
//							if(i%8==0) printf("\r\n  %03d :", i);
//							printf(" %02X", RecvData[i]);
//						}
//						printf("\r\n");
//						#endif
//						#define  TEST_RECV_UNIT (1024*1024)
//						total_received += received;
//						if( total_received >= (TEST_RECV_UNIT) )
//						{
//							LED_set(0, MBytes&0x01);	
//							total_received = total_received%(TEST_RECV_UNIT);
//							MBytes++;
//						}
//					} // end of if(M8266WIFI_SPI_Has_DataReceived())				 
//			 } // end of while(1)
//	 }	 

//	//////////////////////////////////////////////////////////////////////////////////////////
//	//////////////////////////////////////////////////////////////////////////////////////////
//	#elif (SEND_TYPE==3)  	// Echo test: to receive data from remote and then echo back to remote
//	{
//		#define  RECV_DATA_MAX_SIZE  2048    

//		u8  RecvData[RECV_DATA_MAX_SIZE];   // make sure the stack size is more than RECV_DATA_MAX_SIZE
//		u16 received = 0;
//		u16 sent;
//			
//		for(i=0; i<RECV_DATA_MAX_SIZE; i++) 	RecvData[i]=i; 

//		link_no = 0;
//		sent = M8266WIFI_SPI_Send_Data(RecvData, 1024, link_no, &status);

//		while(1)
//		{
//			if(M8266WIFI_SPI_Has_DataReceived())
//			{
//				//u16 M8266WIFI_SPI_RecvData(u8 data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u16* status);
//				received = M8266WIFI_SPI_RecvData(RecvData, RECV_DATA_MAX_SIZE, 5*1000, &link_no, &status);

//				#if 0 // commented it if to avoid delay by printf
//				if((status&0xFF)!=0)
//				{					 
//					printf("\r\n Received data from from M8266 with an error, status = 0x%04X\r\n", status);			 
//				}
//				else
//				{					 
//					printf("\r\n Received data from from M8266 with no error\r\n");			 
//				}
//				printf(" Received Data[%d] from M8266 = ", received);
//				for(i=0; i<received; i++)
//				{
//					if(i%8==0) printf("\r\n  %03d :", i);
//					printf(" %02X", data[i]);
//				}
//				printf("\r\n");
//				#endif

//				// Step 2: echo the data
//				if(received!=0)
//				{
//					//u16 M8266WIFI_SPI_Send_Data(u8 data[], u16 data_len, u8 link_no, u16* status);
//					sent = M8266WIFI_SPI_Send_Data(RecvData, received, link_no, &status);

//					if( (sent!=received) || ( (status&0xFF)!= 0 ) ) 
//					{
//						if(  ((status&0xFF)==0x14)     // 0x14 = connection of link_no not present
//						  ||((status&0xFF)==0x15) )    // 0x15 = connection of link_no closed
//						{
//							// do some work here, including re-establish the closed connect or link here and re-send the packet if any
//							// additional work here
//							M8266HostIf_delay_us(249);	
//						}
//						else if	( (status&0xFF)==0x18)       // 0x18 = TCP server in listening states and no tcp clients connecting to so far
//						{
//							M8266HostIf_delay_us(251);	
//						}				
//						else 
//						{
//							if( (status&0xFF)==0x12 )  		 // 0x12 = Module send buffer full, and normally sent return with 0, i.e. this packet has not been sent anyway
//							{                              
//								M8266HostIf_delay_us(250);   // if 500Kbytes/s, need 2ms for 1Kbytes packet to be sent. If use 250us here, may repeat query 8- times, but in a small interval
//							}
//							while(sent<received)
//							{
//								sent += M8266WIFI_SPI_Send_Data(RecvData+sent, received-sent, link_no, &status);  // try to resend it					
//								M8266HostIf_delay_us(10);
//							}
//						}
//						}
//						#if 0	// to avoid print bring about delay	during test
//						if( (sent!=received) || ( (status&0xFF)!= 0 ) )
//						{
//							printf("\r\n Send Data[len=%d] to M8266 failed. Only %d bytes has been sent(0x%04X).\r\n", received, sent, status);
//							break;
//						}
//						else
//						{
//							printf("\r\n Send Data[len=%d] to M8266 successfully(%04X).\r\n", received, status);
//						}
//						#endif					
//					}
//			} // end of if(M8266WIFI_SPI_Has_DataReceived())
//		} // end of while(1)
//	}	 
//	 
//	/////////////////////////////////////////////////////////////////////////////
//	/////////////////////////////////////////////////////////////////////////////
//	#else
//	#error NOT Supported Test Type! should be 1~3!		
	#endif

} // end of M8266WIFI_Test
