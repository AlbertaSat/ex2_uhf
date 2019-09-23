//*****************************************************************************
//
// sys_startup.c
// Author      : QJ Wang. qjwang@ti.com
// Date        : 9-19-2012
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
//*****************************************************************************

/* Include Files */

#include "sys_common.h"
#include "system.h"
#include "sys_vim.h"
#include "sys_core.h"
//#include "stdio.h"


/* Type Definitions */

typedef void (*handler_fptr)(const uint8_t *in, uint8_t *out);

/* External Functions */

#pragma WEAK(__TI_Handler_Table_Base)
#pragma WEAK(__TI_Handler_Table_Limit)
#pragma WEAK(__TI_CINIT_Base)
#pragma WEAK(__TI_CINIT_Limit)

extern uint32_t   __TI_Handler_Table_Base;
extern uint32_t   __TI_Handler_Table_Limit;
extern uint32_t   __TI_CINIT_Base;
extern uint32_t   __TI_CINIT_Limit;
extern uint32_t   __TI_PINIT_Base;
extern uint32_t   __TI_PINIT_Limit;
extern uint32_t * __binit__;

extern void main(void);
extern void exit(void);


/* Vim Ram Definition */
/** @struct vimRam
*   @brief Vim Ram Definition
*
*   This type is used to access the Vim Ram.
*/
/** @typedef vimRAM_t
*   @brief Vim Ram Type Definition
*
*   This type is used to access the Vim Ram.
*/
typedef volatile struct vimRam
{
    t_isrFuncPTR ISR[VIM_CHANNELS + 1];
} vimRAM_t;

#define vimRAM ((vimRAM_t *)0xFFF82000U)

static const t_isrFuncPTR s_vim_init[]=
{
    &phantomInterrupt,
    &esmHighInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
    &phantomInterrupt,
};


/* Startup Routine */
#pragma INTERRUPT(_c_int00, RESET)

void _c_int00()
{
	    /* Initialize Core Registers to avoid CCM Error */
	    _coreInitRegisters_();

	    /* Initialize Stack Pointers */
	    _coreInitStackPointer_();


	    /* Implement work-around for CCM-R4 issue on silicon revision A */
	    if (DEVICE_ID_REV == 0x802AAD05)
	    {
	        _esmCcmErrorsClear_();
	    }

	    /* Enable response to ECC errors indicated by CPU for accesses to flash */
	    flashWREG->FEDACCTRL1 = 0x000A060A;


	    /* Reset handler: the following instructions read from the system exception status register
	     * to identify the cause of the CPU reset.
	     */

		/* check for power-on reset condition */
		if ((SYS_EXCEPTION & POWERON_RESET) != 0)
		{
			/* clear all reset status flags */
			SYS_EXCEPTION = 0xFFFF;
			/* continue with normal start-up sequence */
		}
		else if ((SYS_EXCEPTION & OSC_FAILURE_RESET) != 0)
		{
			/* Reset caused due to oscillator failure.
			Add user code here to handle oscillator failure */
		}
		else if ((SYS_EXCEPTION & WATCHDOG_RESET) !=0 )
		{
			/* Reset caused due
			 *  1) windowed watchdog violation - Add user code here to handle watchdog violation.
			 *  2) ICEPICK Reset - After loading code via CCS / System Reset through CCS
			 */
			/* Check the WatchDog Status register */
		    if(WATCHDOG_STATUS != 0U)
			{
			    /* Add user code here to handle watchdog violation. */
			    /* Clear the Watchdog reset flag in Exception Status register */
			    SYS_EXCEPTION = WATCHDOG_RESET;
			}
			else
			{
			    /* Clear the ICEPICK reset flag in Exception Status register */
			    SYS_EXCEPTION = ICEPICK_RESET;
			}
		}
		else if ((SYS_EXCEPTION & CPU_RESET) !=0 )
		{
			/* Reset caused due to CPU reset.
			CPU reset can be caused by CPU self-test completion, or
			by toggling the "CPU RESET" bit of the CPU Reset Control Register. */

			/* clear all reset status flags */
			SYS_EXCEPTION = CPU_RESET;

		}
		else if ((SYS_EXCEPTION & SW_RESET) != 0)
		{
			/* Reset caused due to software reset.
			Add user code to handle software reset. */

		}
		else
		{
			/* Reset caused by nRST being driven low externally.
			Add user code to handle external reset. */

		}


		/* Initialize System - Clock, Flash settings with Efuse self check */
		systemInit();

	    /* Enable IRQ offset via Vic controller */
	    _coreEnableIrqVicOffset_();

	    /* Initialize VIM table */
	    {
	        uint32_t i;

	        for (i = 0; i < (VIM_CHANNELS + 1); i++)
	        {
	            vimRAM->ISR[i] = s_vim_init[i];
	        }
	    }

	    /* set IRQ/FIQ priorities */
	    vimREG->FIRQPR0 =  0x00000003;
	    vimREG->FIRQPR1 =  0x00000000;
	    vimREG->FIRQPR2 =  0x00000000;
	    vimREG->FIRQPR3 =  0x00000000;
						
	    /* enable interrupts */
	    vimREG->REQMASKSET0 = 0x00000003;

	    vimREG->REQMASKSET1 =  0x00000000;
	    vimREG->REQMASKSET2 =  0x00000000;
	    vimREG->REQMASKSET3 =  0x00000000;

	    /* initialize copy table */
	    if ((uint32_t *)&__binit__ != (uint32_t *)0xFFFFFFFFU)
	    {
	        extern void copy_in(void *binit);
	        copy_in((void *)&__binit__);
	    }

	    /* initialize the C global variables */
	    if (&__TI_Handler_Table_Base < &__TI_Handler_Table_Limit)
	    {
	        uint8_t **tablePtr   = (uint8_t **)&__TI_CINIT_Base;
	        uint8_t **tableLimit = (uint8_t **)&__TI_CINIT_Limit;

	        while (tablePtr < tableLimit)
	        {
	            uint8_t *loadAdr = *tablePtr++;
	            uint8_t *runAdr  = *tablePtr++;
	            uint8_t  idx     = *loadAdr++;
	            handler_fptr   handler = (handler_fptr)(&__TI_Handler_Table_Base)[idx];

	            (*handler)((const uint8_t *)loadAdr, runAdr);
	        }
	    }

	    /* initialize contructors */
	    if (__TI_PINIT_Base < __TI_PINIT_Limit)
	    {
	        void (**p0)() = (void *)__TI_PINIT_Base;

	        while ((uint32_t)p0 < __TI_PINIT_Limit)
	        {
	            void (*p)() = *p0++;
	            p();
	        }
	    }

	_copyAPI2RAM_();

/* call the application */
    main();

    exit();
}
