/*******************************************************************************
* File Name: communication_api.c  
* Version 1.0
*
* Description:
* This file is created by the author . This contains definitions of APIs 
* used in structure 'CyBtldr_CommunicationsData' defined in cybtldr_api.h ,
* using I2C commuincations component 
********************************************************************************/

#include "communication_api.h"
//#include "StringImage.h"
#define DELAY_US 35000
int pi, file_i2c, i2cbus = 1;
uint8_t sda = 2, scl = 3;

/*******************************************************************************
* Function Name: OpenConnection
********************************************************************************
*
* Summary:
*  Initializes the communications component : In this case I2C
*
* Parameters:  
*  void
*
* Return: 
*  Returns a flag to indicate whether the operation was successful or not
*
*
*******************************************************************************/
int OpenConnection()
{
	int i;

	if (pi = pigpio_start(NULL, NULL) < 0) {
		printf("Failed to initialise the gpio bus");
		return(CYRET_ERR_COMM_MASK);
	}

	if( bb_i2c_open(pi, sda, scl, 100000) != 0) {
		printf("Failed to open the i2c bus");
		return(CYRET_ERR_COMM_MASK);
	}

	/*
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		printf("Failed to open the i2c bus");
		return(CYRET_ERR_COMM_MASK);
	}

	if (ioctl(file_i2c, I2C_SLAVE, SLAVE_ADDR) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return(CYRET_ERR_COMM_MASK);
	}
	*/
	usleep(DELAY_US);

	return(CYRET_SUCCESS);
}


/*******************************************************************************
* Function Name: CloseConnection
********************************************************************************
*
* Summary:
*  Clears the status and stops the communications component(I2C).
*
* Parameters:  
*  void
*
* Return: 
*  Returns a flag to indicate whether the operation was successful or not
*
*
*******************************************************************************/
int CloseConnection(void)
{
	int i;
	bb_i2c_close(pi, sda);
	pigpio_stop(pi);

	usleep(DELAY_US);
	return(CYRET_SUCCESS);
}

/*******************************************************************************
* Function Name: WriteData
********************************************************************************
*
* Summary:
*  Writes the specified number of bytes usign the communications component(I2C)
*
* Parameters:  
*  wrData - Pointer to write data buffer
*  byteCnt - No. of bytes to be written 
*
* Return: 
*  Returns a flag to indicate whether the operation was successful or not
*
*
*******************************************************************************/
int WriteData(uint8_t* wrData, int byteCnt)
{
	int i, ret;
	uint8_t inLen = 5+byteCnt+2;
	uint8_t inBuf[inLen];

	inBuf[0] = 0x04; //set addr
	inBuf[1] = slave_addr; // addr
	inBuf[2] = 0x02; // start
	inBuf[3] = 0x07; // write
	inBuf[4] = byteCnt; // byteCnt

	for(i=0;i<byteCnt;i++) {
		inBuf[5+i] = wrData[i]; // put data in buffer
	}
	inBuf[inLen-2] = 0x03; // stop conditio
	inBuf[inLen-1] = 0x00; // end command

	// PGPIO lib
	if (ret = bb_i2c_zip(pi, sda, inBuf, inLen, NULL, 0) < 0) {
		printf("Unable to write data... %d\n", ret);
		return(CYRET_ERR_COMM_MASK);
	}


	/* Give some delay before the next operation so that the slave can process the written data */
	usleep(DELAY_US);

	return(CYRET_SUCCESS);

}

