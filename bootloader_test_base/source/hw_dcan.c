//*****************************************************************************
//
// hw_dcan.c
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

/* Include Files */

#include "hw_can.h"
#include "hw_sci.h"
#include "bl_config.h"
#include "sci_common.h"

/* USER CODE BEGIN (1) */
/* USER CODE END */

#if defined(CAN_ENABLE_UPDATE)

/* Global and Static Variables */

#if defined (_TMS470_LITTLE) || defined(__little_endian__)
    static const unsigned s_canByteOrder1[] = {3U, 2U, 1U, 0U, 7U, 6U, 5U, 4U};
#else
    static const unsigned s_canByteOrder1[] = {3U, 2U, 1U, 0U, 7U, 6U, 5U, 4U};
#endif

/* USER CODE BEGIN (2) */
/* USER CODE END */
void canExtLoopTest(void)
{
	uint32_t  i;
	uint32_t  Test_Status; // 0 --> Pass, 1 - Fail
	uint8_t tx_data[8] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
	uint8_t rx_data[12] = {0,0,0,0,0,0,0,0,0,0,0,0}; /*1st 4 bytes will be ID*/
	
	canInit();

    UART_putString(sciREG1, "\r DCAN LoopTest ");
	
	canTransmit(canREG1, canMESSAGE_BOX1, tx_data);
	while(!canIsRxMessageArrived(canREG2, canMESSAGE_BOX1));
	canGetData(canREG2, canMESSAGE_BOX1, rx_data);
	
	Test_Status = 0;
	
	for(i=0;i<8;i++)
	{
        UART_putChar(sciREG1, rx_data[i]);
		if(tx_data[i] != rx_data[i+4])
		{
			//Test Failed 
			Test_Status = 1;
		}
	}
	
	if(Test_Status == 0) 
		UART_send32BitData(sciREG1, 0x0);
	else
   		UART_send32BitData(sciREG1, 0x01);
}


/** @fn void canInit(void)
*   @brief Initializes CAN Driver
*
*   This function initializes the CAN driver.
*
*/

/* USER CODE BEGIN (3) */
/* USER CODE END */

