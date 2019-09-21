//*****************************************************************************
//
// bl_config.h - The configurable parameters of the boot loader.
//
// Copyright (c) 2008-2011 Texas Instruments Incorporated.  All rights reserved.
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
//
//*****************************************************************************

#ifndef __BL_CONFIG_H__
#define __BL_CONFIG_H__

//*****************************************************************************
//
// The following defines are used to configure the operation of the boot
// loader.  For each define, its interactions with other defines are described.
// First is the dependencies (in other words, the defines that must also be
// defined if it is defined), next are the exclusives (in other words, the
// defines that can not be defined if it is defined), and finally are the
// requirements (in other words, the defines that must be defined if it is
// defined).
//
// The following defines must be defined in order for the boot loader to
// operate:
//
//     One of CAN_ENABLE_UPDATE, SPI_ENABLE_UPDATE, SCI_ENABLE_UPDATE
//     APP_START_ADDRESS
//
//*****************************************************************************

//*****************************************************************************
//
// The frequency (MHz) of the crystal used to clock the microcontroller.
//
// This defines the crystal frequency used by the microcontroller running the
// boot loader.
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
#define CRYSTAL_FREQ            16             //MHz

#define SYS_CLK_FREQ            160           //MHz

//*****************************************************************************
//
// The starting address of the application.  This must be a multiple of 32K(sector size)
// bytes (making it aligned to a page boundary), and can not be 0 (the first sector is 
// boot loader). 
//
// The flash image of the boot loader must not be larger than this value.
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
#define APP_START_ADDRESS       0x00020000

//*****************************************************************************
//
// The number of words in the data buffer used for receiving packets.  This
// value must be at least 3. The maximum usable value is 65 (larger values will result in
// unused space in the buffer).
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
#define BUFFER_SIZE             64

//*****************************************************************************
//
// Enables updates to the boot loader.  Updating the boot loader is an unsafe
// operation since it is not fully fault tolerant (losing power to the device
// part way though could result in the boot loader no longer being present in
// flash).
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
//#define ENABLE_BL_UPDATE

//*****************************************************************************
//
// Enables the pin-based forced update check.  When enabled, the boot loader
// will go into update mode instead of calling the application if a pin is read
// at a particular polarity, forcing an update operation.  In either case, the
// application is still able to return control to the boot loader in order to
// start an update. 
//
// Depends on: None
// Exclusive of: None
// Requires: FORCED_UPDATE_PERIPH, FORCED_UPDATE_PORT, FORCED_UPDATE_PIN,
//           FORCED_UPDATE_POLARITY
//
//*****************************************************************************
#define ENABLE_UPDATE_CHECK


//*****************************************************************************
//
// The GPIO port to check for a forced update.  This will be one of the
// GPIO_PORTx_BASE values, where "x" is replaced with the port name (A or B).
// Depends on: ENABLE_UPDATE_CHECK
// Exclusive of: None
// Requries: None
//
//*****************************************************************************
#define FORCED_UPDATE_PORT      GPIO_PORTA_BASE

//*****************************************************************************
//
// The pin to check for a forced update.  This is a value between 0 and 7.
//
// Depends on: ENABLE_UPDATE_CHECK
// Exclusive of: None
// Requries: None
//
//*****************************************************************************
#define FORCED_UPDATE_PIN       7

//*****************************************************************************
//
// Selects the baud rate to be used for the UART.
//
// Depends on: UART_ENABLE_UPDATE
// Requires: None
// UART Baud Rates can be: 9600, 19200, 38400, 57600, 115200
//*****************************************************************************
#define UART_BAUDRATE     118200
#define UART              sciREG1   /* Use UART port 1 for UART boot */

//*****************************************************************************
// Selects the UART as the port for communicating with the boot loader.
// Exclusive of: CAN_ENABLE_UPDATE, SPI_ENABLE_UPDATE,
// Requires: UART_FIXED_BAUDRATE, BUFFER_SIZE
//*****************************************************************************
//#define UART_ENABLE_UPDATE

//*****************************************************************************
// Selects the SPI port as the port for communicating with the boot loader.
// Exclusive of: CAN_ENABLE_UPDATE, UART_ENABLE_UPDATE
// Requires: BUFFER_SIZE
//*****************************************************************************
//#define SPI_ENABLE_UPDATE
#define SPI_PORT              spiREG2             /*use SPI2 for SPI boot*/

//*****************************************************************************
// Selects an update via the CAN port.
// Exclusive of: SPI_ENABLE_UPDATE,UART_ENABLE_UPDATE
// Requires: CAN_BOOT_PORT, CAN_BIT_RATE
// CAN_BIT_RATE: 125K, 250K, 500K, 750K, 1000K. The default is 125K
//*****************************************************************************
#define CAN_ENABLE_UPDATE
#define CAN_PORT                 canREG1
#define CAN_BIT_RATE             125000

#define   TMS570LS12

//#define   DEBUG_MSG_L3


#endif // __BL_CONFIG_H__
