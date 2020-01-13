/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
#include "gio.h"
#include "het.h"
#include "FreeRTOS.h"
#include "adf7021.h"
#include "spi.h"
#include "stdio.h"
#include "stdlib.h"
/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */
#   define PINSET( PINNAME ) gioSetBit( gioPORTA, PINNAME, 1 )
#   define PINCLEAR( PINNAME ) gioSetBit( gioPORTA, PINNAME, 0 )
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */

/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
    gioInit();
    spiInit();
    gioSetDirection(gioPORTB, 0xFFFFFFFF);

    spiDAT1_t dataconfig1_t;

    dataconfig1_t.CS_HOLD = FALSE;
    dataconfig1_t.WDEL    = FALSE;
    dataconfig1_t.DFSEL   = SPI_FMT_0;
    dataconfig1_t.CSNR    = 0x00;


    uint16 data = 0b1010101010101010;


    unsigned long mcu_xtal = 50000000; //TODO: define this

    adf_set_power_on(mcu_xtal);
    //fprintf(stderr, "power on successful");

    adf_configure();
    //fprintf(stderr, "configuration successful");

    adf_set_tx_mode();
    //fprintf(stderr, "tx mode on");

    spiSendData(spiREG3, &dataconfig1_t, 1, &data);
    spiTransmitData(spiREG3, &dataconfig1_t, 1, &data);


/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */

/* USER CODE END */
