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
#define BAUD 9600

FILE uart0_str = FDEV_SETUP_STREAM(uart0SendByte, NULL, _FDEV_SETUP_WRITE);
FILE uart1_str = FDEV_SETUP_STREAM(uart1SendByte, uart1GetByte, _FDEV_SETUP_RW);

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
    time_t time;//={21,3,7,3,1,3,9};
	uartInit();
	uartSetBaudRate(0,BAUD);
	uartSetBaudRate(1,BAUD);
	stdout = stdin = &uart1_str;
	stderr = &uart0_str;
	printf("HEI!!!!");
	fprintf(stderr, "Hello world!\n");
	i2cInit();
	i2cSetBitrate(100);	
	if (ds1307_enable(DS1307_I2C_ADDR)) printf("ds1307 Enabled");
	else (printf("ds1307 not found"));
	//ds1307_settime(DS1307_I2C_ADDR,time);
	//unsigned char hour, min, sec;
	//rtc_get_time(&hour, &min, &sec);
	//printf("%d:%d:%d",hour,min,sec);
	
	while(1)
	{	unsigned char tempSec=time.sec;
		if(ds1307_gettime(DS1307_I2C_ADDR, &time))
		{	if(tempSec!=time.sec)	
			//if((time.sec%10==0)&&(tempSec!=time.sec))
			{
				printf("\n\rTime:%d:%d:%d",time.hr,time.min,time.sec);
			}
		}
		else(printf("ERROR"));
	}
	return 0;	
}