void canInit(void)
{
/* USER CODE BEGIN (4) */
/* USER CODE END */
    /** @b Initialize @b DCAN1: */

    /** - Setup control register
    *     - Disable automatic wakeup on bus activity
    *     - Local power down mode disabled
    *     - Disable DMA request lines
    *     - Enable global Interrupt Line 0 and 1
    *     - Disable debug mode
    *     - Release from software reset
    *     - Disable parity and ECC
    *     - Enable/Disable auto bus on timer
    *     - Setup message completion before entering debug state
    *     - Setup normal operation mode
    *     - Request write access to the configuration registers
    *     - Setup automatic retransmission of messages
    *     - Disable error interrups
    *     - Disable status interrupts
    *     - Enter initialization mode
    */
    canREG1->CTL = 0x00000000U | 0x00000000U | 0x00021443U;  /* both IE0, IE1 are 1*/

    /** - Clear all pending error flags and reset current status */
    canREG1->ES = 0x0000031FU;

    /** - Assign interrupt level for messages */
    canREG1->INTMUXx[0U] = 0x00000011U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U;

    canREG1->INTMUXx[1U] = 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U;

    /** - Setup auto bus on timer pewriod */
    canREG1->ABOTR = 0U;

    /** - Initialize message 1 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x00000000U) << 18U);
    canREG1->IF1ARB  = 0x80000000U | 0x20000000U | 0x00000000U | ((1U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000C00U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 1;

    /** - Initialize message 2 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((0x1F020000 & 0x1FFFFFFF));
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 2;

    /** - Initialize message 3 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((3U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 3;

    /** - Initialize message 4 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((4U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 4;

    /** - Initialize message 5 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((5U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 5;

    /** - Initialize message 6 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((6U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 6;

    /** - Initialize message 7 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((7U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 7;

    /** - Initialize message 8 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((8U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 8;

    /** - Initialize message 9 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((9U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 9;

    /** - Initialize message 10 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((10U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 10;

    /** - Initialize message 11 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((11U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 11;

    /** - Initialize message 12 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((12U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 12;

    /** - Initialize message 13 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((13U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 13;

    /** - Initialize message 14 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((14U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 14;

    /** - Initialize message 15 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((15U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 15;

    /** - Initialize message 16 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((16U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 16;

    /** - Initialize message 17 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((17U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 17;

    /** - Initialize message 18 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((18U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 18;

    /** - Initialize message 19 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((19U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 19;

    /** - Initialize message 20 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((20U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 20;

    /** - Initialize message 21 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((21U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 21;

    /** - Initialize message 22 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((22U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 22;

    /** - Initialize message 23 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((23U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 23;

    /** - Initialize message 24 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((24U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 24;

    /** - Initialize message 25 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((25U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 25;

    /** - Initialize message 26 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((26U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 26;

    /** - Initialize message 27 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((27U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 27;

    /** - Initialize message 28 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((28U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 28;

    /** - Initialize message 29 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((29U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 29;

    /** - Initialize message 30 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((30U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 30;

    /** - Initialize message 31 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((31U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 31;

    /** - Initialize message 32 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((32U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 32;

    /** - Initialize message 33 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((33U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 33;

    /** - Initialize message 34 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((34U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 34;

    /** - Initialize message 35 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((35U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 35;

    /** - Initialize message 36 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((36U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 36;

    /** - Initialize message 37 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((37U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 37;

    /** - Initialize message 38 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((38U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 38;

    /** - Initialize message 39 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((39U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 39;

    /** - Initialize message 40 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((40U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 40;

    /** - Initialize message 41 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((41U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 41;

    /** - Initialize message 42 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((42U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 42;

    /** - Initialize message 43 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((43U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 43;

    /** - Initialize message 44 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((44U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 44;

    /** - Initialize message 45 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((45U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 45;

    /** - Initialize message 46 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((46U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 46;

    /** - Initialize message 47 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((47U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 47;

    /** - Initialize message 48 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((48U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 48;

    /** - Initialize message 49 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((49U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 49;

    /** - Initialize message 50 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((50U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 50;

    /** - Initialize message 51 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((51U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 51;

    /** - Initialize message 52 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((52U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 52;

    /** - Initialize message 53 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((53U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 53;

    /** - Initialize message 54 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((54U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 54;

    /** - Initialize message 55 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((55U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 55;

    /** - Initialize message 56 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((56U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 56;

    /** - Initialize message 57 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((57U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 57;

    /** - Initialize message 58 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((58U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 58;

    /** - Initialize message 59 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((59U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 59;

    /** - Initialize message 60 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((60U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 60;

    /** - Initialize message 61 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((61U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 61;

    /** - Initialize message 62 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((62U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 62;

    /** - Initialize message 63 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG1->IF1STAT & 0x80);

    canREG1->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF1ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((63U & 0x000007FFU) << 18U);
    canREG1->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF1CMD  = 0xF8;
    canREG1->IF1NO   = 63;

    /** - Initialize message 64 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG1->IF2STAT & 0x80);

    canREG1->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x000007FFU) << 18U);
    canREG1->IF2ARB  = 0x00000000U | 0x00000000U | 0x20000000U | ((64U & 0x000007FFU) << 18U);
    canREG1->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG1->IF2CMD  = 0xF8;
    canREG1->IF2NO   = 64;

    /** - Setup IF1 for data transmission 
    *     - Wait until IF1 is ready for use 
    *     - Set IF1 control byte
    */
    while (canREG1->IF1STAT & 0x80);
    canREG1->IF1CMD  = 0x87;

    /** - Setup IF2 for reading data
    *     - Wait until IF1 is ready for use 
    *     - Set IF1 control byte
    */
    while (canREG1->IF2STAT & 0x80);
    canREG1->IF2CMD = 0x17;

    /** - Setup bit timing 
    *     - Setup baud rate prescaler extension
    *     - Setup TSeg2
    *     - Setup TSeg1
    *     - Setup sample jump width
    *     - Setup baud rate prescaler
    *     
    */

    canREG1->BTR = (0U << 16U) |
                   ((3U - 1U) << 12U) |
                   (((1U + 3U) - 1U) << 8U) |
                   ((3U - 1U) << 6U) |
                   9U;

    //canREG1->BTR = 0x2B41;  /*500k*/

    /** - Setup TX pin to functional output */
    canREG1->TIOC = 0x0000004CU;

    /** - Setup RX pin to functional input */
    canREG1->RIOC = 0x00000048U;

    /** - Leave configuration and initialization mode  */
    canREG1->CTL &= ~0x00000041U;

    /** @b Initialize @b DCAN2: */

    /** - Setup control register
    *     - Disable automatic wakeup on bus activity
    *     - Local power down mode disabled
    *     - Disable DMA request lines
    *     - Enable global Interrupt Line 0 and 1
    *     - Disable debug mode
    *     - Release from software reset
    *     - Disable parity and ECC
    *     - Enable/Disable auto bus on timer
    *     - Setup message completion before entering debug state
    *     - Setup normal operation mode
    *     - Request write access to the configuration registers
    *     - Setup automatic retransmission of messages
    *     - Disable error interrups
    *     - Disable status interrupts
    *     - Enter initialization mode
    */
    canREG2->CTL = 0x00000000U | 0x00000000U | 0x00021443U;

    /** - Clear all pending error flags and reset current status */
    canREG2->ES = 0x0000031FU;

    /** - Assign interrupt level for messages */
    canREG2->INTMUXx[0U] = 0x00000001U
                         | 0x00000001U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U;

    canREG2->INTMUXx[1U] = 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U
                         | 0x00000000U;


    /** - Setup auto bus on timer pewriod */
    canREG2->ABOTR = 0U;

    /** - Initialize message 1 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x80000000U | 0x00000000U | 0x00000000U | ((1U & 0x000007FFU) << 18U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 1;

    /** - Initialize message 2 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((2U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 2;

    /** - Initialize message 3 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((3U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 3;

    /** - Initialize message 4 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((4U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 4;

    /** - Initialize message 5 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((5U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 5;

    /** - Initialize message 6 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((6U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 6;

    /** - Initialize message 7 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((7U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 7;

    /** - Initialize message 8 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((8U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 8;

    /** - Initialize message 9 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((9U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 9;

    /** - Initialize message 10 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((10U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 10;

    /** - Initialize message 11 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((11U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 11;

    /** - Initialize message 12 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((12U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 12;

    /** - Initialize message 13 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((13U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 13;

    /** - Initialize message 14 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((14U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 14;

    /** - Initialize message 15 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((15U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 15;

    /** - Initialize message 16 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((16U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 16;

    /** - Initialize message 17 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((17U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 17;

    /** - Initialize message 18 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((18U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 18;

    /** - Initialize message 19 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((19U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 19;

    /** - Initialize message 20 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((20U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 20;

    /** - Initialize message 21 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((21U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 21;

    /** - Initialize message 22 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((22U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 22;

    /** - Initialize message 23 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((23U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 23;

    /** - Initialize message 24 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((24U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 24;

    /** - Initialize message 25 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((25U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 25;

    /** - Initialize message 26 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((26U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 26;

    /** - Initialize message 27 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((27U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 27;

    /** - Initialize message 28 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((28U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 28;

    /** - Initialize message 29 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((29U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 29;

    /** - Initialize message 30 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((30U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 30;

    /** - Initialize message 31 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((31U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 31;

    /** - Initialize message 32 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((32U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 32;

    /** - Initialize message 33 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((33U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 33;

    /** - Initialize message 34 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((34U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 34;

    /** - Initialize message 35 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((35U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 35;

    /** - Initialize message 36 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((36U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 36;

    /** - Initialize message 37 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((37U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 37;

    /** - Initialize message 38 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((38U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 38;

    /** - Initialize message 39 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((39U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 39;

    /** - Initialize message 40 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((40U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 40;

    /** - Initialize message 41 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((41U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 41;

    /** - Initialize message 42 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((42U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 42;

    /** - Initialize message 43 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((43U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 43;

    /** - Initialize message 44 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((44U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 44;

    /** - Initialize message 45 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((45U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 45;

    /** - Initialize message 46 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((46U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 46;

    /** - Initialize message 47 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((47U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 47;

    /** - Initialize message 48 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((48U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 48;

    /** - Initialize message 49 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((49U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 49;

    /** - Initialize message 50 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((50U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 50;

    /** - Initialize message 51 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((51U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 51;

    /** - Initialize message 52 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((52U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 52;

    /** - Initialize message 53 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((53U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 53;

    /** - Initialize message 54 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((54U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 54;

    /** - Initialize message 55 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((55U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 55;

    /** - Initialize message 56 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((56U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 56;

    /** - Initialize message 57 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((57U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 57;

    /** - Initialize message 58 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((58U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 58;

    /** - Initialize message 59 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((59U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 59;

    /** - Initialize message 60 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((60U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 60;

    /** - Initialize message 61 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((61U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 61;

    /** - Initialize message 62 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((62U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 62;

    /** - Initialize message 63 
    *     - Wait until IF1 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF1 control byte
    *     - Set IF1 message number
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF1ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((63U & 0x1FFFFFFFU) << 0U);
    canREG2->IF1MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF1CMD  = 0xF8;
    canREG2->IF1NO   = 63;

    /** - Initialize message 64 
    *     - Wait until IF2 is ready for use 
    *     - Set message mask
    *     - Set message control word
    *     - Set message arbitration
    *     - Set IF2 control byte
    *     - Set IF2 message number
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2MSK  = 0xC0000000U | ((0x000007FFU & 0x1FFFFFFFU) << 0U);
    canREG2->IF2ARB  = 0x00000000U | 0x40000000U | 0x20000000U | ((64U & 0x1FFFFFFFU) << 0U);
    canREG2->IF2MCTL = 0x00001080U | 0x00000000U | 8U;
    canREG2->IF2CMD  = 0xF8;
    canREG2->IF2NO   = 64;

    /** - Setup IF1 for data transmission 
    *     - Wait until IF1 is ready for use 
    *     - Set IF1 control byte
    */
    while (canREG2->IF1STAT & 0x80);

    canREG2->IF1CMD  = 0x87;

    /** - Setup IF2 for reading data
    *     - Wait until IF1 is ready for use 
    *     - Set IF1 control byte
    */
    while (canREG2->IF2STAT & 0x80);

    canREG2->IF2CMD = 0x17;

    /** - Setup bit timing 
    *     - Setup baud rate prescaler extension
    *     - Setup TSeg2
    *     - Setup TSeg1
    *     - Setup sample jump width
    *     - Setup baud rate prescaler
    */
/*    canREG2->BTR = (0U << 16U) |
                   ((6U - 1U) << 12U) |
                   (((7U + 6U) - 1U) << 8U) |
                   ((4U - 1U) << 6U) |
                   9U;
*/ 
    canREG2->BTR = (0U << 16U) |
                   ((3U - 1U) << 12U) |
                   (((1U + 3U) - 1U) << 8U) |
                   ((3U - 1U) << 6U) |
                   9U;

    /** - Setup TX pin to functional output */
    canREG2->TIOC = 0x0000004CU;

    /** - Setup RX pin to functional input */
    canREG2->RIOC = 0x00000048U;

    /** - Leave configuration and initialization mode  */
    canREG2->CTL &= ~0x00000041U;


    /**   @note This function has to be called before the driver can be used.\n
    *           This function has to be executed in priviledged mode.\n
    */

/* USER CODE BEGIN (5) */
/* USER CODE END */
}


