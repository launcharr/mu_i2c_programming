/*  -------------------------------------------------------------------------------------------------
* Copyright 2013, Cypress Semiconductor Corporation.
*
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international
* treaty provisions. Cypress hereby grants to licensee a personal,
* non-exclusive, non-transferable license to copy, use, modify, create
* derivative works of, and compile the Cypress Source Code and derivative
* works for the sole purpose of creating custom software in support of
* licensee product to be used only in conjunction with a Cypress integrated
* circuit as specified in the applicable agreement. Any reproduction,
* modification, translation, compilation, or representation of this
* software except as specified above is prohibited without the express
* written permission of Cypress.
* 
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND,EXPRESS OR IMPLIED,
* WITH REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising
* out of the application or use of any product or circuit described herein.
* Cypress does not authorize its products for use as critical components in
* life-support systems where a malfunction or failure may reasonably be
* expected to result in significant injury to the user. The inclusion of
* Cypress' product in a life-support systems application implies that the
* manufacturer assumes all risk of such use and in doing so indemnifies
* Cypress against all charges.
* 
* Use may be limited by and subject to the applicable Cypress software
* license agreement.
* ---------------------------------------------------------------------------------------------------
* Copyright (c) Cypress MicroSystems 2000-2003. All Rights Reserved.
*
*****************************************************************************************************
*  Project Name: I2C_Bootloader_Host_PSoC5LP
*  Project Revision: 1.00
*  Software Version: PSoC Creator 2.2
*  Device Tested: CY8C5868AXI-LP035
*  Compilers Tested: ARM GCC
*  Related Hardware: CY8CKIT-050
*****************************************************************************************************
****************************************************************************************************/

/****************************************************************************************************
* Project Description:
* This is a sample bootloader host program demonstrating PSoC5LP bootloading PSoC3.
* The project is tested using CY8CKIT-050 with PSoC 5LP chip and CY8CKIT-030 with PSoC 3 chip.
* PSoC3 must be programmed with the I2C Bootloader Program attached with the app note.
*
* Connections Required
* CY8CKIT-050 (PSoC 5LP DVK) :
*  P5[3] - SDA - Should be pulled to Vcc using 1.8k resistors and connected to SDA of PSoC3
*  P5[5] - SCL - Should be pulled to Vcc using 1.8k resistors and connected to SCL of PSoC3 
*  P6.1 is internally connected to SW1 on DVK.
*
* CY8CKIT-030 (PSoC 3 DVK) : PSoC  3 intially programmed with I2C_Bootloader program.
*  P0[1] - SDA - Connected to SDA of PSoC 5LP
*  P0[0] - SCL - Connected to SCL of PSoC 5LP
*  P6.1 is internally connected to SW1 on DVK.
*
* Note that the GNDs of both DVKs should be connected together.
*
* Bootload function is defined in main.c: BootloadStringImage which uses Bootloader Host APIs
* to bootload the contents of .cyacd file.
*
* BootloadStringImage function requires each line of .cyacd to be stored as a seperate string in a string array.
* The total number of lines in the file should also be manually calculated and stored as #define LINE_CNT
* The string image of .cyacd file is stored in StringImage.h file.
*
* The following events happens alternatively on each switch press
* On first switch press it will bootload 'stringImage_1' stored in StringImage.h using BootloadStringImage function . 
* On next switch press it will bootload 'stringImage_2' stored in StringImage.h using BootloadStringImage function .
*
* These bootloadable images display either "Hello" or "Bye" on the first row of LCD(PSoC 3 DVK)
* Note that in order to enter the bootloader from the application program P6.1 - connected to SW1
* should be pressed so that the application program running on PSoC3 enters Bootloader and is ready to 
* bootload a new application code/data . 
*
* I2C Slave Addr of the bootloader is set to 0x04 in communication_api.h
****************************************************************************************************/

#include "string.h"
#include "cybtldr_api2.h"
#include "cybtldr_parse.h"
#include "cybtldr_command.h"
#include "communication_api.h"
#include "cybtldr_api.h"
#include "StringImage.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define BRDCST_STAT 1

