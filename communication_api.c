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
#define DELAY_US 28000
int file_i2c, i2cbus = 1;

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

	if (gpioInitialise() < 0) {
		printf("Failed to initialise the gpio bus");
		return(CYRET_ERR_COMM_MASK);
	}

	if( file_i2c = i2cOpen(i2cbus, slave_addr, 0) < 0) {
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
	gpioTerminate();
	i2cClose(file_i2c);

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
	int i, ii, ret;
	//struct i2c_msg msg;
	//struct i2c_rdwr_ioctl_data pack;

	/*if(byteCnt > 32) {
		printf("Byte count larger than 32 bits...\n");
		return(CYRET_ERR_COMM_MASK);
	}*/
	// WRITE
	/*
	if (write(file_i2c, wrData, byteCnt) != byteCnt)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		//ERROR HANDLING: i2c transaction failed
		printf("Failed to write to the i2c bus.\n");
		return(CYRET_ERR_COMM_MASK);
	}
	*/

	// SMBUS byte by byte
	/*
	for(i=0;i<byteCnt;i++) {
		i2c_smbus_write_byte(file_i2c, wrData[i]);
		for(ii=0;ii<100;ii++);
	}*/


	// IOCTL lib
	/*
	msg.addr = SLAVE_ADDR;
	msg.flags = 0;
	msg.len = byteCnt;
	msg.buf = wrData;

	pack.msgs = &msg;
	pack.nmsgs = 1;

	if( ioctl(file_i2c, I2C_RDWR, &pack) < 0 ) {
		printf("Unable to write data...\n");
		return(CYRET_ERR_COMM_MASK);
	}
	*/

	// PGPIO lib
	if (ret = i2cWriteDevice(file_i2c, wrData, byteCnt) != 0) {
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

	if(byteCnt > 32) {
		printf("Byte count larger than 32 bits...\n");
		return(CYRET_ERR_COMM_MASK);
	}
    // write request
	if (i2cWriteI2CBlockData(file_i2c, req, wrData, byteCnt) != byteCnt)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
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
	int i, ii, ret;
	//struct i2c_msg msg;
	//struct i2c_rdwr_ioctl_data pack;

	/*if(byteCnt > 32) {
		printf("Byte count larger than 32 bits...\n");
		return(CYRET_ERR_COMM_MASK);
	}*/

	/* //----- READ BYTES -----
	if (read(file_i2c, rdData, byteCnt) != byteCnt)		//read() returns the number of bytes actually read, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		//ERROR HANDLING: i2c transaction failed
		printf("Failed to read from the i2c bus.\n");
		return(CYRET_ERR_COMM_MASK);
	}
	*/
	/*
	for(i=0;i<byteCnt;i++) {
		rdData[i] = i2c_smbus_read_byte(file_i2c);
		for(ii=0;ii<100;ii++);
	}*/
	/*
	msg.addr = SLAVE_ADDR;
	msg.flags = I2C_M_RD;
	msg.len = byteCnt;
	msg.buf = rdData;

	pack.msgs = &msg;
	pack.nmsgs = 1;

	if( ioctl(file_i2c, I2C_RDWR, &pack) < 0) {
		printf("Unable to read data...");
		return(CYRET_ERR_COMM_MASK);
	}
	*/

	// PIGPIO lib
	if (ret = i2cReadDevice(file_i2c, rdData, byteCnt) != byteCnt) {
		printf("Unable to read data... %d\n", ret);
		return(CYRET_ERR_COMM_MASK);
	}


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

	if(byteCnt > 32) {
		printf("Byte count larger than 32 bits...\n");
		return(CYRET_ERR_COMM_MASK);
	}
    // write request
	if (num = i2cReadI2CBlockData(file_i2c, req, rdData, byteCnt) != byteCnt)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to read from the i2c bus. data = %x num = %d byteCnt = %d\n", rdData[0], num, byteCnt);
		return(CYRET_ERR_COMM_MASK);
	}

	usleep(DELAY_US);

	return(CYRET_SUCCESS);
}

/* [] END OF FILE */

