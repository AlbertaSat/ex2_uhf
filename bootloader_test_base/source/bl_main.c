//*****************************************************************************
//
// bl_main.c      : The file holds the main control loop of the boot loader.
// Author         : QJ Wang. qjwang@ti.com
// Date           : 9-19-2012
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


#include "bl_config.h"
#include "bl_spi.h"
#include "bl_uart.h"
#include "system.h"
#include "bl_check.h"
#include "hw_spi.h"
#include "hw_can.h"
#include "sci_common.h"
#include "bl_led_demo.h"


//*****************************************************************************
//
// bl_main_api
//
//*****************************************************************************
#if defined (SPI_ENABLE_UPDATE) || defined(UART_ENABLE_UPDATE) || defined(CAN_ENABLE_UPDATE)


//*****************************************************************************
//
// This holds the current remaining size in bytes to be downloaded.
//
//*****************************************************************************
uint32_t g_ulTransferSize;

//*****************************************************************************
//
// This holds the current address that is being written to during a download
// command.
//
//*****************************************************************************
uint32_t g_ulTransferAddress;

//*****************************************************************************
//
// This is the data buffer used during transfers to the boot loader.
//
//*****************************************************************************
uint32_t g_pulDataBuffer[BUFFER_SIZE];

//*****************************************************************************
//
// This is the data buffer used for update status.
//
//*****************************************************************************

uint32_t g_pulUpdateSuccess[] = {0x5A5A5A5A, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
uint32_t g_ulUpdateStatusAddr = 0x07FC0;
uint32_t g_ulVectorTable = 0x8000000;
uint32_t g_ulUpdateBufferSize = 32;  //32 bytes or 8 32-bit words

#define E_PASS     		0
#define E_FAIL     		0x1U
#define E_TIMEOUT  		0x2U

//*****************************************************************************
//
// This is an specially aligned buffer pointer to g_pulDataBuffer to make
// copying to the buffer simpler.  It must be offset to end on an address that
// ends with 3.
//
//*****************************************************************************
uint8_t *g_pucDataBuffer;

//*****************************************************************************
//
// This holds the current address that is being written to during a download
// command.
//
//*****************************************************************************
//extern uint32_t g_ulTransferAddress;

void delay(void) {
	  static volatile uint32_t delayval;
	  delayval = 10000;   // 100000 are about 10ms
	  while(delayval--);
}

void main(void)
{
    uint32_t fnRetValue;

    /** - Initialize SCI Routines to receive Command and transmit data */
	sciInit();

#if defined (SPI_ENABLE_UPDATE)
	UART_putString(UART, "\r Hercules MCU SPI BootLoader ");
#endif
#if defined (CAN_ENABLE_UPDATE)
	UART_putString(UART, "\r Hercules MCU CAN BootLoader ");
#endif
#if defined (UART_ENABLE_UPDATE)
	UART_putString(UART, "\r Hercules MCU UART BootLoader ");
#endif

	UART_putString(UART, "\r TI Safety MCU Application Team, qjwang@ti.com \r\r");

//    
//  See if an update should be performed.
//  
    fnRetValue = CheckForceUpdate();
    
    if ( !fnRetValue )
    {
#ifdef DEBUG_MSG_L3
    	UART_putString(UART, "\r Jump to application...  ");
#endif
        g_ulTransferAddress = (uint32_t)APP_START_ADDRESS;
        ((void (*)(void))g_ulTransferAddress)();
    }
 
//
//  Configure the microcontroller.
//
//EnterBootLoader
#ifdef CAN_ENABLE_UPDATE
    ConfigureCANDevice(CAN_PORT);
#endif
#ifdef SPI_ENABLE_UPDATE
    ConfigureSPIDevice(SPI_PORT);
#endif
#ifdef UART_ENABLE_UPDATE
    ConfigureUartDevice();
#endif
 

// 
// Branch to the update handler. Use can1
// 
#ifdef CAN_ENABLE_UPDATE
    UpdaterCAN(CAN_PORT);
#endif

#ifdef UART_ENABLE_UPDATE
    UpdaterUART();
#endif
#ifdef SPI_ENABLE_UPDATE
    UpdaterSPI(SPI_PORT);
#endif
    
}

//*****************************************************************************
//
//! Configures the microcontroller.
//!
//! This function configures the peripherals and GPIOs of the microcontroller,
//! preparing it for use by the boot loader.  The interface that has been
//! selected as the update port will be configured, and auto-baud will be
//! performed if required.
//!
//! \return None.
//
//*****************************************************************************
void ConfigureSPIDevice(spiBASE_t *node)
{

#ifdef SPI_ENABLE_UPDATE
    //
    // Initialize the SPI1 as slave mode
    // Enable the SPI interface in slave mode.
    // Set the SPI protocol to Motorola with default clock high and data valid on the rising edge.
    //
    spiInit();
    
#endif
}

//*****************************************************************************
void ConfigureUartDevice(void)
{
    //
    // Enable the the clocks to the UART and GPIO modules.
    //
   sciInit();
}

#endif
