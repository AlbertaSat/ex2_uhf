//*****************************************************************************
//
// bl_flash.c  : Flash programming functions used by the boot loader.
// Author      : QJ Wang. qjwang@ti.com
// Date        : 9-19-2012
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
#include "bl_flash.h"
#include "F021.h"
#include "sci_common.h"
#include "flash_defines.h"


//*****************************************************************************
//
//! Returns the size of the ist sector size of the flash in bytes.
//!
//*****************************************************************************
uint32_t
BLInternalFlashFirstSectorSizeGet(void)
{
	uint32_t firstSectorSize;
	firstSectorSize = (uint32_t)(flash_sector[0].start) + flash_sector[0].length;
    return (firstSectorSize);
}
//*****************************************************************************
//
//! Returns the size of the internal flash in bytes.
//!
//! This function returns the total number of bytes of internal flash in the
//! current part.  No adjustment is made for any sections reserved via
//! options defined in bl_config.h.
//!
//! \return Returns the total number of bytes of internal flash.
//
//*****************************************************************************
uint32_t
BLInternalFlashSizeGet(void)
{
	uint32_t flashSize;
	flashSize = (uint32_t)flash_sector[NUMBEROFSECTORS-1].start + flash_sector[NUMBEROFSECTORS-1].length;
    return (flashSize);
}

//*****************************************************************************
//
//! Checks whether a given start address is valid for a download.
//!
//! This function checks to determine whether the given address is a valid
//! download image start address given the options defined in bl_config.h.
//!
//! \return Returns non-zero if the address is valid or 0 otherwise.
//
//*****************************************************************************
uint32_t
BLInternalFlashStartAddrCheck(uint32_t ulAddr, uint32_t ulImgSize)
{
    uint32_t count=0, i;

	uint32_t ulWholeFlashSize;

    //
    // Determine the size of the flash available on the part in use.
    //
    ulWholeFlashSize = (uint32_t)flash_sector[NUMBEROFSECTORS-1].start + flash_sector[NUMBEROFSECTORS-1].length;  /* 3MB */

	/* The start address must be at the begining of the sector */
    for (i = 0; i < NUMBEROFSECTORS-1; i++){
		if (ulAddr == (uint32_t)(flash_sector[i].start))
		{
			count++;
		}
	}
    if (count == 0){
#ifdef DEBUG_MSG_L3
    UART_putString(UART, "\r Application Address should be aligned with flash sector start address!\r");
#endif
    	return(0);
    }

    //
    // Is the address we were passed a valid start address?  We allow:
    //
    // 1. Address 0 if configured to update the boot loader.
    // 2. The start of the reserved block if parameter space is reserved (to
    //    allow a download of the parameter block contents).
    // 3. The application start address specified in bl_config.h.
    //
    // The function fails if the address is not one of these, if the image
    // size is larger than the available space or if the address is not word
    // aligned.
    //
    if((
#ifdef ENABLE_BL_UPDATE
                       (ulAddr != 0) &&
#endif
                        (ulAddr != APP_START_ADDRESS)) ||
                       ((ulAddr + ulImgSize) > ulWholeFlashSize) ||
                       ((ulAddr & 3) != 0))
    {
#ifdef DEBUG_MSG_L3
    UART_putString(UART, "\r Flash address and size check failed!\r");
#endif
    	return(0);
    }
    else
    {
#ifdef DEBUG_MSG_L3
    UART_putString(UART, "\r Flash address and size check passed!\r");
#endif
        return(1);
    }
}


