/** @example example_canCommunication_loopback.c
*   This is an example which describes using Loopback mode 
*   Configuring and Interrupt handling in CAN Module
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
*   - Enable can1 driver   
*   - Enable can2 driver   
*   - Enable can3 driver   
*   - Enable can4 driver   
*   - Disable others
*
*   Navigate: -> TMS570LSxx /RM4 -> Enable Drivers
*	@image html example_canenable.jpg "Figure: Create a new Project"
*
*   @b Step @b 3:
*
*   Configure CAN Baudrate: 
*
*   Navigate: -> TMS570Sxx /RMxx -> CAN
*
*   @b Step @b 4:
*
*   Enable all CAN High interrupts in the VIM Channels tab. 
*
*   Navigate: -> TMS570Sxx /RMxx -> VIM Channels
*
*   @b Step @b 5:
*
*   Configure CAN MessageBox: 
*
*   - Configure CAN1 , MessageBox 1 -- Activate and Enable TX
*   - Configure CAN1 , MessageBox 2 -- Activate and Enable RX and Enable Interrupt
*   - Configure CAN2 , MessageBox 1 -- Activate and Enable TX
*   - Configure CAN2 , MessageBox 2 -- Activate and Enable RX and Enable Interrupt
*   - Configure CAN3 , MessageBox 1 -- Activate and Enable TX
*   - Configure CAN3 , MessageBox 2 -- Activate and Enable RX and Enable Interrupt
*   - Configure CAN4 , MessageBox 1 -- Activate and Enable TX
*   - Configure CAN4 , MessageBox 2 -- Activate and Enable RX and Enable Interrupt
*
*	Change the ID such that both MessageBox 1 and 2 have the same ID.
*
*   Navigate: -> TMS570Sxx /RMxx -> CAN
*	@image html example_can_config.jpg "Figure: Create a new Project"
*
*   @b Step @b 6:
*
*   Copy the source code below into your sys_main.c or replace sys_main.c with this file.
*
*   The example file example_canCommunication.c can also be found in the examples folder: ../HALCoGen/examples
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

#include "HL_sys_common.h"
#include "HL_system.h"

/* USER CODE BEGIN (1) */

#include "HL_can.h"
#include "HL_esm.h"
#include "HL_sys_core.h"

#define D_COUNT  8

uint32 cnt=0, error =0, tx_done =0;
uint8 tx_data1[D_COUNT] = {1,2,3,4,5,6,7,8};
uint8 tx_data2[D_COUNT] = {11,12,13,14,15,16,17,18};
uint8 tx_data3[D_COUNT] = {21,22,23,24,25,26,27,28};
uint8 tx_data4[D_COUNT] = {31,32,33,34,35,36,37,38};

uint8 rx_data1[D_COUNT] = {0};
uint8 rx_data2[D_COUNT] = {0};
uint8 rx_data3[D_COUNT] = {0};
uint8 rx_data4[D_COUNT] = {0};

uint8 *dptr=0;


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

void main(void)
{
/* USER CODE BEGIN (3) */

    /* enable irq interrupt in */
    _enable_IRQ_interrupt_();

    /** - configuring CAN1 MB1,Msg ID-1 to transmit and CAN2 MB1 to receive */
    canInit();
    canEnableloopback(canREG1, Internal_Lbk);
    canEnableloopback(canREG2, Internal_Lbk);
    canEnableloopback(canREG3, Internal_Lbk);
    canEnableloopback(canREG4, Internal_Lbk);

    /** - enabling error interrupts */
    canEnableErrorNotification(canREG1);
	canEnableErrorNotification(canREG2);
    canEnableErrorNotification(canREG3);
	canEnableErrorNotification(canREG4);

    canTransmit(canREG1, canMESSAGE_BOX1, (const uint8 *) &tx_data1[0]);
    canTransmit(canREG2, canMESSAGE_BOX1, (const uint8 *) &tx_data2[0]);
    canTransmit(canREG3, canMESSAGE_BOX1, (const uint8 *) &tx_data3[0]);
    canTransmit(canREG4, canMESSAGE_BOX1, (const uint8 *) &tx_data4[0]);

    while(1){};
/* USER CODE END */
}

/* USER CODE BEGIN (4) */


/* can interrupt notification */
void canMessageNotification(canBASE_t *node, uint32 messageBox)
{
     if(node==canREG1)
     {
         canGetData(canREG1, canMESSAGE_BOX2, (uint8 * )&rx_data1[0]); /* copy to RAM */
     }
     if(node==canREG2)
     {
         canGetData(canREG2, canMESSAGE_BOX2, (uint8 * )&rx_data2[0]); /* copy to RAM */
     }
     if(node==canREG3)
     {
         canGetData(canREG3, canMESSAGE_BOX2, (uint8 * )&rx_data3[0]); /* copy to RAM */
     }
     if(node==canREG4)
     {
         canGetData(canREG4, canMESSAGE_BOX2, (uint8 * )&rx_data4[0]); /* copy to RAM */
     }
}
/* USER CODE END */
