#include <stdlib.h>
#include <avr/io.h>
#include "global.h"
#include "uart2.h"
#include <avr/interrupt.h>	// include interrupt support
#include "i2c.h"			// include i2c support
#include "ds1307.h"			// include DS1631 support
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <avr/wdt.h> 

#define BAUD 9600
#define XBeeUART 0
#define EthUART 1

#define pSlave_modules 0
#define pHour 1 
#define pMin 2
#define pSec 3
#define pNumber 4
#define pType 5
#define pStatus 6
#define pTemp 7

#define pModuleStart 4 //First byte that is different for each module
#define pLastField 7
#define pFieldsModules 4 //Number of fields in one module = pLastField-pModuleStart+1
#define noOfModules 3
#define noOfBytes (pModuleStart+((1+pLastField-pModuleStart)*noOfModules))

typedef struct {
	u08 type;
	u08 status;
	u08 temp;
} slaveModule;

FILE uart0_str = FDEV_SETUP_STREAM(uart0SendByte, NULL, _FDEV_SETUP_WRITE);
FILE uart1_str = FDEV_SETUP_STREAM(uart1SendByte, uart1GetByte, _FDEV_SETUP_RW);

slaveModule sm[3]={{0,1,5},{0,0,15},{0,1,20}};
unsigned char ethPacket[noOfBytes];
unsigned char ethBuffer[noOfBytes];
time_t time;//={21,3,7,3,1,3,9};
void checkForEthPacket(void);
void sendEthPacket(void);

///////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: Main
//				
// ARGUMENTS:	
//
// MADE BY:		Jon Ove Storhaug  26.03.2009				
//////////////////////////////////////////////////////////////////////////////////


int main(void)
{
	//CLKPR=0x80;
	//CLKPR=0x00;
	//DDRB=0xff; //portB er utgang
	
	 
    
	uartInit();
	uartSetBaudRate(0,BAUD);
	uartSetBaudRate(1,BAUD);
	stdout = stdin = &uart1_str;
	stderr = &uart0_str;
	printf("HEI!!!!");
	fprintf(stderr, "Hello world!\n");
	i2cInit();
	i2cSetBitrate(100);	
	if (ds1307_enable(DS1307_I2C_ADDR)) 
		printf("ds1307 Enabled");
	else 
		printf("ds1307 not found");
	uartFlushReceiveBuffer(EthUART);
	uartFlushReceiveBuffer(XBeeUART);
	//ds1307_settime(DS1307_I2C_ADDR,time);
	unsigned char tempSec;
	while(1)
	{	
		wdt_enable(WDTO_2S);
		wdt_reset();
		
		tempSec=time.sec;
		if(ds1307_gettime(DS1307_I2C_ADDR, &time))
		{	//if(tempSec!=time.sec)	
			if((time.sec%10==0)&&(tempSec!=time.sec))
			{
				//printf("\n\rTime:%d:%d:%d",time.hr,time.min,time.sec);
				sendEthPacket();
			}
			//checkForEthPacket();
		}
		else(printf("ERROR"));
	}
	return 0;	
}
void checkForEthPacket(void)
{
	wdt_disable();
	if(!uartReceiveBufferIsEmpty(EthUART))
	{
		int i;
		i=0;
		while (i<noOfBytes)
		{
			while (uartReceiveBufferIsEmpty(EthUART))
				;;
			uartReceiveByte(EthUART, &ethPacket[i]);
			_delay_ms(100);
			i++;
		}
		//printf(ethPacket);
	}
}
void sendEthPacket(void)
{
	ethBuffer[pSlave_modules]=noOfModules;
	ethBuffer[pHour]=time.hr;
	ethBuffer[pMin]=time.min;
	ethBuffer[pSec]=time.sec;
	int number=0;
	while (number<noOfModules)
	{
		ethBuffer[pNumber+number*pFieldsModules]=number;
		ethBuffer[pType+number*pFieldsModules]=sm[number].type;
		ethBuffer[pStatus+number*pFieldsModules]=sm[number].status;
		ethBuffer[pTemp+number*pFieldsModules]=sm[number].temp;		
		number++;
	}
	for (int i=0;i<noOfBytes;i++)
	{
		uart1SendByte(ethBuffer[i]);
	}
}