uint32_t Fapi_BlockErase( uint32_t Bank, uint32_t Flash_Start_Address, uint32_t Size_In_Bytes)
{
	uint32_t status;
	uint32_t Freq_In_MHz;
	uint8_t  i, j, bk, ucStartBank, ucStartSector, ucEndBank;
	uint32_t *eraseStartAddr0;
	uint32_t *eraseStartAddr;
    int  remaining;
    uint32_t Flash_End_Address;

	ucStartBank = 0, i=0, ucStartSector = 0;
	ucEndBank = 0;
	eraseStartAddr0 = (uint32_t *)Flash_Start_Address;
	eraseStartAddr = (uint32_t *)Flash_Start_Address;
	Flash_End_Address = Flash_Start_Address + Size_In_Bytes;

	Freq_In_MHz = SYS_CLK_FREQ;

	for (i = 0; i < NUMBEROFSECTORS-1; i++){
		if (Flash_Start_Address == (uint32_t)(flash_sector[i].start))
		{
			ucStartBank     = flash_sector[i].bankNumber;
		    ucStartSector   = flash_sector[i].sectorNumber;
		    eraseStartAddr0 = flash_sector[i].start;
		    eraseStartAddr  = flash_sector[i].start;
		    break;
		}
	}

	for (i = ucStartSector; i < NUMBEROFSECTORS-1; i++){
		if (Flash_End_Address <= ((uint32_t)(flash_sector[i].start) + flash_sector[i].length))
		{
			ucEndBank   = flash_sector[i].bankNumber;
		    break;
		}
	}

	//F021_CPU0_REGISTER_ADDRESS is defined as 0xfff87000 in FMC.h
	//Fapi_initializeAPI((Fapi_FmcRegistersType *)F021_CPU0_REGISTER_ADDRESS, Freq_In_MHz); /*used for API Rev1.5*/
	Fapi_initializeFlashBanks(Freq_In_MHz); /* used for API Rev2.01 */

    for (bk = ucStartBank; bk < ucEndBank+1; bk++){

    	Fapi_setActiveFlashBank((Fapi_FlashBankType)bk);

    	if (bk == 0){
    	    j = ucStartSector;
    	    remaining = Size_In_Bytes;
    	}else{
     	    j = 0;
    	}

        Fapi_enableMainBankSectors(0xFF);        /* used for API 2.01*/
        while( FAPI_CHECK_FSM_READY_BUSY == Fapi_Status_FsmBusy );

    	do{
        	Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, eraseStartAddr);
        	while( FAPI_CHECK_FSM_READY_BUSY == Fapi_Status_FsmBusy );
        	while(FAPI_GET_FSM_STATUS != Fapi_Status_Success);

            remaining -= flash_sector[j++].length;
    		eraseStartAddr = flash_sector[j].start;

        }while((remaining > 0) && ( j < flash_bank[bk].numOfSectors));
    }

	status =  Flash_Erase_Check((uint32_t)eraseStartAddr0, Size_In_Bytes);

	return (status);
}

//Bank here is not used. We calculate the bank in the function based on the Flash-Start-addr
uint32_t Fapi_BlockProgram( uint32_t Bank, uint32_t Flash_Start_Address, uint32_t Data_Start_Address, uint32_t Size_In_Bytes)
{
	register uint32_t src = Data_Start_Address;
	register uint32_t dst = Flash_Start_Address;
	register uint32_t bytes_remain = Size_In_Bytes;
	uint8_t  i, ucBank;
	uint32_t status;
	uint32_t bytes;
	uint32_t Freq_In_MHz;

	Freq_In_MHz = SYS_CLK_FREQ;

	if (Size_In_Bytes < 16)
		bytes = Size_In_Bytes;
	else
		bytes = 16;

	for (i = 0; i < NUMBEROFSECTORS-1; i++){
		if (Flash_Start_Address < (uint32_t)(flash_sector[i+1].start))
		{
			ucBank     = flash_sector[i].bankNumber;
		    break;
		}
	}

	if(( Flash_Start_Address == APP_START_ADDRESS ) || ( ucBank == 1 )){
		Fapi_initializeFlashBanks(Freq_In_MHz); /* used for API Rev2.01 */

		Fapi_setActiveFlashBank((Fapi_FlashBankType)ucBank);

		Fapi_enableMainBankSectors(0xFF);        /* used for API 2.01*/

		while( FAPI_CHECK_FSM_READY_BUSY == Fapi_Status_FsmBusy );
		while( FAPI_GET_FSM_STATUS!= Fapi_Status_Success );
	}

    while( bytes_remain > 0)
	{
		Fapi_issueProgrammingCommand((uint32_t *)dst,
									 (uint8_t *)src,
									 (uint32_t) bytes,
									 0,
									 0,
									 Fapi_AutoEccGeneration);

 		while( FAPI_CHECK_FSM_READY_BUSY == Fapi_Status_FsmBusy );
        while(FAPI_GET_FSM_STATUS!= Fapi_Status_Success);

		src += bytes;   //Size_In_Bytes;
		dst += bytes; //Size_In_Bytes;
        bytes_remain -= bytes; //Size_In_Bytes;
        if ( bytes_remain < 16){
           bytes = bytes_remain;
        }
    }

#ifdef DEBUG_MSG_L3
        UART_putString(UART, "\r Programmed the flash: ");
        UART_send32BitData(UART, Size_In_Bytes);
        UART_putString(UART, "   Bytes. \r ");
#endif
	status =  Flash_Program_Check(Flash_Start_Address, Data_Start_Address, Size_In_Bytes);

	return (status);
}


