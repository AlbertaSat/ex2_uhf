/** @file HL_sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2015 Texas Instruments Incorporated - www.ti.com
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  INCLUDING NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>    // Include file for Win32 time functions
#include <conio.h>      // Include file for _getch/kbhit 
#include <string.h> 

#include "HL_sys_common.h"

/* USER CODE BEGIN (1) */
/* Include FreeRTOS scheduler files */
#include "FreeRTOS.h"
#include "os_task.h"

/* Include HET header file - types, definitions and function declarations for system driver */
#include "HL_het.h"
#include "HL_gio.h"

/* Include the CAN bootloader test files */
#include "bl_commands.h"
#include "can_bltest.h"
#include "HL_can.h"
#include "HL_esm.h"
#include "HL_system.h"
#include "HL_sys_core.h"

#define D_COUNT 8
uint32 cnt=0, error =0, tx_done =0;
uint8 tx_data1[D_COUNT] = {1,2,3,4,5,6,7,8};
uint8 tx_data2[D_COUNT] = {11,12,13,14,15,16,17,18};
uint8 tx_data3[D_COUNT] = {21,22,23,24,25,26,27,28};
uint8 tx_data4[D_COUNT] = {31,32,33,34,35,36,37,38};
uint8 rx_data1[D_COUNT] = {0};
uint8 rx_data2[D_COUNT] = {0};
uint8 rx_data3[D_COUNT] = {0};
uint8 rx_data4[D_COUNT] = {0};
uint8 *dptr=0;

u32					i;
u16					j;
u8                  k;
char				Interface[6] = "CAN0";
char				output[15];
char				CharBuff[50];
int					XTD = 0; // Set XTD = 1 to send extended ID frames
int					id = CAN_COMMAND_PING;
char				data_length = 8;

char                *filename = "TMS570LS31Blinky.bin";
int                 rcvID;
int                 rcvData;	
int                 rcvDataLen;
unsigned char ulIdx;
unsigned short ulOffset;
unsigned long ulLength;
unsigned long ulAddress;

uint8   emacAddress[6U] =   {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};
uint32  emacPhyAddress  =   1U;

//////////////////////////////////////////////// Blinky Task code
/* Define Task Handles */
xTaskHandle xTask1Handle;

/* Task1 */
void vTask1(void *pvParameters)
{
    for(;;)
    {
        /* Taggle HET[1] with timer tick */
        gioSetBit(gioPORTB, 6, gioGetBit(gioPORTB, 6) ^ 1);
        vTaskDelay(100);
    }
}
/* USER CODE END */


//////////////////////////////////////////////// CAN declarations and variables
#define CAN_CMD_SUCCESS         0x00
#define CAN_CMD_FAIL            0x01

#define BUFFER_SIZE  2048

static unsigned char g_pucBuffer[256];

unsigned char image[BUFFER_SIZE*8];
unsigned long image_size;

static unsigned char data_in[BUFFER_SIZE];
static unsigned char data_out[BUFFER_SIZE];

// Below use NI CAN - not being used
// // Handle for Network Interface Object (receiver)
// NCTYPE_OBJH		RxHandle = 0;
// // Handle for the CAN object (transmitter)
// NCTYPE_OBJH		TxHandle = 0;

// /* NI-CAN handle */
// NCTYPE_OBJH	TxRx=0;

//////////////////////////////////////////////// CAN code

/* This function converts the absolute time obtained from ncReadMult into a string. */
void AbsTimeToString()
{
   // uses NI CAN - not implemented
}

/* This function is used to print a description of an NI-CAN error/warning. */
void PrintStat()
{
	// uses NI CAN - not implemented
}

void delay(void) {
	static volatile unsigned int delayval;
	delayval = 10000000;   // 100000 are about 10ms
	while(delayval--);
}

