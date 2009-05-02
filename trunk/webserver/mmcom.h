/**
 * \file mmcom.h
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

#ifndef MMCOM_H_
#define MMCOM_H_

#include "uart.h"

#define pSlave_modules 0 ///< Number of Slave Modules connected
#define pYear 1
#define pMonth 2
#define pDate 3
#define pHour 4 
#define pMin 5
#define pSec 6
#define pNumber 7 
#define pType 8
#define pStatus 9
#define pTemp 10

#define pModuleStart 7 ///<First byte that is different for each module
#define pLastField 10
#define pFieldsModules 4 ///<Number of fields in one module = pLastField-pModuleStart+1
#define noOfModules 3
#define noOfBytes (pModuleStart+((1+pLastField-pModuleStart)*noOfModules))

/// Slave_module structure
///
typedef struct  { 
	unsigned char type; ///< Type of slave module. 0=heat control
	unsigned char status; ///< Status of slave module. 0=off, 1=on, 2=no change
	signed char temp; ///< The temperature in degrees Celcius
} slaveModule;

///Time structure
///
typedef struct {
	u08 sec ;
	u08 min ;
	u08 hr  ;
	u08 day ;
	u08 dat ;
	u08 mon ;
	u08 yr ;
} time_t;

extern unsigned char stop_mmcomm;

//! Checks if a packet is ready to be received
///
int checkForEthPacket(signed char* ethPacket);

//! Send a packet
//! \param time The actual time the packet is sent (sec, min, hr, dat, mon, yr)
///
void sendEthPacket(time_t time, slaveModule* sm);




#endif /*MMCOM_H_*/
