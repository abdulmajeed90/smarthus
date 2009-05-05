#include <stdlib.h>
#include <avr/io.h>
#include "global.h"
#include "uart.h"
#include <avr/interrupt.h>	// include interrupt support
#include "i2c.h"			// include i2c support
#include "ds1631.h"			// include DS1631 support
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <avr/wdt.h>
#define BAUD 9600

 uint8_t mcusr_mirror;

    void get_mcusr(void) \
      __attribute__((naked)) \
      __attribute__((section(".init3")));
    


void compare(void);
void sendPacket(void);
void readtobuffer(void);
void XBeeGetName(void);
void ds1631init(void);
void ds1631getTemp(void);

int i=0;
signed char temp;
int counter=0;
char buffer1[11];
char name[10];//="~ROUTER0";
FILE uart_str = FDEV_SETUP_STREAM(uartSendByte, uartGetByte, _FDEV_SETUP_RW);

///////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: Main
//				
// ARGUMENTS:	
//
// MADE BY:		Jon Ove Storhaug, Asbjørn Tveito  26.03.2009				
//////////////////////////////////////////////////////////////////////////////////


int main(void)
{
	get_mcusr();
	CLKPR=0x80;
	CLKPR=0x00;
	DDRB=0xff; //portB er utgang
    uartInit();
	uartSetBaudRate(BAUD);
	stdout = stdin = &uart_str;
	_delay_ms(2000);
	ds1631init();
	XBeeGetName();
		
	wdt_enable(WDTO_8S);
	wdt_reset();
	while(1)
	{		
		wdt_reset();
		readtobuffer();
	}
	return 0;	
}





///////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: To read incomming data from Main module
//				
// ARGUMENTS:	
//
// MADE BY:		Jon Ove Storhaug, Asbjørn Tveito  26.03.2009				
//////////////////////////////////////////////////////////////////////////////////


void readtobuffer(void)
{
	while(uartReceiveBufferIsEmpty())
		;;
	if (uartReceiveByte(&buffer1[counter]))
	{
		if(buffer1[0]=='~')
		{
			counter++;
			
			if (counter>=11)
			{
				counter=0;
				compare();
			}
		}
	}


}


///////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: TO compare incomming data against the modules name   
//				
// ARGUMENTS:	
//
// MADE BY:		Jon Ove Storhaug, Asbjørn Tveito  26.03.2009				
//////////////////////////////////////////////////////////////////////////////////	

	
void compare(void)
{

	if (strncmp(buffer1,name,8)==0)
	{
		if((buffer1[9]=='o')&&(buffer1[10]=='n'))
		{
			PORTB |=1<<1;
			ds1631getTemp();
			sendPacket();
		}
		else if((buffer1[9]=='o')&&(buffer1[10]=='f'))
		{
			PORTB &=~(1<<1);
			ds1631getTemp();
			sendPacket();
		}
		else if((buffer1[9]=='n')&&(buffer1[10]=='c'))
		{
			ds1631getTemp();
			sendPacket();
		}
            
	}
}
	
	


///////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:  TO transmit name, status and temperature back to Main module  
//				
// ARGUMENTS:	
//
// MADE BY:		Jon Ove Storhaug, Asbjørn Tveito  26.03.2009				
//////////////////////////////////////////////////////////////////////////////////



void sendPacket(void)
{
    char status[3];
	printf(name);
    if(PINB&(1<<PINB1)) 
    sprintf(status,"on");
    else
		sprintf(status,"of");
 
	printf("_");
	printf(status);
	printf("_");
	uartSendByte(temp);
	//printf(temp);
}
///////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: The XBee's name is read and saved in the global variable name
//
// ARGUMENTS:	
//
// MADE BY:		Jon Ove Storhaug  29.03.2009				
//////////////////////////////////////////////////////////////////////////////////

void XBeeGetName(void)
{
	unsigned char status[4];
	uartFlushReceiveBuffer();
	printf("+++"); // Enter command mode
	int i=0;
	// Check if "OK\r" is received to verify command mode
// 	while(i<3) 
// 	{
// 		if (uartReceiveByte(&status[i]))
// 			i++;
// 	}
// 	if(!(status[0]=='O') && !(status[1]=='K'))
// 	{
// 		_delay_ms(1100);
// 		printf("atcn\r"); // Exit command mode
// 		_delay_ms(1100);
// 		printf("ERROR"); // Send ERROR message
// 	}
// 	else 
		_delay_ms(1100);
		uartFlushReceiveBuffer();
		printf("atni\r"); // Ask for XBee's name
	i=1;
	// Receive the name
	while(i!=20)
	{
		name[0]='~';
		if (uartReceiveByte(&name[i]))
		{
			if (name[i]=='\r')
			{
				name[i]='\0';
				i=20;
			}
			else
				i++;
		}
	}
	printf("atcn\r"); // Exit command mode
// 	i=0;
// 	// Check if "OK\r" is received to verify the exit from command mode
// 	while(i<3)
// 	{
// 		if (uartReceiveByte(&status[i]))
// 			i++;
// 	}
// 	if(!(status[0]=='O') && !(status[1]=='K'))
// 	{
// 		printf("atcn\r"); // Exit command mode
// 		_delay_ms(1100);
// 		printf("ERROR"); // Send ERROR message
// 	}
	uartFlushReceiveBuffer();
}
///////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: Initialize the ds1631/ds1621 temperature sensor 
//				
// ARGUMENTS:	
//
// MADE BY:		Jon Ove Storhaug 26.03.2009				
//////////////////////////////////////////////////////////////////////////////////

void ds1631init(void)
{
	

	// initialize i2c function library
	i2cInit();
	i2cSetBitrate(100);

	// initialize
	//if(ds1631Init(DS1631_I2C_ADDR))
	//{
	//	printf("DS1631 detected and initialized!\r\n");
	//}
	//else
	//{
	//	printf("Cannot detect DS1631!\r\n");
	//	return;
	//}
	
	// set config
	ds1631SetConfig(DS1631_I2C_ADDR, 0x0F);
	// set the temperature limit registers
	ds1631SetTH(DS1631_I2C_ADDR, 35<<8);
	ds1631SetTL(DS1631_I2C_ADDR, 30<<8);
	// read them back for verification
	//printf("TH is set to: %d\r\n",ds1631GetTH(DS1631_I2C_ADDR)>>8);
	//printf("TL is set to: %d\r\n",ds1631GetTL(DS1631_I2C_ADDR)>>8);

	
}

///////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: Gets the temperature from DS1631/DS1621 and saves it in the 
//              string named temp 
// ARGUMENTS:	
//
// MADE BY:		Jon Ove Storhaug  26.03.2009				
//////////////////////////////////////////////////////////////////////////////////

void ds1631getTemp(void)
{
	s16 T=0;	
	// start convert
	ds1631StartConvert(DS1631_I2C_ADDR);
	// wait until done
	// this works but seems to take forever (5-10 seconds)
	while(!(ds1631GetConfig(DS1631_I2C_ADDR) & DS1631_CONFIG_DONE));
	// 12-bit conversion are only suppored to take this long
	//_delay_ms(750);
	// Read temp
	T = ds1631ReadTemp(DS1631_I2C_ADDR);
	// Insert the formatted temperature reading in the string temp 
	temp=T>>8;
	//sprintf(temp,"%d.%d", T>>8, (10*((T&0x00FF)))/256);
}
void get_mcusr(void)
    {
      mcusr_mirror = MCUSR;
      MCUSR = 0;
      wdt_disable();
    }