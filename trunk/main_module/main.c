#include <stdlib.h>
#include <avr/io.h>
#include "global.h"
#include "uart2.h"
#include <avr/interrupt.h>	// include interrupt support
#include "i2c.h"			// include i2c support
#include "ds1631.h"			// include DS1631 support
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
//#define NAME "Router1"		//Navnet Router1 blir definert som NAME
#define BAUD 9600

void rtc_init(unsigned char rs, unsigned char sqwe, unsigned char out);
void rtc_get_time(unsigned char *hour, unsigned char *min, unsigned char *sec);

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
    uartInit();
	uartSetBaudRate(0,BAUD);
	uartSetBaudRate(1,BAUD);
	stdout = stdin = &uart1_str;
	stderr = &uart0_str;
	printf("HEI!!!!");
	fprintf(stderr, "Hello world!\n");
	i2cInit();
	i2cSetBitrate(100);	
	rtc_init(0,0,0);
	unsigned char hour, min, sec;
	rtc_get_time(&hour, &min, &sec);
	printf("%d:%d:%d",hour,min,sec);
	while(1)
	{
		//readtobuffer();
	}
	return 0;	
}

void rtc_init(unsigned char rs, unsigned char sqwe, unsigned char out)
{	
	rs&=3;
	if (sqwe)
		rs|=0x10;
	if (out)
		rs|=0x80;
	i2cSendStart();
	i2cSendByte(0xd0);
	i2cSendByte(7);
	i2cSendByte(rs);
	i2cSendStop();
}
void rtc_get_time(unsigned char *hour, unsigned char *min, unsigned char *sec)
{
	i2cSendStart();
	i2cSendByte(0xd0);
	i2cSendByte(0);
	i2cSendStart();
	i2cSendByte(0xd1);
	i2cReceiveByte(TRUE);
	*sec=(i2cGetReceivedByte());
	i2cReceiveByte(TRUE);
	*min=(i2cGetReceivedByte());
	i2cReceiveByte(TRUE);
	*hour=(i2cGetReceivedByte());
	i2cSendStop();
}