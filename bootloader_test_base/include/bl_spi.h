//*****************************************************************************
//
// bl_spi.h - Definitions for the SPI transport functions.
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
//
//*****************************************************************************

#ifndef __BL_SPI_H__
#define __BL_SPI_H__


#include "sys_common.h"
#include "hw_spi.h"

//*****************************************************************************
//
// SPI Transport APIs
//
//*****************************************************************************
extern void SPISend(spiBASE_t *node, uint8_t const *pucData, uint8_t ulSize);
extern void SPIReceive(spiBASE_t *node, uint8_t *pucData, uint8_t ulSize);
void UpdaterSPI(spiBASE_t *node);
void ConfigureSPIDevice(spiBASE_t *node);

//*****************************************************************************
//
// Define the transport functions if the SPI port is being used.
//
//*****************************************************************************
#ifdef SPI_ENABLE_UPDATE
#define SendData                SPISend
#define ReceiveData             SPIReceive
#endif

#endif // __BL_SPI_H__
