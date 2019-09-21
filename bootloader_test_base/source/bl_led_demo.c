//*****************************************************************************
//
// bl_led_demo.c  : Flash the leds
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


#include "hw_het.h"
#include "hw_gio.h"
#include "sys_common.h"
#include "bl_led_demo.h"
#include "bl_config.h"

/** @fn run_LED_StartUp(void)
*   
*   This function is called during the start up, LED's are turned on 
*   in a sequence to indicate the board is powerd up and ready to run
*   the demo software.  
*   
*/



#if defined(TMS570LS04) || defined(RM42)

void run_LED_StartUp(uint8_t ledNum)
{
   	int i, temp,delay;

   	gioInit();

	/** - Delay Parameter */
	delay				= 0x40000;

// LED: GIOA6           GIOA3
// LED: GIOA4           GIOA2
//
	if(1)
	{
	    gioPORTA->DIR  =  0        /* Bit 0 */
	                   | (0 << 1)  /* Bit 1 */
	                   | (1 << 2)  /* Bit 2 */
	                   | (1 << 3)  /* Bit 3 */
	                   | (1 << 4)  /* Bit 4 */
	                   | (0 << 5)  /* Bit 5 */
	                   | (1 << 6)  /* Bit 6 */
	                   | (0 << 7); /* Bit 7 */

			/** - Turn all LEDs off */
			gioPORTA->DOUT  =  0x00;

			/** - Simple Dealy */
			for(temp=0;temp<delay;temp++);

			/** - Set only GIO[3] */
			gioPORTA->DOUT  = (0x01 << ledNum);;
			/** - Simple Dealy */
			for(temp=0;temp<delay;temp++);

	    }

	}

#else  //for TMS570LS31x/12x RM48/46

void run_LED_StartUp(uint8_t ledNum)
{
   	int temp,delay;
   	//LED Number: 0, 31, 17, 29, 18, 25
   	//LED1: 27   //Tricolor: 4,16,27,2,5,20
   	//LED2: 5
	/** - Delay Parameter */
	delay				= 0x40000;

    /** - NHET is configured as Master 
     *  - NHET is turned On */    
   	hetREG1->GCR     	= 0x01000001;
	
	/** - PULL functinality is enabled */
	hetREG1->PULDIS	= 0x00000000;
	 
	/** - This if statment is a placeholder for ArgoBoard/USBStick check */
	if(1)
	{
		/** - Configure NHET pins as output */
   		hetREG1->DIR = 0 | (1 << 0)
   				        | (1 << 5)
   				        | (1 << 17)
   				        | (1 << 18)
   				        | (1 << 25)
   				        | (1 << 27)
   				        | (1 << 29)
   				        | (1 << 31); //	0xAA06,0021;


		/** - Turn all LEDs off */
		//hetREG1->DOUT   	= 0xAA060021 & ~(1 << ledNum);
		hetREG1->DOUT   	= 0x00000000;

		/** - Simple Dealy */
		for(temp=0;temp<delay;temp++);

		/** - Set only NHET[0] */ 
		hetREG1->DOUT   	= (0x1 << ledNum);  //0x08110035
		/** - Simple Dealy */
		for(temp=0;temp<delay;temp++);
		
	}
}
#endif
