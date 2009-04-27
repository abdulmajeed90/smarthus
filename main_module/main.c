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



time_t time;//={21,3,7,3,1,3,9};
slaveModule sm[noOfModules]={{0,1,5},{0,0,15},{0,1,20}};
unsigned char ethPacket[noOfBytes];

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
				sendEthPacket(time, sm);
			}
			wdt_disable();
			if (checkForEthPacket(&ethPacket))
			{
				for (int i=0;i<noOfBytes;i++)
				{
					uart1SendByte(ethPacket[i]);
				}
			}
		}
		else(printf("ERROR"));
	}
	return 0;	
}