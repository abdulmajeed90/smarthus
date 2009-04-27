/**
 * \file mmcom.c
 * \author  Jon Ove Storhaug <jostorha@gmail.com>
 * \version 1.0
 * \brief Serial communication between Webserver and Main Module 
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
#include "mmcom.h"


int checkForEthPacket(unsigned char* ethPacket)
{
	if(!uartReceiveBufferIsEmpty())
	{
		int i;
		i=0;
		while (i<noOfBytes)
		{
			while (uartReceiveBufferIsEmpty())
				;;
			uartReceiveByte(&ethPacket[i]);
			i++;
		}
		uartFlushReceiveBuffer();
		return 0;
	}
	else
		return 0;
}
void sendEthPacket(time_t time, slaveModule* sm)
{
	unsigned char ethBuffer[noOfBytes];
	ethBuffer[pSlave_modules]=noOfModules;
	ethBuffer[pYear]=time.yr;
	ethBuffer[pMonth]=time.mon;
	ethBuffer[pDate]=time.dat;
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
		uartSendByte(ethBuffer[i]);
	}
}
