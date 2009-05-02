/**
 * \file
 * \author  Jon Ove Storhaug <jostorha@gmail.com>
 * \version 1.0
 * \brief Main Module 
 *
 * \section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * \section DESCRIPTION
 *
 * This file is used for sending and receiving packets to and from the Webserver
 * and the main module
 */
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
#include "webcom.h"

#define BAUD 9600
#define XBeeUART 0
#define EthUART 1





FILE uart0_str = FDEV_SETUP_STREAM(uart0SendByte, NULL, _FDEV_SETUP_WRITE);
FILE uart1_str = FDEV_SETUP_STREAM(uart1SendByte, uart1GetByte, _FDEV_SETUP_RW);

void sendSlaveModule(unsigned char number,unsigned char status);
uint8_t mcusr_mirror;

void get_mcusr(void) \
	__attribute__((naked)) \
		__attribute__((section(".init3")));

time_t time={3,37,1,3,2,4,9};
slaveModule sm[noOfModules]={{0,1,-5},{0,0,15},{0,1,20}};
slaveModule sm_web[noOfModules];
signed char ethPacket[noOfBytes];
unsigned char XBeePacketCounter[noOfModules];

int main(void)
{	
	get_mcusr();
    
	uartInit();
	uartSetBaudRate(0,BAUD);
	uartSetBaudRate(1,BAUD);
	stdout = stdin = &uart1_str;
	stderr = &uart0_str;
	//fprintf(stderr, "Hello world!\n");
	i2cInit();
	i2cSetBitrate(100);	
// 	if (ds1307_enable(DS1307_I2C_ADDR)) 
// 		printf("ds1307 Enabled");
// 	else 
// 		printf("ds1307 not found");
	ds1307_enable(DS1307_I2C_ADDR);
	uartFlushReceiveBuffer(EthUART);
	uartFlushReceiveBuffer(XBeeUART);
	//ds1307_settime(DS1307_I2C_ADDR,time);
	unsigned char tempSec;
	int e=0;
	DDRB=0xff;
	PORTD=0xff;
	_delay_ms(1000);
	DDRB=0x00;
	PORTD=0x00;
// 	while(e<noOfBytes)
// 	{
// 		uart1SendByte(1);
// 		e++;
// 		_delay_ms(2);
// 	}
	uartFlushReceiveBuffer(EthUART);
	//_delay_ms(10000);
	wdt_enable(WDTO_2S);
	wdt_reset();
	while(1)
	{	
		wdt_reset();
		//wdt_enable(WDTO_2S);
		//wdt_reset();
		
		//sendEthPacket(time, sm);
		
		tempSec=time.sec;
		if(ds1307_gettime(DS1307_I2C_ADDR, &time))
		{	//if(tempSec!=time.sec)	
			if((time.sec%2==0)&&(tempSec!=time.sec))
			{
				//printf("\n\rTime:%d:%d:%d",time.hr,time.min,time.sec);
				//sendEthPacket(time, sm);
				if (checkForEthPacket(ethPacket))
				{
					if (0)//edit)
					{
						time.yr=ethPacket[pYear];
						time.mon=ethPacket[pMonth];
						time.dat=ethPacket[pDate];
						time.hr=ethPacket[pHour];
						time.min=ethPacket[pMin];
						ds1307_settime(DS1307_I2C_ADDR,time);
					}
					int number=0;
					while (number<noOfModules)
					{
						sm_web[number].type=ethPacket[pType+number*pFieldsModules];
						sm_web[number].status=ethPacket[pStatus+number*pFieldsModules];
						sm_web[number].temp=ethPacket[pTemp+number*pFieldsModules];
						switch (sm_web[number].type)
						{
							case 0:
								if (sm_web[number].status==0)
									sendSlaveModule(number,0);
								else
								{
									if ((sm[number].temp)<(sm_web[number].temp-1))
										sendSlaveModule(number,1);
									else if ((sm[number].temp)>(sm_web[number].temp+1))
										sendSlaveModule(number,0);
									else 
										sendSlaveModule(number,2);
								}
								break;
						}
						number++;
					}
				}
				//fprintf(stderr, "Sender pakke!\n");
				
			
				while(!uartReceiveBufferIsEmpty(XBeeUART))
				{
					//fprintf(stderr, "mottar pakke!\n");
					int i;
					i=0;
					signed char XBeePacket[12];
					while (i<13)
					{
						while (uartReceiveBufferIsEmpty(XBeeUART))
							;;
						uartReceiveByte(XBeeUART, &XBeePacket[i]);
						if ((XBeePacket[i]=='$')&&(i!=0))
							i=0;
						else if (XBeePacket[0]=='$')
							i++;
					}
					//fprintf(stderr, "har mottatt alt!\n");
					sm[XBeePacket[7]-'0'].temp=XBeePacket[12];
					if (XBeePacket[10]=='f')
						sm[XBeePacket[7]-'0'].status=0;
					else if (XBeePacket[10]=='n')
						sm[XBeePacket[7]-'0'].status=1;
					XBeePacketCounter[XBeePacket[7]-'0']=1;
				}
				for (int i=0; i<noOfModules; i++)
				{		
					if (!XBeePacketCounter[i])
						sm[i].status=2;
					XBeePacketCounter[i]=0;
				}
				sendEthPacket(time, sm);
			}
		}
		else(printf("ERROR_klokkefeil"));
	}				
	return 0;	
}

void sendSlaveModule(unsigned char number,unsigned char status)
{
	switch (status)
	{
		case 0:
			fprintf(stderr,"$ROUTER%d_of",number);
			break;
		case 1:
			fprintf(stderr,"$ROUTER%d_on",number);
			break;
		case 2:
			fprintf(stderr,"$ROUTER%d_nc",number);
			break;
	}
}
void get_mcusr(void)
    {
      mcusr_mirror = MCUSR;
      MCUSR = 0;
      wdt_disable();
    }