/** @fn unsigned canTransmit(canBASE_t *node, unsigned messageBox, const uint8_t *data)
*   @brief Transmits a CAN message
*   @param[in] node Pointer to CAN node:
*              - canREG1: DCAN1 node pointer
*              - canREG2: DCAN2 node pointer
*              - canREG3: DCAN3 node pointer
*   @param[in] messageBox Message box number of CAN node:
*              - canMESSAGE_BOX1: CAN message box 1
*              - canMESSAGE_BOXn: CAN message box n [n: 1-64]
*              - canMESSAGE_BOX64: CAN message box 64
*   @param[in] data Pointer to CAN TX data
*   @return The function will return:
*           - 0: When the setup of the TX message box wasn't successful   
*           - 1: When the setup of the TX message box was successful   
*
*   This function writes a CAN message into a CAN message box.
*
*/

/* USER CODE BEGIN (6) */
/* USER CODE END */

unsigned canTransmit(canBASE_t *node, unsigned messageBox, const uint8_t *data)
{
    unsigned i;
    unsigned success  = 0U;
    unsigned regIndex = (messageBox - 1U) >> 5U;
    unsigned bitIndex = 1U << ((messageBox - 1U) & 0x1FU);

/* USER CODE BEGIN (7) */
/* USER CODE END */

    /** - Check for pending message:
    *     - pending message, return 0
    *     - no pending message, start new transmission 
    */
    if (node->TXRQx[regIndex] & bitIndex)
    {
        return success;
    }

    /** - Wait until IF1 is ready for use */
    while (node->IF1STAT & 0x80);

    /** - Copy TX data into IF1 */
    for (i = 0U; i < 8U; i++)
    {
#if defined (_TMS470_LITTLE) || defined(__little_endian__)
        node->IF1DATx[i] = *data++;
#else
        node->IF1DATx[s_canByteOrder1[i]] = *data++;
#endif
    }

    /** - Copy TX data into mesasge box */
    node->IF1NO = messageBox;

    success = 1U; 

    /**   @note The function canInit has to be called before this function can be used.\n
    *           The user is responsible to initialize the message box.
    */

/* USER CODE BEGIN (8) */
/* USER CODE END */

    return success;
}


