#include "webcom.h"
#include "uart.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>



slaveModule sm[3]={{0,1,5},{0,0,15},{0,1,20}};
unsigned char ethPacket[noOfBytes];
unsigned char ethBuffer[noOfBytes];



void checkForEthPacket(void)
{
	if(!uartReceiveBufferIsEmpty(EthUART))
	{
		int i;
		i=0;
		while (i<noOfBytes)
		{
			while (uartReceiveBufferIsEmpty(EthUART))
				;;
			uartReceiveByte(EthUART, &ethPacket[i]);
			_delay_ms(10);
			i++;
		}
		//printf(ethPacket);
	}
}
void sendEthPacket(time_t time)
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
