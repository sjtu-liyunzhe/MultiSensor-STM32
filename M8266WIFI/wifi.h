#ifndef _WIFI_H
#define _WIFI_H

//SCLK		PB13		
//MISO		PB14		
//MOSI		PB15		
//nCS		PH12	
//nRST		PH11	

void Wifi_Init(void);
void Set_Socket(void);
void Wifi_Send_Packet(void);

void Wifi_Test(void);

#endif