uint8_t broadcast_percentage(int fd, struct sockaddr_in addr, uint8_t perc);
void send_status(unsigned char arrayId, unsigned short rowNum);
/* This structure contains function pointers to the four communication layer functions 
   contained in the communication_api.c / .h */
CyBtldr_CommunicationsData comm1;
char ssh_ip[16];
char my_ip[16];
int ssh_port;
struct sockaddr_in sendaddr, myaddr;
int fd;



int main(int argc, char **argv)
{
    /* error holds the success / failure message of the bootload operation */
	uint8_t rdBuf, rd4[10], strt = 0x77;
	unsigned char securityKey[] = {0x11,0x22,0x33,0x44,0x55,0x66}, appID = 1;
	int32_t time;
	uint16_t error = 0;
	int ii, j=0, n = 1000;

	if (argc > 1) {
		strcpy(ssh_ip, argv[1]);
		ssh_port = atoi(argv[2]);
		strcpy(my_ip, argv[3]);
		printf("SSH IP = %s\nSSH PORT = %d\nMY IP = %s\n", ssh_ip, ssh_port, my_ip);
	}

	slave_addr = SLAVE_ADDR;
	/* Display the instruction to the user on how to start bootloading */
	printf("Bootloading board start\n");

	/* Initialize the communication structure element -comm1 */
	comm1.OpenConnection = &OpenConnection;
	comm1.CloseConnection = &CloseConnection;
	comm1.ReadData = &ReadData;
	comm1.WriteData =&WriteData;
	comm1.MaxTransferSize = 32;

	#if BRDCST_STAT == 1
	        if( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
			printf("Error creating a socket!!\n");
			broadcast_percentage(fd, sendaddr, 110);
			return 1;
		}
		// set and bindaddress of the pi
	        memset(&myaddr,0,sizeof(myaddr));
		myaddr.sin_family = AF_INET;
		inet_pton(AF_INET, my_ip, &myaddr.sin_addr.s_addr);
	        myaddr.sin_port=htons(0);

		if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
			printf("Bind failed!!\n");
			broadcast_percentage(fd, sendaddr, 111);
			close(fd);
			return 1;
		}

	        // set up destination address
	        memset(&sendaddr,0,sizeof(sendaddr));
		sendaddr.sin_family = AF_INET;
		inet_pton(AF_INET, ssh_ip, &sendaddr.sin_addr.s_addr);
	        sendaddr.sin_port=htons(ssh_port);
		broadcast_percentage(fd, sendaddr, 0);
	#endif

	OpenConnection();

	RequestReadData(0x04, &rdBuf, 1);
	if(rdBuf != 0x65) {
		printf("Not in bootloader or I2C not working propperly.\n");
		broadcast_percentage(fd, sendaddr, 120);
		return 0;
	}

	RequestReadData(0x77, &rdBuf, 1);

	CloseConnection();

	usleep(100);
	printf("Bootloading...\n");

	error = CyBtldr_Program(CYACD_PATH, securityKey, appID, &comm1, &send_status);

	/* Check if the bootload operation is successful */
	if(error == CYRET_SUCCESS)
	{
		/* Display the success message */
		printf("Bootloading succesful\n");
	}
	else
	{
		/* Display the failure message along with the approiate error code */
		if(error & CYRET_ERR_COMM_MASK) /* Check for comm error*/
		{
			printf("I2C communication Error\n");
			broadcast_percentage(fd, sendaddr, 130);
		}
		else /* Else Display the bootload error code */
		{
			printf("Bootload Err: %08x \n", error);
			broadcast_percentage(fd, sendaddr, 131);
		}
	}
	return 0;
}

void send_status(unsigned char arrayId, unsigned short rowNum) {

	printf("arrayId: %02x\nRowNum: %d\n", arrayId, rowNum);
	//broadcast_percentage(fd, sendaddr, (uint8_t)(100*rowNum/LINE_CNT));
}

uint8_t broadcast_percentage(int fd, struct sockaddr_in addr, uint8_t perc) {

	//char *message;

	if (sendto(fd, &perc, sizeof(perc), 0,(struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		perror("Sending failed!!\n");
		return 0;
	}
	return 1;
}




/* [] END OF FILE */