/** @fn unsigned canGetData(canBASE_t *node, unsigned messageBox, uint8_t * const data)
*   @brief Gets received a CAN message
*   @param[in] node Pointer to CAN node:
*              - canREG1: DCAN1 node pointer
*              - canREG2: DCAN2 node pointer
*              - canREG3: DCAN3 node pointer
*   @param[in] messageBox Message box number of CAN node:
*              - canMESSAGE_BOX1: CAN message box 1
*              - canMESSAGE_BOXn: CAN message box n [n: 1-64]
*              - canMESSAGE_BOX64: CAN message box 64
*   @param[out] data Pointer to store CAN RX data
*   @return The function will return:
*           - 0: When RX message box hasn't received new data   
*           - 1: When RX data are stored in the data buffer   
*           - 3: When RX data are stored in the data buffer and a message was lost   
*
*   This function writes a CAN message into a CAN message box.
*
*/


/* USER CODE BEGIN (9) */
/* USER CODE END */

unsigned canGetData(canBASE_t *node, unsigned messageBox, uint8_t * const data)
{
    unsigned       i;
    unsigned       size;
    uint8_t *pData    = (uint8_t *)data;
    unsigned       success  = 0U;
    unsigned       regIndex = (messageBox - 1U) >> 5U;
    unsigned       bitIndex = 1U << ((messageBox - 1U) & 0x1FU);
    uint32_t * plData = (uint32_t *)pData;

/* USER CODE BEGIN (10) */
/* USER CODE END */

    /** - Check if new data have been arrived:
    *     - no new data, return 0
    *     - new data, get received message 
    */
    if (!(node->NWDATx[regIndex] & bitIndex))
    {
        return success;
    }

    /** - Wait until IF2 is ready for use */
    while (node->IF2STAT & 0x80);

	node->IF1CMD |= 0x20;

    /** - Copy data into IF2 */
    node->IF2NO = messageBox;

    /** - Wait until data are copied into IF2 */
    while (node->IF2STAT & 0x80);

    /** - Get number of received bytes */
    size = node->IF2MCTL & 0xFU;
    
    if(node->IF2ARB & 0x40000000){
    //extended ID
    	*plData++ = (node->IF2ARB & 0x1FFFFFFF) | 0x80000000;
    }else{
    //Standard ID
    	*plData++ = (node->IF2ARB & 0x1FFC0000) >> 18;
    }
    
    pData++;
    pData++;
    pData++;
    pData++;
    

    /** - Copy RX data into destination buffer */
    for (i = 0U; i < size; i++)
    {
#if defined (_TMS470_LITTLE) || defined(__little_endian__)
        *pData++ = node->IF2DATx[i];
#else
        *pData++ = node->IF2DATx[s_canByteOrder1[i]];
#endif
    }

    success = 1U;

    /** - Check if data have been lost:
    *     - no data lost, return 1
    *     - data lost, return 3 
    */
    if (node->IF2MCTL & 0x4000U)
    {
        success = 3U;
    }

    /**   @note The function canInit has to be called before this function can be used.\n
    *           The user is responsible to initialize the message box.
    */

/* USER CODE BEGIN (11) */
/* USER CODE END */

    return success;
}


