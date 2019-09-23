//*****************************************************************************
//
// system.c    : Functions to configure PLL, and peripherals
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

#include "system.h"
#include "bl_config.h"

//#include "sys_selftest.h"
//#include "pinmux.h"

/* USER CODE BEGIN (1) */
/* USER CODE END */

/** @fn void systemInit(void)
*   @brief Initializes System Driver
*
*   This function initializes the System driver.
*
*/

/* USER CODE BEGIN (2) */
/* USER CODE END */

void setupPLL(void)
{

/* USER CODE BEGIN (3) */
/* USER CODE END */

    /** - Configure PLL control registers */
    /** @b Initialize @b Pll1: */

    /**   - Setup pll control register 1:
    *     - Setup reset on oscillator slip 
    *     - Setup bypass on pll slip
    *     - setup Pll output clock divider to max before Lock
    *     - Setup reset on oscillator fail
    *     - Setup reference clock divider 
    *     - Setup Pll multiplier          
    */
    systemREG1->PLLCTL1 =  0x00000000U 
                        |  0x20000000U 
                        | ((0x1F)<< 24U) 
                        |  0x00000000U 
                        | ((6U - 1U)<< 16U) 
                        | ((120U - 1U)<< 8U);

    /**   - Setup pll control register 2
    *     - Enable/Disable frequency modulation
    *     - Setup spreading rate
    *     - Setup bandwidth adjustment
    *     - Setup internal Pll output divider
    *     - Setup spreading amount
    */
    systemREG1->PLLCTL2 =  0x00000000U
                        | (255U << 22U)
                        | (7U << 12U)
                        | ((2U - 1U)<< 9U)
                        |  61U;

    /** @b Initialize @b Pll2: */

    /**   - Setup pll2 control register :
    *     - setup Pll output clock divider to max before Lock
    *     - Setup reference clock divider 
    *     - Setup internal Pll output divider
    *     - Setup Pll multiplier          
    */
    systemREG2->PLLCTL3 = ((2U - 1U) << 29U)
                        | ((0x1F)<< 24U) 
                        | ((5U - 1U)<< 16U) 
                        | ((30U - 1U) << 8U);
    
	/** - Enable PLL(s) to start up or Lock */
    systemREG1->CSDIS = 0x00000000U	
                      | 0x00000000U 
                      | 0x00000008U 
                      | 0x00000080U 
                      | 0x00000000U 
                      | 0x00000000U 
                      | 0x00000000U;
}

void trimLPO(void)
{

/* USER CODE BEGIN (4) */
/* USER CODE END */

    /** @b Initialize Lpo: */
    /** Load TRIM values from OTP if present else load user defined values */
    if(LPO_TRIM_VALUE != 0xFFFF)
    {
    
        systemREG1->LPOMONCTL  = (1U << 24U)
                                | LPO_TRIM_VALUE;
    }
    else
    {

    	systemREG1->LPOMONCTL 	= (1U << 24U)
                                 | (16U << 8U)
                                 | 8U;
    }

/* USER CODE BEGIN (5) */
/* USER CODE END */

}

void setupFlash(void)
{

/* USER CODE BEGIN (6) */
/* USER CODE END */

    /** - Setup flash read mode, address wait states and data wait states */
    flashWREG->FRDCNTL =  0x00000000U 
#if defined(TMS570LS04) || defined(RM42)
                       | (1U << 8U)
                       | (0U << 4U)
#else
                       | (3U << 8U) 
                       | (1U << 4U) 
#endif
                       |  1U;

    /** - Setup flash access wait states for bank 7 */
    FSM_WR_ENA = 0x5;
    EEPROM_CONFIG = 0x00030002;

/* USER CODE BEGIN (7) */
/* USER CODE END */

    /** - Disable write access to flash state machine registers */
    FSM_WR_ENA = 0xA;

    /** - Setup flash bank power modes */
    flashWREG->FBFALLBACK = 0x00000000
                          | (SYS_ACTIVE << 14U) 
                          | (SYS_SLEEP << 12U) 
                          | (SYS_SLEEP << 10U) 
                          | (SYS_SLEEP << 8U) 
                          | (SYS_SLEEP << 6U) 
                          | (SYS_SLEEP << 4U) 
#if defined(TMS570LS04) || defined(RM42)
                          | (SYS_SLEEP << 2U)
#else
                          | (SYS_ACTIVE << 2U)
#endif
                          |  SYS_ACTIVE;

/* USER CODE BEGIN (8) */
/* USER CODE END */

}

void periphInit(void)
{

/* USER CODE BEGIN (9) */
/* USER CODE END */

    /** - Disable Peripherals before peripheral powerup*/
    systemREG1->PENA = 0U;

	/** - Release peripherals from reset and enable clocks to all peripherals */
    /** - Power-up all peripharals */
    pcrREG->PSPWRDWNCLR0 = 0xFFFFFFFFU;
    pcrREG->PSPWRDWNCLR1 = 0xFFFFFFFFU;
    pcrREG->PSPWRDWNCLR2 = 0xFFFFFFFFU;
    pcrREG->PSPWRDWNCLR3 = 0xFFFFFFFFU;

    /** - Enable Peripherals */
    systemREG1->PENA = 1U;
	
/* USER CODE BEGIN (10) */
/* USER CODE END */

}

