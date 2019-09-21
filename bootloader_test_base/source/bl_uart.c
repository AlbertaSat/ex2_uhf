//*****************************************************************************
//
// bl_uart.c : Functions to transfer data via the UART port.
// Author    : QJ Wang. qjwang@ti.com
// Date      : 9-19-2012
//
// Copyright (c) 2006-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
//*****************************************************************************

#include "hw_sci.h"
#include "bl_config.h"
#include "bl_uart.h"
#include "sci_common.h"
#include "bl_ymodem.h"
#include "sys_common.h"
#include "bl_led_demo.h"

uint32_t JumpAddress;
void get_software_Version(void);
void get_hardware_Info(void);

extern uint32_t g_pulUpdateSuccess[8];
extern uint32_t g_ulUpdateStatusAddr;
extern uint32_t g_ulUpdateBufferSize;  //32 bytes or 8 32-bit words
extern char file_name[FILE_NAME_LENGTH];
char tab_1024[1024] =
  {
    0
  };

//*****************************************************************************
//
//  bl_uart_api
//
//
//*****************************************************************************
#if defined(UART_ENABLE_UPDATE)

/**
  * @brief  Download a file via serial port
  * @param  None
  * @retval None
  */
void UART_Download()
{
  char Number[10] = "          ";
  int Size = 0;

  UART_putString(UART, "\r Waiting for the file to be sent ... ");
  UART_putString(UART, "\r Use Transfer->Send File and Ymodem Protocol from HyperTerminal \r");

  Size = Ymodem_Receive(UART, &tab_1024[0]);
  if (Size > 0)
  {
    UART_putString(UART, "\n\r The application image has been programmed successfully!\r------------------------\r Name: ");
    UART_putString(UART, file_name);
    Int2Str(Number, Size);
    UART_putString(UART, "\n\r Size: ");
    UART_putString(UART, Number);
    UART_putString(UART, " Bytes\r");
    UART_putString(UART, "---------------------------\n");
  }
  else if (Size == -1)
  {
    UART_putString(UART, "\n\rThe image size is higher than the allowed space memory!\n\r");
  }
  else if (Size == -2)
  {
    UART_putString(UART, "\n\rThe flash programming failed!\n\r");
  }
  else if (Size == -3)
  {
    UART_putString(UART, "\r\n\nAborted by user.\n\r");
  }
  else
  {
    UART_putString(UART, "\n\rFailed to receive the file!\n\r");
  }
}



/**
  * @brief  Upload a file via serial port.
  * @param  None
  * @retval None
  */
void UART_Upload(void)
{
	  uint32_t status = 0;
	  uint32_t *updateInfo;
	  uint32_t imageSize;
	  uint32_t imageAddress;

	  updateInfo = (uint32_t *)g_ulUpdateStatusAddr;

	  imageAddress = updateInfo[1];
	  imageSize = updateInfo[2];


	  UART_putString(UART, "\n\n\rSelect Receive File and Ymodel protocol in the drop-down menu... (press any key to abort)\n\r");

	  if (UART_getKey(UART) == CRC)
	  {
			/* Transmit the flash image through ymodem protocol */
			status = Ymodem_Transmit(UART, (uint8_t*)imageAddress, (char*)"UploadedApplicationImage.bin", imageSize);

			if (status != 0)
			{
				UART_putString(UART, "\n\rError occured while transmitting\n\r");
			}
			else
			{
				UART_putString(UART, "\n\rApplication image has been transmitted successfully \n\r");
			}
	  }
	  else
	  {
		  UART_putString(UART, "\r\n\nAborted by user.\n\r");
	  }

}

//*****************************************************************************
//
//! This function performs the update on the selected port.
//!
//! This function is called directly by the boot loader or it is called as a
//! result of an update request from the application.
//!
//! \return Never returns.
//
//*****************************************************************************
void UpdaterUART(void)
{
	  char key = 0;

	  while (1)
	  {
		    LITE_TOPRIGHT_LED;
			UART_putString(UART, "\r================== Main Menu ==========================\r\n");
			UART_putString(UART, "  1. Download Application Image To the Internal Flash \r\n");
			UART_putString(UART, "  2. Upload The Application Image From the Internal Flash \r\n");
			UART_putString(UART, "  3. Execute The Application Code \r\n");
			UART_putString(UART, "  4. Get Bootloader Version \r\n");
			UART_putString(UART, "  5. Get Device Information \r\n");

			UART_putString(UART,   "=======================================================\r\n");

			key = UART_getKey(UART);

			if (key == 0x31)
			{
			   /* Download user application in the Flash */
			   UART_Download();
			}
		    else if (key == 0x32)
		    {
		      /* Upload user application from the Flash */
		      UART_Upload();
		    }
			else if (key == 0x33)
			{
		        JumpAddress = (uint32_t)APP_START_ADDRESS;
		        ((void (*)(void))JumpAddress)();
			}
			else if (key == 0x34)
			{
				get_software_Version();
			}
			else if (key == 0x35)
			{
				get_hardware_Info();
			}
			else
			{
				UART_putString(UART, "Invalid Number !! \r");
			}
	  }
}

#endif