/** @fn unsigned canIsTxMessagePending(canBASE_t *node, unsigned messageBox)
*   @brief Gets Tx message box transmission status
*   @param[in] node Pointer to CAN node:
*              - canREG1: DCAN1 node pointer
*              - canREG2: DCAN2 node pointer
*              - canREG3: DCAN3 node pointer
*   @param[in] messageBox Message box number of CAN node:
*              - canMESSAGE_BOX1: CAN message box 1
*              - canMESSAGE_BOXn: CAN message box n [n: 1-64]
*              - canMESSAGE_BOX64: CAN message box 64
*   @return The function will return the tx request flag
*
*   Checks to see if the Tx message box has a pending Tx request, returns
*   0 is flag not set otherwise will return the Tx request flag itself.
*/


/* USER CODE BEGIN (12) */
/* USER CODE END */

unsigned canIsTxMessagePending(canBASE_t *node, unsigned messageBox)
{
    unsigned flag;
    unsigned regIndex = (messageBox - 1U) >> 5U;
    unsigned bitIndex = 1U << ((messageBox - 1U) & 0x1FU);

/* USER CODE BEGIN (13) */
/* USER CODE END */

    /** - Read Tx request reigster */
    flag = node->TXRQx[regIndex] & bitIndex;

/* USER CODE BEGIN (14) */
/* USER CODE END */

    return flag;
}