/*******************************************************************************
* Function Name: RequestWriteData
********************************************************************************
*

* Summary:
*  Request write of the specified number of bytes usign the communications component(I2C)
*
* Parameters:  
*  req - Request specific location for data 
*  wrData - Pointer to write data buffer
*  byteCnt - No. of bytes to be written 
*
* Return: 
*  Returns a flag to indicate whether the operation was successful or not
*
*

*******************************************************************************/
int RequestWriteData(uint8_t req, uint8_t* wrData, int byteCnt) {

	int i;
	uint8_t inLen = 9+byteCnt+2;
	uint8_t inBuf[inLen];

	if(byteCnt > 32) {
		printf("Byte count larger than 32 bits...\n");
		return(CYRET_ERR_COMM_MASK);
	}

	inBuf[0] = 0x04; //set addr
	inBuf[1] = slave_addr; // addr
	inBuf[2] = 0x02; // start
	inBuf[3] = 0x07; // write
	inBuf[4] = 1; // request size
	inBuf[5] = req; // request
	inBuf[6] = 0x02; // (re)start
	inBuf[7] = 0x07; // write
	inBuf[8] = byteCnt; // amount of data to be written
	for(i=0;i<byteCnt;i++) {
		inBuf[9+i] = wrData[i]; // put data in buffer
	}
	inBuf[inLen-2] = 0x03; // stop
	inBuf[inLen-1] = 0x00; // end commands

	// write request
	if (bb_i2c_zip(pi, sda, inBuf, inLen, NULL, 0) != byteCnt)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write to the i2c bus.\n");
		return(CYRET_ERR_COMM_MASK);
	}

	/* Give some delay before the next operation so that the slave can process the written data */
	usleep(DELAY_US);

	return(CYRET_SUCCESS);
}

/*******************************************************************************
* Function Name: ReadData
********************************************************************************
*
* Summary:
*  Reads the specified number of bytes usign the communications component(I2C)
*
* Parameters:  
*  rdData - Pointer to read data buffer
*  byteCnt - No. of bytes to be read 
*
* Return: 
*  Returns a flag to indicate whether the operation was successful or not
*
*
*******************************************************************************/
int ReadData(uint8_t* rdData, int byteCnt)
{
	int i, ret;
	uint8_t inLen = 7;
	uint8_t inBuf[inLen];

	inBuf[0] = 0x04; //set addr
	inBuf[1] = slave_addr; // addr
	inBuf[2] = 0x02; // start
	inBuf[3] = 0x06; // read
	inBuf[4] = byteCnt; // byteCnt
	inBuf[5] = 0x03; // stop conditio
	inBuf[6] = 0x00; // end command

	// PGPIO lib
	if (ret = bb_i2c_zip(pi, sda, inBuf, inLen, rdData, byteCnt) < 0) {
		printf("Unable to write data... %d\n", ret);
		return(CYRET_ERR_COMM_MASK);
	}

	usleep(DELAY_US/20);

	return(CYRET_SUCCESS);
}

/*******************************************************************************
* Function Name: ReadData
********************************************************************************
*
* Summary:
*  Reads the specified number of bytes usign the communications component(I2C)

*
* Parameters:  
*  rdData - Pointer to read data buffer
*  byteCnt - No. of bytes to be read 
*

* Return: 
*  Returns a flag to indicate whether the operation was successful or not
*
*
*******************************************************************************/
int RequestReadData(uint8_t req, uint8_t* rdData, int byteCnt)
{
	int i, num;
	uint8_t inLen = 11;
	uint8_t inBuf[inLen];
	char outBuf;

	if(byteCnt > 32) {
		printf("Byte count larger than 32 bits...\n");
		return(CYRET_ERR_COMM_MASK);
	}

	inBuf[0] = 0x04; //set addr
	inBuf[1] = slave_addr; // addr
	inBuf[2] = 0x02; // start
	inBuf[3] = 0x07; // write
	inBuf[4] = 1; // request size
	inBuf[5] = req; // request
	inBuf[6] = 0x02; // (re)start
	inBuf[7] = 0x06; // read
	inBuf[8] = (uint8_t) byteCnt; // amount of data to be read
	inBuf[9] = 0x03; // stop
	inBuf[10] = 0x00; // end commands

	// write request
	if (num = bb_i2c_zip(pi, sda, inBuf, inLen, rdData, byteCnt) != byteCnt)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to read from the i2c bus. data = %x num = %d byteCnt = %d\n", rdData[0], num, byteCnt);
		return(CYRET_ERR_COMM_MASK);
	}

	usleep(DELAY_US);

	return(CYRET_SUCCESS);
}

/* [] END OF FILE */

