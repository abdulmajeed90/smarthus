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
#define NAME "Router1"		//Navnet Router1 blir definert som NAME
#define BAUD 9600

int i=0;
char temp[8];
int counter=0;
void ds1631init(void);
void ds1631getTemp(void);
void compare(void);
void sendPacket(void);
void readtobuffer(void);
unsigned char buffer1[10];
FILE uart_str = FDEV_SETUP_STREAM(uartSendByte, uartGetByte, _FDEV_SETUP_RW);


///////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: To read incomming data from RS232
//				
// ARGUMENTS:	
//
// MADE BY:		Jon Ove Storhaug, Asbjørn Tveito  26.03.2009				
//////////////////////////////////////////////////////////////////////////////////


void readtobuffer(void)
{
	if (uartReceiveByte(&buffer1[counter]))
	{
		counter++;
		if (counter>=10)
		{
			counter=0;
			compare();
		}
	}


}


///////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: TO compare incomming data against the moduls name   
//				
// ARGUMENTS:	
//
// MADE BY:		Jon Ove Storhaug, Asbjørn Tveito  26.03.2009				
//////////////////////////////////////////////////////////////////////////////////	

	
void compare(void)
{

	if (strncmp(buffer1,NAME,7)==0)
		{
			if((buffer1[8]=='o')&&(buffer1[9]=='n'))
			{
                PORTB |=1<<1;
                ds1631getTemp();
                sendPacket();
            }
			else if((buffer1[8]=='o')&&(buffer1[9]=='f'))
			{
                PORTB &=~(1<<1);
                ds1631getTemp();
                sendPacket();
			}
			else if((buffer1[8]=='n')&&(buffer1[9]=='c'))
            {
                ds1631getTemp();
				sendPacket();
            }
            
		}
	
	else
	{
		printf ("ERROR");
		return;
	}	


///////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:  TO transmit Routername, status and temperature back to main modul  
//				
// ARGUMENTS:	
//
// MADE BY:		Jon Ove Storhaug, Asbjørn Tveito  26.03.2009				
//////////////////////////////////////////////////////////////////////////////////


}
void sendPacket(void)
{
    char status[3];
    printf(NAME);
    if(PINB&(1<<PINB1)) 
    sprintf(status,"on");
    else
		sprintf(status,"of");
 
	printf("_");
	printf(status);
	printf("_");
	printf(temp);
}


///////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: Main Structure    
//				
// ARGUMENTS:	
//
// MADE BY:		Jon Ove Storhaug, Asbjørn Tveito  26.03.2009				
//////////////////////////////////////////////////////////////////////////////////


int main(void)
{
	int vent=2000;
	CLKPR=0x80;
	CLKPR=0x00;
	DDRB=0xff; //portB er utgang
    uartInit();
	uartSetBaudRate(BAUD);
	stdout = stdin = &uart_str;
	ds1631init();
	
	while(1)
	{
		readtobuffer();
	}
	return 0;
	
}

///////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: 
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
// DESCRIPTION: 
//
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
	//while(!(ds1631GetConfig(DS1631_I2C_ADDR) & DS1631_CONFIG_DONE));
	// 12-bit conversion are only suppored to take this long
	_delay_ms(750);
	// Read temp
	T = ds1631ReadTemp(DS1631_I2C_ADDR);
	// Insert the formatted temperature reading in the string temp 
	sprintf(temp,"%d.%d", T>>8, (10*((T&0x00FF)))/256);
}