/** @fn unsigned canIsRxMessageArrived(canBASE_t *node, unsigned messageBox)
*   @brief Gets Rx message box reception status
*   @param[in] node Pointer to CAN node:
*              - canREG1: DCAN1 node pointer
*              - canREG2: DCAN2 node pointer
*              - canREG3: DCAN3 node pointer
*   @param[in] messageBox Message box number of CAN node:
*              - canMESSAGE_BOX1: CAN message box 1
*              - canMESSAGE_BOXn: CAN message box n [n: 1-64]
*              - canMESSAGE_BOX64: CAN message box 64
*   @return The function will return the new data flag
*
*   Checks to see if the Rx message box has pending Rx data, returns
*   0 is flag not set otherwise will return the Tx request flag itself.
*/


/* USER CODE BEGIN (15) */
/* USER CODE END */

unsigned canIsRxMessageArrived(canBASE_t *node, unsigned messageBox)
{
    unsigned flag;
    unsigned regIndex = (messageBox - 1U) >> 5U;
    unsigned bitIndex = 1U << ((messageBox - 1U) & 0x1FU);

/* USER CODE BEGIN (16) */
/* USER CODE END */

    /** - Read Tx request register */
    flag = node->NWDATx[regIndex] & bitIndex;

/* USER CODE BEGIN (17) */
/* USER CODE END */

    return flag;
}


