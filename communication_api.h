/*******************************************************************************
* File Name: i2c_comm.h  
* Version 1.0
*
* Description:
* This is the header file for the communication module created by the author.  
* It contains function prototypes and constants for the users convenience. 
********************************************************************************/

#include "cybtldr_utils.h"
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include <linux/i2c.h>
#include <pigpio.h>
//#include "StringImage.h"
/* Slave address for I2C used for bootloading.Replace this constant value with your
*  bootloader slave address project */
//#define SLAVE_ADDR 0x55
uint8_t slave_addr;

/* Function declarations */
int OpenConnection(void);
int CloseConnection(void);
int ReadData(uint8_t* rdData, int byteCnt);
int RequestReadData(uint8_t req, uint8_t* rdData, int byteCnt);

int WriteData(uint8_t* wrData, int byteCnt);
int RequestWriteData(uint8_t req, uint8_t* wrData, int byteCnt);



//[] END OF FILE

