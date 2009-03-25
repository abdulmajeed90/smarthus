#include <stdlib.h>
#include <avr/io.h>
#include "global.h"
#include "uart.h"
#include <avr/interrupt.h>	// include interrupt support
//#include "timer.h"		// include timer function library (timing, PWM, etc)
#include "i2c.h"		// include i2c support
#include "ds1631.h"		// include DS1631 support
#include <util/delay.h>
#include <string.h>
#include <stdio.h>


#define BAUD 9600

char temp[8];
void ds1631init(void);
void ds1631getTemp(void);
FILE uart_str = FDEV_SETUP_STREAM(uartSendByte, uartGetByte, _FDEV_SETUP_RW);

int main(void)
{
	int vent=2000;
	CLKPR=0x80;
	CLKPR=0x00; //Hvorfor trengs dette???
	uartInit();
	uartSetBaudRate(BAUD);
	//rprintfInit(uartSendByte);
	stdout = stdin = &uart_str;
	// initialize the timer system
	//timerInit();
	printf("\r\nWelcome to DS6131 test!\r\n");
	printf("hei\r\n");
	
	ds1631init();
	while(1)
	{
		ds1631getTemp();
		printf(temp);
		printf("\r\n");
	}
	
	return 0;
	
}
void ds1631init(void)
{
	

	// initialize i2c function library
	i2cInit();
	i2cSetBitrate(100);

	// initialize
	if(ds1631Init(DS1631_I2C_ADDR))
	{
		printf("DS1631 detected and initialized!\r\n");
	}
	else
	{
		printf("Cannot detect DS1631!\r\n");
		return;
	}
	
	// set config
	ds1631SetConfig(DS1631_I2C_ADDR, 0x0F);
	// set the temperature limit registers
	ds1631SetTH(DS1631_I2C_ADDR, 35<<8);
	ds1631SetTL(DS1631_I2C_ADDR, 30<<8);
	// read them back for verification
	printf("TH is set to: %d\r\n",ds1631GetTH(DS1631_I2C_ADDR)>>8);
	printf("TL is set to: %d\r\n",ds1631GetTL(DS1631_I2C_ADDR)>>8);

	
}
void ds1631getTemp(void)
{
	s16 T=0;
	
	// start convert
	ds1631StartConvert(DS1631_I2C_ADDR);
	// wait until done
	// this works but seems to take forever (5-10 seconds)
	//while(!(ds1631GetConfig(DS1631_I2C_ADDR) & DS1631_CONFIG_DONE));
	// 12-bit conversion are only suppored to take this long
	//timerPause(750);
	_delay_ms(600);
	// read temp
	T = ds1631ReadTemp(DS1631_I2C_ADDR);
	// Print the formatted temperature reading
	sprintf(temp,"%d.%d", T>>8, (10*((T&0x00FF)))/256);
}