void Get_BinaryData( char *filename )
{
    FILE *file;
    int trans_num = 0;
	unsigned char *pImage;

	image_size = 0;
	pImage = &image[0];

    // Open the file
    file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        return;
    }

    while (!feof(file)) {
        int num_write, count;
        int i;

        // Read from the file
        num_write = fread((void *)pImage, 1, BUFFER_SIZE, file);
        if (!num_write)  break;

        fprintf(stderr, "*** Transaction #%02d\n", trans_num);

        // Dump the data to the screen
        fprintf(stderr, "Data written to device:");
        for (i = image_size; i < (image_size+num_write); ++i) {
            if ((i&0x0f) == 0)      fprintf(stderr, "\n%04x:  ", i);
            fprintf(stderr, "%02x ", image[i] & 0xff);
            if (((i+1)&0x07) == 0)  fprintf(stderr, " ");
        }
        fprintf(stderr, "\n\n");

		image_size += num_write;
		pImage += num_write;
        ++trans_num;

    }

cleanup:
    fclose(file);
}

//////////////////////////////////////////////// main()
//=========================================================================
// MAIN PROGRAM
//=========================================================================
void main(void)
{
// /* USER CODE BEGIN (3) */

//     /* Set high end timer GIO port hetPort pin direction to all output */
//     gioInit();
//     gioSetDirection(gioPORTB, 0xFFFFFFFF);


//     /* Create Task 1 */
//     if (xTaskCreate(vTask1,"Task1", configMINIMAL_STACK_SIZE, NULL, 1, &xTask1Handle) != pdTRUE)
//     {
//         /* Task could not be created */
//         while(1);
//     }

//     /* Start Scheduler */
//     vTaskStartScheduler();

//     /* Run forever */
//     while(1);
// /* USER CODE END */


    /////////////
    /* Configure the CAN Network Interface Object */
    // TODO: replace all these values
	// AttrIdList[0] =         NC_ATTR_BAUD_RATE;   
	// AttrValueList[0] =      Baudrate;
	// AttrIdList[1] =         NC_ATTR_START_ON_OPEN;
	// AttrValueList[1] =      NC_TRUE;
	// AttrIdList[2] =         NC_ATTR_READ_Q_LEN;
	// AttrValueList[2] =      150;
	// AttrIdList[3] =         NC_ATTR_WRITE_Q_LEN;
	// AttrValueList[3] =      0;	
	// AttrIdList[4] =         NC_ATTR_CAN_COMP_STD;
	// AttrValueList[4] =      0;
	// AttrIdList[5] =         NC_ATTR_CAN_MASK_STD;
	// AttrValueList[5] =      NC_CAN_MASK_STD_DONTCARE;
	// AttrIdList[6] =         NC_ATTR_CAN_COMP_XTD;
	// AttrValueList[6] =      0;
	// AttrIdList[7] =         NC_ATTR_CAN_MASK_XTD;
	// AttrValueList[7] =      NC_CAN_MASK_XTD_DONTCARE;

	// Status = ncConfig(Interface, 8, AttrIdList, AttrValueList);
	// if (Status < 0) 
	// {
	//    PrintStat(Status, "ncConfig");
	// }

    // /* open the CAN Network Interface Object */
	// Status = ncOpenObject(Interface, &TxHandle);
	// if (Status < 0) 
	// {
	// 	PrintStat(Status, "ncOpenObject");
	// }   
	// /* open the CAN Network Interface Object */
	// Status = ncOpenObject(Interface, &RxHandle);
	// if (Status < 0) 
	// {
	// 	PrintStat(Status, "ncOpenObject");
	// }   

    // /* print out the instructions to the I/O window */
	// printf("\n\ninitialized successfuly on CAN0 ... \n\n");

	/* enable irq interrupt in */
    _enable_IRQ_interrupt_();

    canInit();

	/** - enabling error interrupts */
	canEnableErrorNotification(canREG1);
	canEnableErrorNotification(canREG2);
	canEnableErrorNotification(canREG3);
	canEnableErrorNotification(canREG4);

	/*==============================================================================
    * Basic Test
    *===============================================================================*/
    int status1;
	int status2;
	int status3;
	int status4;

	fprintf(stderr,"Transmitting CAN");

	status1 = canTransmit(canREG1, canMESSAGE_BOX1, (const uint8 *) &tx_data1[0]);
	status2 = canTransmit(canREG2, canMESSAGE_BOX1, (const uint8 *) &tx_data2[0]);
	status3 = canTransmit(canREG3, canMESSAGE_BOX1, (const uint8 *) &tx_data3[0]);
	status4 = canTransmit(canREG4, canMESSAGE_BOX1, (const uint8 *) &tx_data4[0]);

	if (status1 + status2 + status3 + status4 != 4) {
		fprintf(stderr, "TX message box setup unsuccessful.");
		fprintf(stderr, "status1 value: %i", status1);
		fprintf(stderr, "status2 value: %i", status2);
		fprintf(stderr, "status3 value: %i", status3);
		fprintf(stderr, "status4 value: %i", status4);
	}

	// is canIsRxMessageArrived the right method?
	while(!canIsRxMessageArrived(canREG1,canMESSAGE_BOX1)){
		canGetData(canREG1, canMESSAGE_BOX1, rx_data1);
		canGetData(canREG2, canMESSAGE_BOX1, rx_data2);
		canGetData(canREG3, canMESSAGE_BOX1, rx_data3);
		canGetData(canREG4, canMESSAGE_BOX1, rx_data4);
	}

	fprintf(stderr,"Message has arrived");

	// likely not applicable here because it isn't a loopback
	// todo: find a way to test that the code is received on the other board
	for(int i = 0; i < 8; i++){
		if(tx_data1[i] != rx_data1[i]){
			fprintf(stderr, "can 1, index = %d error\n", i);
			fprintf(stderr, "tx = %d, rx = %d\n", tx_data1[i], rx_data1[i]);
		}
		else if(tx_data2[i] != rx_data2[i]){
			fprintf(stderr,"can 2, index = %d error\n", i);
		}
		else if(tx_data3[i] != rx_data3[i]){
			fprintf(stderr,"can 3, index = %d error\n", i);
		}
		else if(tx_data4[i] != rx_data4[i]){
			fprintf(stderr,"can 4, index = %d error\n", i);
		}

		else if(i == 7){
			fprintf(stderr,"Checking tx and rx complete");
		}
	}

	/*==============================================================================
    * COMMAND Download
    *===============================================================================*/
	Get_BinaryData( filename );
	ulAddress = 0x00020000;

	Transmit.DataLength = 8;
	Transmit.IsRemote = 0;
	Transmit.ArbitrationId = CAN_COMMAND_DOWNLOAD;	

	Transmit.Data[0] = (ulAddress >> 24) & 0xff;
	Transmit.Data[1] = (ulAddress >> 16) & 0Xff;
	Transmit.Data[2] = (ulAddress >> 8) & 0xff;
	Transmit.Data[3] = ulAddress & 0xff;
	Transmit.Data[4] = (image_size >> 24) & 0xff;
	Transmit.Data[5] = (image_size >> 16) & 0xff;
	Transmit.Data[6] = (image_size >> 8) & 0xff;
	Transmit.Data[7] = image_size & 0xff;

	Status= ncWrite(TxHandle, sizeof(Transmit), &Transmit);
	Status = 	
	if (Status < 0) 
	{
		PrintStat(Status, "ncWrite");
	}

	Status = ncRead(RxHandle, sizeof(ReceiveBuf), (void *)ReceiveBuf);
	if (Status < 0)
	{
			PrintStat(Status, "ncReadMult");
	}
	rcvID = ReceiveBuf[0].ArbitrationId;
	rcvData = ReceiveBuf[0].Data[0];
    rcvDataLen = ReceiveBuf[0].DataLength;
	if((rcvID != 0x05a6) && (rcvData != 0))
	{
		printf(" - COMMAND_DOWNLOAD failed!\n");
		return 0;
	}

	return(0); // end
    
	//==============================================================================
	//==============================================================================
	// Below is the original test code meant to use the NICAN device
	// It's been left below for reference
	//==============================================================================
	//==============================================================================

    /*==============================================================================
    * COMMAND PING
    *===============================================================================*/

	// Transmit.DataLength = 0;
	// Transmit.IsRemote = 0;
	// Transmit.ArbitrationId = CAN_COMMAND_PING;	
						
	// Status= ncWrite(TxHandle, sizeof(Transmit), &Transmit);			
	// if (Status < 0) 
	// {
	// 	PrintStat(Status, "ncWrite");
	// }

	// Status = ncRead(RxHandle, sizeof(ReceiveBuf), (void *)ReceiveBuf);
	// if (Status < 0)
	// {
	// 		PrintStat(Status, "ncReadMult");
	// }
	// rcvID = ReceiveBuf[0].ArbitrationId;
	// rcvData = ReceiveBuf[0].Data[0];
    // rcvDataLen = ReceiveBuf[0].DataLength;
	// if((rcvID != 0x05a6) && (rcvData != 0))
	// {
	// 	printf(" - COMMAND_PING failed!\n");
	// 	return 0;
	// }

    /*==============================================================================
    * COMMAND Download
    *===============================================================================*/
    Get_BinaryData( filename );
	ulAddress = 0x00020000;

	Transmit.DataLength = 8;
	Transmit.IsRemote = 0;
	Transmit.ArbitrationId = CAN_COMMAND_DOWNLOAD;	

	Transmit.Data[0] = (ulAddress >> 24) & 0xff;
	Transmit.Data[1] = (ulAddress >> 16) & 0Xff;
	Transmit.Data[2] = (ulAddress >> 8) & 0xff;
	Transmit.Data[3] = ulAddress & 0xff;
	Transmit.Data[4] = (image_size >> 24) & 0xff;
	Transmit.Data[5] = (image_size >> 16) & 0xff;
	Transmit.Data[6] = (image_size >> 8) & 0xff;
	Transmit.Data[7] = image_size & 0xff;

	Status= ncWrite(TxHandle, sizeof(Transmit), &Transmit);	
	if (Status < 0) 
	{
		PrintStat(Status, "ncWrite");
	}

	Status = ncRead(RxHandle, sizeof(ReceiveBuf), (void *)ReceiveBuf);
	if (Status < 0)
	{
			PrintStat(Status, "ncReadMult");
	}
	rcvID = ReceiveBuf[0].ArbitrationId;
	rcvData = ReceiveBuf[0].Data[0];
    rcvDataLen = ReceiveBuf[0].DataLength;
	if((rcvID != 0x05a6) && (rcvData != 0))
	{
		printf(" - COMMAND_DOWNLOAD failed!\n");
		return 0;
	}

    /*==============================================================================
    * COMMAND Sent Data
    *===============================================================================*/

	Get_BinaryData( filename );
	ulAddress = 0x00020000;

	Transmit.DataLength = 8;
	Transmit.IsRemote = 0;
	Transmit.ArbitrationId = CAN_COMMAND_SEND_DATA;	
	ulLength = image_size;

    //#3, Loop through the data in the code to be downloaded.
    for(ulOffset = 0; ulOffset < ulLength; ulOffset += 8)
    {
        // Build a send data command.
        for(ulIdx = 0; ulIdx < 8; ulIdx++)
        {
           	Transmit.Data[ulIdx] = image[ulOffset + ulIdx];
        }

        // See if the entire buffer contains valid data to be downloaded.
        if((ulOffset + ulIdx) > ulLength)
        {
			Transmit.DataLength = ulLength - ulOffset;
            // Send the send data command with the remainder of the data to be downloaded.
			Sleep(100);
			Status= ncWrite(TxHandle, sizeof(Transmit), &Transmit);			
			if (Status < 0) 
			{
				PrintStat(Status, "ncWrite");
			}

			//Get the ACK
			Sleep(10000);
			Status = ncRead(RxHandle, sizeof(ReceiveBuf), (void *)ReceiveBuf);
			if (Status < 0)
			{
					PrintStat(Status, "ncReadMult");
			}
			rcvID = ReceiveBuf[0].ArbitrationId;
			rcvData = ReceiveBuf[0].Data[0];
			rcvDataLen = ReceiveBuf[0].DataLength;
			if((rcvID != 0x07F4) && (rcvData != 0))
			{
				printf(" - COMMAND_SEND_DATA failed!\n");
				return 0;
			}
        }
        else
        {
			Transmit.DataLength = 8;
            // Send the send data command with the remainder of the data to be downloaded.
			Sleep(100);
			do{
				Status= ncWrite(TxHandle, sizeof(Transmit), &Transmit);			
			}while(Status < 0);
			if (Status < 0) 
			{
				PrintStat(Status, "ncWrite");
			}
			Sleep(100);
			//Get the ACK
			do{
				Status = ncRead(RxHandle, sizeof(ReceiveBuf), (void *)ReceiveBuf);
			} while (Status < 0);
				
			if (Status < 0)
			{
					PrintStat(Status, "ncReadMult");
			}
			rcvID = ReceiveBuf[0].ArbitrationId;
			rcvData = ReceiveBuf[0].Data[0];
			rcvDataLen = ReceiveBuf[0].DataLength;
			if((rcvID != 0x05a6) && (rcvData != 0))
			{
				printf(" - COMMAND_SEND_DATA failed!\n");
				return 0;
			}
        }
    }


    /*==============================================================================
    * COMMAND RUN
    *===============================================================================*/
	ulAddress = 0x00020000;

	// todo: create the Transmit object?
	Transmit.DataLength = 4;
	Transmit.IsRemote = 0;
	Transmit.ArbitrationId = CAN_COMMAND_RUN;	

	Transmit.Data[0] = (ulAddress >> 24) & 0xff;
	Transmit.Data[1] = (ulAddress >> 16) & 0Xff;
	Transmit.Data[2] = (ulAddress >> 8) & 0xff;
	Transmit.Data[3] = ulAddress & 0xff;

	Status= ncWrite(TxHandle, sizeof(Transmit), &Transmit);			
	if (Status < 0) 
	{
		PrintStat(Status, "ncWrite");
	}

	Status = ncRead(RxHandle, sizeof(ReceiveBuf), (void *)ReceiveBuf);
	if (Status < 0)
	{
			PrintStat(Status, "ncReadMult");
	}
	rcvID = ReceiveBuf[0].ArbitrationId;
	rcvData = ReceiveBuf[0].Data[0];
    rcvDataLen = ReceiveBuf[0].DataLength;
	if((rcvID != 0x05a6) && (rcvData != 0))
	{
		printf(" - COMMAND_DOWNLOAD failed!\n");
		return 0;
	}

	Sleep(100);

    /*==============================================================================
    * COMMAND RESET
    *===============================================================================*/
	Transmit.DataLength = 0;
	Transmit.IsRemote = 0;
	Transmit.ArbitrationId = CAN_COMMAND_RESET;	

	Status= ncWrite(TxHandle, sizeof(Transmit), &Transmit);			
	if (Status < 0) 
	{
		PrintStat(Status, "ncWrite");
	}

	Status = ncRead(RxHandle, sizeof(ReceiveBuf), (void *)ReceiveBuf);
	if (Status < 0)
	{
			PrintStat(Status, "ncReadMult");
	}
	rcvID = ReceiveBuf[0].ArbitrationId;
	rcvData = ReceiveBuf[0].Data[0];
    rcvDataLen = ReceiveBuf[0].DataLength;
	if((rcvID != 0x05a6) && (rcvData != 0))
	{
		printf(" - COMMAND_DOWNLOAD failed!\n");
		return 0;
	}

	Sleep(100);

    /* Close the Network Interface Object */
	Status = ncCloseObject(TxHandle);              
	if (Status < 0)
	{
		PrintStat(Status, "ncCloseObject");
	}
	Status = ncCloseObject(RxHandle);              
	if (Status < 0)
	{
		PrintStat(Status, "ncCloseObject");
	}

    printf("Application Code has been downloaded to flash correctly!!\n");
	
	return 0;    
    
}


/* USER CODE BEGIN (4) */
/* USER CODE END */
