#ifndef WEBCOM_H_
#define WEBCOM_H_

#define XBeeUART 0
#define EthUART 1
#define pSlave_modules 0
#define pHour 1 
#define pMin 2
#define pSec 3
#define pNumber 4
#define pType 5
#define pStatus 6
#define pTemp 7

#define pModuleStart 4 //First byte that is different for each module
#define pLastField 7
#define pFieldsModules 4 //Number of fields in one module = pLastField-pModuleStart+1
#define noOfModules 3
#define noOfBytes (pModuleStart+((1+pLastField-pModuleStart)*noOfModules))

typedef struct  {
	unsigned char type;
	unsigned char status;
	unsigned char temp;
} slaveModule;






#endif /*webcom_H_*/