/** @fn unsigned canIsMessageBoxValid(canBASE_t *node, unsigned messageBox)
*   @brief Chechs if message box is valid
*   @param[in] node Pointer to CAN node:
*              - canREG1: DCAN1 node pointer
*              - canREG2: DCAN2 node pointer
*              - canREG3: DCAN3 node pointer
*   @param[in] messageBox Message box number of CAN node:
*              - canMESSAGE_BOX1: CAN message box 1
*              - canMESSAGE_BOXn: CAN message box n [n: 1-64]
*              - canMESSAGE_BOX64: CAN message box 64
*   @return The function will return the new data flag
*
*   Checks to see if the message box is valid for operation, returns
*   0 is flag not set otherwise will return the validation flag itself.
*/


/* USER CODE BEGIN (18) */
/* USER CODE END */

unsigned canIsMessageBoxValid(canBASE_t *node, unsigned messageBox)
{
    unsigned flag;
    unsigned regIndex = (messageBox - 1U) >> 5U;
    unsigned bitIndex = 1U << ((messageBox - 1U) & 0x1FU);

/* USER CODE BEGIN (19) */
/* USER CODE END */

    /** - Read Tx request register */
    flag = node->MSGVALx[regIndex] & bitIndex;

/* USER CODE BEGIN (20) */
/* USER CODE END */

    return flag;
}


/** @fn unsigned canGetLastError(canBASE_t *node)
*   @brief Gets last RX/TX-Error of CAN message traffic
*   @param[in] node Pointer to CAN node:
*              - canREG1: DCAN1 node pointer
*              - canREG2: DCAN2 node pointer
*              - canREG3: DCAN3 node pointer
*   @return The function will return:
*           - canERROR_OK (0): When no CAN error occured   
*           - canERROR_STUFF (1): When a stuff error occured on RX message    
*           - canERROR_FORMAT (2): When a form/format error occured on RX message   
*           - canERROR_ACKNOWLEDGE (3): When a TX message wasn't acknowledged  
*           - canERROR_BIT1 (4): When a TX message monitored dominant level where recessive is expected   
*           - canERROR_BIT0 (5): When a TX message monitored recessive level where dominant is expected   
*           - canERROR_CRC (6): When a RX message has wrong CRC value   
*           - canERROR_NO (7): When no error occured since last call of this function   
*
*   This function returns the last occured error code of an RX or TX message,
*   since the last call of this function.
*
*/


/* USER CODE BEGIN (21) */
/* USER CODE END */

unsigned canGetLastError(canBASE_t *node)
{
    unsigned errorCode;

/* USER CODE BEGIN (22) */
/* USER CODE END */

    /** - Get last error code */
    errorCode = node->ES & 7U;

    /**   @note The function canInit has to be called before this function can be used. */

/* USER CODE BEGIN (23) */
/* USER CODE END */

    return errorCode;
}


/** @fn unsigned canGetErrorLevel(canBASE_t *node)
*   @brief Gets error level of a CAN node
*   @param[in] node Pointer to CAN node:
*              - canREG1: DCAN1 node pointer
*              - canREG2: DCAN2 node pointer
*              - canREG3: DCAN3 node pointer
*   @return The function will return:
*           - canLEVEL_ACTIVE (0x00): When RX- and TX error counters are below 96   
*           - canLEVEL_WARNING (0x40): When RX- or TX error counter are between 96 and 127     
*           - canLEVEL_PASSIVE (0x20): When RX- or TX error counter are between 128 and 255     
*           - canLEVEL_BUS_OFF (0x80): When RX- or TX error counter are above 255     
*
*   This function returns the current error level of a CAN node.
*
*/


/* USER CODE BEGIN (24) */
/* USER CODE END */

unsigned canGetErrorLevel(canBASE_t *node)
{
    unsigned errorLevel;

/* USER CODE BEGIN (25) */
/* USER CODE END */

    /** - Get error level */
    errorLevel = node->ES & 0xE0U;

    /**   @note The function canInit has to be called before this function can be used. */

/* USER CODE BEGIN (26) */
/* USER CODE END */

    return errorLevel;
}


#endif
