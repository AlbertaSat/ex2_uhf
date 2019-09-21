//*****************************************************************************
//
// bl_led_demo.h - Definitions for the Liting LEDs
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

#ifndef BL_LED_DEMO_H_
#define BL_LED_DEMO_H_


#include "sys_common.h"
#include "bl_config.h"

void run_LED_StartUp(uint8_t ledNum);

#if defined(TMS570LS04) || defined(RM42)

#define LITE_TOPRIGHT_LED    run_LED_StartUp(3)
#define LITE_BOTRIGHT_LED    run_LED_StartUp(2)
#define LITE_TOPLEFT_LED     run_LED_StartUp(6)
#define LITE_BOTLEFT_LED     run_LED_StartUp(4)

#else

#define LITE_TOPRIGHT_LED    run_LED_StartUp(0)
#define LITE_BOTRIGHT_LED    run_LED_StartUp(25)
#define LITE_TOPLEFT_LED     run_LED_StartUp(17)
#define LITE_BOTLEFT_LED     run_LED_StartUp(29)

#endif


#endif /* BL_LED_DEMO_H_ */
