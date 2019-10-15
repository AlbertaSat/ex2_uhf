/** @example example_canIntCommunication.c
*   This example code configures two CAN nodes and emulates a data transfer
*   from CAN1 to CAN2.
* 
*   node1: CAN1 [MSG BOX 1 (ID1)]-> TX
*   node2: CAN2 [MSG BOX 1 (ID1)]<- RX
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
*   - Enable CAN driver
*   - Disable others
*
*   Navigate: -> TMS570LSxx /RM4 -> Enable Drivers
*
*   @image html can_enabledriver.jpg "Figure: CAN 1,2 Driver Enable"
*
*   @b Step @b 3:
*
*   Configure CAN Message Box: 
*   - Configure CAN1 Message box 1 as transmit
*   Navigate: -> TMS570LSxx /RM4 -> CAN
*
*   @image html can1box.jpg "Figure: CAN1 MBox Configuration"
*
*   - Configure CAN2 Message box 1 as receive
*
*   @image html can2box.jpg "Figure: CAN2 MBox Configuration"
*
*   - Enable CAN1 High level interrupt and CAN2 High level interrupt  
*
*   @b Step @b 4:
*
*   Enable CAN interrupts in VIM: 
*
*   Navigate: -> TMS570LSxx /RM4 -> VIM
*
*   @image html can1_Int.jpg "Figure: VIM Configuration"
*   @image html can2_Int.jpg "Figure: VIM Configuration"
*
*   @b Step @b 5:
*
*   Copy the source code below into your sys_main.c (or) replace sys_main.c with this file.
*   Copy dma.c and dma.h into your application.
*
*   Execution:
*   The CAN1 and CAN2 communication line needs to be connected on to the live CAN bus.
* 
*   The example file can also be found in the examples folder: ../HALCoGen/examples
*
*   @note HALCoGen generates an enpty main function in sys_main.c. 
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
#include "esm.h"
#include "sys_core.h"

#define D_COUNT  8 

uint32 cnt=0, error =0, tx_done =0;
uint8 tx_data[D_COUNT][8] = {0};
uint8 rx_data[D_COUNT][8] = {0};
uint8 *tx_ptr = &tx_data[0][0];
uint8 *rx_ptr = &rx_data[0][0];
uint8 *dptr=0;

void dumpSomeData();
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

    /* enable irq interrupt in Cortex R4 */
    _enable_interrupt_();

    /** - writing a random data in RAM - to transmit */
    dumpSomeData();
        
    /** - configuring CAN1 MB1,Msg ID-1 to transmit and CAN2 MB1 to receive */
    canInit();

    /** - enabling error interrupts */    
    canEnableErrorNotification(canREG1);
	canEnableErrorNotification(canREG2);
    
    /** - starting transmission */
     for(cnt=0;cnt<D_COUNT;cnt++)
    {         
      canTransmit(canREG1, canMESSAGE_BOX1, tx_ptr); /* transmitting 8 different chunks 1 by 1 */
      while(tx_done == 0){};                 /* ... wait until transmit request is through        */
      tx_done=0;
      tx_ptr +=8;    /* next chunk ...*/
    }
    
    /** - check the received data with the one that was transmitted */
    tx_ptr = &tx_data[0][0];
    rx_ptr = &rx_data[0][0];
        
    for(cnt=0;cnt<63;cnt++)
     {
          if(*tx_ptr++ != *rx_ptr++)
          {
               error++; /* data error */
          }
     }

    while(1){}; /* wait forever after tx-rx complete. */
	
/* USER CODE END */	
}

/* USER CODE BEGIN (4) */
/* writing some data to ram  */
void dumpSomeData()
{
     uint32 tmp = 0x11;
     
     cnt = (D_COUNT*8)-1;
     dptr = &tx_data[0][0];
     *dptr = tmp;

     while(cnt--)
     {
        tmp = *dptr++;
        *dptr = tmp + 0x11;
     }
}


/* can interrupt notification */
/* Note-You need to remove canMessageNotification from notification.c to avoid redefinition */
void canMessageNotification(canBASE_t *node, uint32 messageBox)
{
     /* node 1 - transfer request */
     if(node==canREG1) 
     {     
       tx_done=1; /* confirm transfer request */
     }

     /* node 2 - receive complete */     
     if(node==canREG2)
     {
      while(!canIsRxMessageArrived(canREG2, canMESSAGE_BOX1));
      canGetData(canREG2, canMESSAGE_BOX1, rx_ptr); /* copy to RAM */    
      rx_ptr +=8;     
     }     

    /* Note: since only message box 1 is used on both nodes we dont check it here.*/
}
/* USER CODE END */