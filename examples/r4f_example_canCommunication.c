/** @example example_canCommunication.c
*   This is an example which describes the steps to create an example application which 
*   configures two can nodes 1 and 2 and starts a communication with a sample data chunk.
*   CAN 1 and CAN2 must be part of CAN network
*
*   
*   @b Step @b 1:
*
*   Create a new project.
*
*   Navigate: -> File -> New -> Project
*
*   @image html example_createProject.jpg "Figure: Create a new Project"
*
*   @b Step @b 2:
*
*   Configure driver code generation: 
*   - Enable can driver   
*   - Disable others
*
*   Navigate: -> TMS570LSxx /RM4 -> Enable Drivers
*
*   @image html can_enabledriver.JPG "Figure: Enable CAN Driver"
*
*   @b Step @b 3:
*
*   Configure CAN Baudrate: 
*
*   Navigate: -> TMS570LSxx /RM4 -> CAN
*
*   @image html can1baud.JPG "Figure: CAN1 BaudRate Configuration"
*   @image html can2baud.JPG "Figure: CAN2 BaudRate Configuration"
*
*   @b Step @b 4:
*
*   Configure CAN MessageBox: 
*
*   - Configure CAN1 , MessageBox 1 -- Activate and Enable TX
*   - Configure CAN2 , MessageBox 1 -- Activate and Enable RX
*
*   Navigate: -> TMS570LSxx /RM4 -> CAN
*
*   @image html canCommunication1.JPG "Figure: CAN1 MBox Configuration"
*   @image html canCommunication2.JPG "Figure: CAN2 MBox Configuration"
*
*   @b Step @b 5:
*
*   Copy the source code below into your sys_main.c or replace sys_main.c with this file.
*
*   The example file can also be found in the examples folder: ../HALCoGen/examples
*
*   @note HALCoGen generates an empty main function in sys_main.c, 
*
*/

/* 
* Copyright (C) 2009-2015 Texas Instruments Incorporated - www.ti.com
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
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  INCLUDING NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "sys_common.h"
#include "system.h"

/* USER CODE BEGIN (1) */
#include "can.h"

/* Include ESM header file - types, definitions and function declarations for system driver */
#include "esm.h"

#define  D_SIZE 9

uint8  tx_data[D_SIZE]  = {'H','E','R','C','U','L','E','S','\0'};
uint8  rx_data[D_SIZE] = {0};
uint32 error = 0;

uint32 checkPackets(uint8 *src_packet,uint8 *dst_packet,uint32 psize);
/* USER CODE END */


/** @fn void main(void)
*   @brief Application main function
*
*/

/* USER CODE BEGIN (2) */
/* USER CODE END */


void main(void)
{
/* USER CODE BEGIN (3) */

    /* initialize can 1 and 2   */
    canInit(); /* can1 -> can2 */                                      

    /* transmit on can1 */
    canTransmit(canREG1, canMESSAGE_BOX1, tx_data); 

    /*... wait until message receive on can2 */
    while(!canIsRxMessageArrived(canREG2, canMESSAGE_BOX1));
    canGetData(canREG2, canMESSAGE_BOX1, rx_data);  /* receive on can2  */

    /* check received data patterns */
    error = checkPackets(&tx_data[0],&rx_data[0],D_SIZE);

    /* ... run forever */
    while(1);

/* USER CODE END */	
}

/* USER CODE BEGIN (4) */
/** @fn checkPackets(uint8 *src_packet,uint8 *dst_packet,uint32 psize)
*   @brief check two buffers and report error
*
*/
uint32 checkPackets(uint8 *src_packet,uint8 *dst_packet,uint32 psize)
{
   uint32 err=0;
   uint32 cnt=psize;

   while(cnt--)
   {
     if((*src_packet++) != (*dst_packet++))
     {
        err++;           /*  data error  */
     }
   }
   return (err);
}
/* USER CODE END */