void mapClocks(void)
{
	
/* USER CODE BEGIN (11) */
/* USER CODE END */

    /** @b Initialize @b Clock @b Tree: */
    /** - Diable / Enable clock domain */
    systemREG1->CDDIS= (FALSE << 4 ) /* AVCLK 1 OFF */
                      |(TRUE << 5 ) /* AVCLK 2 OFF */
	                  |(FALSE << 8 ) /* VCLK3 OFF */
	                  |(FALSE << 10) /* AVCLK 3 OFF */
                      |(FALSE << 11); /* AVCLK 4 OFF */

    /** - Wait for until clocks are locked */
    while ((systemREG1->CSVSTAT & ((systemREG1->CSDIS ^ 0xFF) & 0xFF)) != ((systemREG1->CSDIS ^ 0xFF) & 0xFF))
    {
    }
//    while (!(systemREG1->CSVSTAT & 0x040));

/* USER CODE BEGIN (12) */
/* USER CODE END */

	/* Now the PLLs are locked and the PLL outputs can be sped up */
	/* The R-divider was programmed to be 0xF. Now this divider is changed to programmed value */
#if defined(TMS570LS04) || defined(RM42)
    systemREG1->PLLCTL1 = (systemREG1->PLLCTL1 & 0xE0FFFFFF)|((2U - 1U)<< 24U);
#else
    systemREG1->PLLCTL1 = (systemREG1->PLLCTL1 & 0xE0FFFFFF)|((1U - 1U)<< 24U);
#endif
    systemREG2->PLLCTL3 = (systemREG2->PLLCTL3 & 0xE0FFFFFF)|((1U - 1U)<< 24U);
	
	/** - Map device clock domains to desired sources and configure top-level dividers */
	/** - All clock domains are working off the default clock sources until now */
	/** - The below assignments can be easily modified using the HALCoGen GUI */
    
    /** - Setup GCLK, HCLK and VCLK clock source for normal operation, power down mode and after wakeup */
    systemREG1->GHVSRC = (SYS_PLL1 << 24U) 
                       | (SYS_PLL1 << 16U) 
                       |  SYS_PLL1;
                       
    /** - Setup synchronous peripheral clock dividers for VCLK1, VCLK2, VCLK3 */
#if defined(TMS570LS04) || defined(RM42)
    systemREG1->VCLKR   = 1;   //1U;
    systemREG1->VCLK2R  = 1;    //1U;
    systemREG2->VCLK3R  = 1U;
#else
    systemREG1->VCLKR   = 1;   //1U;
    systemREG1->VCLK2R  = 1;    //1U;
    systemREG2->VCLK3R  = 1U;
#endif
/* USER CODE BEGIN (13) */
/* USER CODE END */
	
    /** - Setup RTICLK1 and RTICLK2 clocks */
    systemREG1->RCLKSRC = (1U << 24U)
                        | (SYS_VCLK << 16U) 
                        | (1U << 8U)  
                        |  SYS_VCLK;

    /** - Setup asynchronous peripheral clock sources for AVCLK1 and AVCLK2 */
    systemREG1->VCLKASRC = (SYS_VCLK << 8U)
                          |  SYS_VCLK;

    systemREG2->VCLKACON1 = (1U << 24) 
                           | 1 << 20U 
                           | (SYS_VCLK << 16)
                           | (1U << 8)
                           | 1 << 4U 
#if defined(TMS570LS04) || defined(RM42)
                           | SYS_VCLK;
#else
                           | SYS_PLL2;
#endif
/* USER CODE BEGIN (14) */
/* USER CODE END */

}

void systemInit(void)
{
/* USER CODE BEGIN (15) */
/* USER CODE END */

	/* Configure PLL control registers and enable PLLs.
	 * The PLL takes (127 + 1024 * NR) oscillator cycles to acquire lock.
	 * This initialization sequence performs all the tasks that are not
	 * required to be done at full application speed while the PLL locks.
	 */
	setupPLL();
	
	
/* USER CODE BEGIN (17) */
/* USER CODE END */
	
	/* Enable clocks to peripherals and release peripheral reset */
	periphInit();

/* USER CODE BEGIN (18) */
/* USER CODE END */

	
/* USER CODE BEGIN (20) */
/* USER CODE END */

	/** - Set up flash address and data wait states based on the target CPU clock frequency
	 * The number of address and data wait states for the target CPU clock frequency are specified
	 * in the specific part's datasheet.
	 */
	setupFlash();

/* USER CODE BEGIN (21) */
/* USER CODE END */

	/** - Configure the LPO such that HF LPO is as close to 10MHz as possible */
	trimLPO();

/* USER CODE BEGIN (23) */
/* USER CODE END */

	/** - Wait for PLLs to start up and map clock domains to desired clock sources */
	mapClocks();

/* USER CODE BEGIN (24) */
/* USER CODE END */

	/** - set ECLK pins functional mode */
    systemREG1->SYSPC1 = 0U;

    /** - set ECLK pins default output value */
    systemREG1->SYSPC4 = 0U;

    /** - set ECLK pins output direction */
    systemREG1->SYSPC2 = 1U;

    /** - set ECLK pins open drain enable */
    systemREG1->SYSPC7 = 0U;

    /** - set ECLK pins pullup/pulldown enable */
    systemREG1->SYSPC8 = 0U;   

    /** - set ECLK pins pullup/pulldown select */
    systemREG1->SYSPC9 = 1U;

    /** - Setup ECLK */
    systemREG1->ECPCNTL = (0U << 24U)
                        | (0U << 23U)
                        | ((8U - 1U) & 0xFFFFU);

/* USER CODE BEGIN (25) */
/* USER CODE END */
}

void systemPowerDown(uint32_t mode)
{

/* USER CODE BEGIN (26) */
/* USER CODE END */

    /* Disable clock sources */
    systemREG1->CSDISSET = mode & 0x000000FFU;

    /* Disable clock domains */
    systemREG1->CDDIS = (mode >> 8U) & 0x00000FFFU;

    /* Idle CPU */
    asm(" wfi");

/* USER CODE BEGIN (27) */
/* USER CODE END */

}

/* USER CODE BEGIN (28) */
/* USER CODE END */