uint32_t Fapi_UpdateStatusProgram( uint32_t Bank, uint32_t Flash_Start_Address, uint32_t Data_Start_Address, uint32_t Size_In_Bytes)
{
	register uint32_t src = Data_Start_Address;
	register uint32_t dst = Flash_Start_Address;
	uint32_t status;
	int bytes;
	uint32_t Freq_In_MHz;

	Freq_In_MHz = SYS_CLK_FREQ;

	if (Size_In_Bytes < 16)
		bytes = Size_In_Bytes;
	else
		bytes = 16;

	Fapi_initializeAPI((Fapi_FmcRegistersType *)F021_CPU0_REGISTER_ADDRESS, Freq_In_MHz);

	Fapi_setActiveFlashBank((Fapi_FlashBankType)Bank);

#ifdef DEBUG_MSG_L2
    	UART_putString(UART, "\r Programming The Update Status.... \r ");
#endif
		Fapi_issueProgrammingCommand((uint32_t *)dst,
									 (uint8_t *)src,
									 (uint32_t) bytes,   //8,
									 0,
									 0,
									 Fapi_AutoEccGeneration);

 		while( Fapi_checkFsmForReady() == Fapi_Status_FsmBusy );

	status =  Flash_Program_Check(Flash_Start_Address, Data_Start_Address, Size_In_Bytes);

	return (status);
}



uint32_t Flash_Program_Check(uint32_t Program_Start_Address, uint32_t Source_Start_Address, uint32_t No_Of_Bytes)
{
	uint32_t error=0;
	register uint32_t *src1 = (uint32_t *) Source_Start_Address;
	register uint32_t *dst1 = (uint32_t *) Program_Start_Address;
	register uint32_t bytes = No_Of_Bytes;

	while(bytes > 0)
	{	
		if(*dst1++ != *src1++)
			error = 1;

		bytes -= 0x4;
	}
#ifdef DEBUG_MSG_L2
        UART_putString(UART, "\r Program Flash Errors:   ");
        UART_send32BitData(sciREG1, error);
        UART_putString(UART, "\r ");
#endif
	return(error);

}	


uint32_t Flash_Erase_Check(uint32_t Start_Address, uint32_t Bytes)
{
	uint32_t error=0;
	register uint32_t *dst1 = (uint32_t *) Start_Address;
	register uint32_t bytes = Bytes;

	while(bytes > 0)
	{	
		if(*dst1++ != 0xFFFFFFFF){
			error = 2;
		}
		bytes -= 0x4;
	}
#ifdef DEBUG_MSG_L2
        UART_putString(UART, "\r Erase Flash Errors:   ");
        UART_send32BitData(sciREG1, error);
        UART_putString(UART, "\r ");
#endif
	return(error);
}



uint32_t Fapi_BlockRead( uint32_t Bank, uint32_t Flash_Start_Address, uint32_t Data_Start_Address, uint32_t Size_In_Bytes)
{
	register uint32_t src = Data_Start_Address;
	register uint32_t dst = Flash_Start_Address;
	register uint32_t bytes_remain = Size_In_Bytes;
	uint32_t status =0;
	int bytes;
	uint32_t Freq_In_MHz;

	Freq_In_MHz = SYS_CLK_FREQ;

	if (Size_In_Bytes < 16)
		bytes = Size_In_Bytes;
	else
		bytes = 16;
	Fapi_initializeAPI((Fapi_FmcRegistersType *)F021_CPU0_REGISTER_ADDRESS, Freq_In_MHz);


 	while( bytes_remain > 0)
	{
		Fapi_doMarginReadByByte((uint8_t *)src,
								(uint8_t *)dst,
								(uint32_t) bytes,                //16
								Fapi_NormalRead);


		src += bytes;   //Size_In_Bytes;     //0x10;
		dst += bytes; //Size_In_Bytes;     //0x10;
        bytes_remain -= bytes; //Size_In_Bytes;    //0x10;
    }

	return (status);
}
