//*****************************************************************************
//
// bl_can.c    : Functions to transfer data via the CAN port.
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

#include "hw_can.h"
#include "hw_gio.h"
#include "hw_sci.h"
#include "bl_config.h"
#include "bl_check.h"
#include "bl_flash.h"
#include "bl_can.h"
#include "bl_commands.h"
#include "F021.h"
#include "system.h"
#include "sci_common.h"
#include "bl_led_demo.h"


#if defined (CAN_ENABLE_UPDATE)

extern void delay();
void  ConfigureCANDevice(canBASE_t *);

/* Global and Static Variables */
#ifndef __little_endian__
    static const uint32_t s_canByteOrder[] = {3U, 2U, 1U, 0U, 7U, 6U, 5U, 4U};
#else
    static const uint32_t s_canByteOrder[] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U};
#endif

//*****************************************************************************
//
// The results that can be returned by the CAN APIs.
//
//*****************************************************************************
#define CAN_CMD_SUCCESS         0x00
#define CAN_CMD_FAIL            0x01


//*****************************************************************************
//
// The message object number and index to the local message object memory to
// use when accessing the messages.
//
//*****************************************************************************
#define MSG_OBJ_BCAST_RX_ID     1
#define MSG_OBJ_BCAST_TX_ID     2

//*****************************************************************************
//
// Holds the current address to write to when data is received via the Send
// Data Command.
//
//*****************************************************************************
extern uint32_t g_ulTransferAddress;

//*****************************************************************************
//
// Holds the remaining bytes expected to be received.
//
//*****************************************************************************
extern uint32_t g_ulTransferSize;

//*****************************************************************************
//
// The buffer used to receive data from the update.
//
//*****************************************************************************
static uint8_t *g_pucCommandBuffer;

//*****************************************************************************
//
// These globals are used to store the first two words to prevent a partial
// image from being booted.
//
//*****************************************************************************
//extern uint8_t *g_pucDataBuffer;
extern uint32_t g_pulDataBuffer[BUFFER_SIZE];
extern uint32_t g_pulUpdateSuccess[8];
extern uint32_t g_ulUpdateStatusAddr;
extern uint32_t g_ulUpdateBufferSize;  //32 bytes or 8 32-bit words

//*****************************************************************************
//
// Initializes the CAN controller after reset.
//
// After reset, the CAN controller is left in the disabled state.  However,
// the memory used for message objects contains undefined values and must be
// cleared prior to enabling the CAN controller the first time.  This prevents
// unwanted transmission or reception of data before the message objects are
// configured.  This function must be called before enabling the controller
// the first time.
//
// return None.
//
//*****************************************************************************
static void
CANInit(canBASE_t *node)
{
    uint32_t iMsg;

    //
    // Place CAN controller in init state, regardless of previous state.  This
    // will put the controller in idle, and allow the message object RAM to be
    // programmed.
    //
    node->CTL = 0x00000000U | 0x00000000U | 0x00021443U;  /* both IE0, IE1 are 1*/

    /** - Clear all pending error flags and reset current status */
    node->ES = 0x0000031FU;

    /** - Assign interrupt level for messages */
    node->INTMUXx[0U] = 0x00000011U;
    node->INTMUXx[1U] = 0x00000000U;

    /** - Setup auto bus on timer pewriod */
    node->ABOTR = 0U;

    //
    // Loop through to program all 32 message objects
    //

    for(iMsg = 1; iMsg <= 64; iMsg++)
    {
        while (node->IF1STAT & 0x80);

        node->IF1CMD  = (CAN_IFCMD_WRNRD | CAN_IFCMD_ARB | CAN_IFCMD_CONTROL);
        node->IF1ARB  = 0x0;
        node->IF1MCTL = 0x0;
        node->IF1NO   = iMsg;
    }

    // Baud rate = 1000/[(Tseg1+Tseg2+1)*(15+1)/fosc]=1000/(8*16/16)=125khz
#if defined(TMS570LS04) || defined(RM42)
    //assume the VCLK is 40MHz
    if (CAN_BIT_RATE == 125000)
		node->BTR =      (0U 			 << 16U) |
						((3U - 1U) 	     << 12U) |
					   (((3U + 1U) - 1U) << 8U)  |
					    ((3U - 1U) 	     << 6U)  |
					     39U;
    else if (CAN_BIT_RATE == 250000)
         node->BTR =     (0U 			 << 16U) |
                        ((6U - 1U) 	     << 12U) |
                       (((3U + 6U) - 1U) << 8U)  |
                        ((4U - 1U) 		 << 6U)  |
                         9U;
    else if (CAN_BIT_RATE == 500000)
         node->BTR =     (0U             << 16U) |
                        ((2U - 1U)       << 12U) |
                       (((3U + 2U) - 1U) << 8U)  |
                        ((2U - 1U)       << 6U)  |
                         9U;

    else if (CAN_BIT_RATE == 750000)
         node->BTR =    (0U 			 << 16U) |
                       ((2U - 1U) 		 << 12U) |
                       (((6U + 2U) - 1U) << 8U)  |
                       ((2U - 1U) 		 << 6U)  |
                         4U;
    else if (CAN_BIT_RATE == 1000000)
         node->BTR =     (0U 			 << 16U) |
                        ((1U - 1U) 		 << 12U) |
                       (((7U + 1U) - 1U) << 8U)  |
                        ((1U - 1U) 		 << 6U)  |
                          3U;
#else
    //assume the VCLK is 80MHz
    if (CAN_BIT_RATE == 125000)
		node->BTR =      (1U 			 << 16U) |
						((3U - 1U) 	     << 12U) |
					   (((3U + 1U) - 1U) << 8U)  |
					    ((3U - 1U) 	     << 6U)  |
					     15U;
    else if (CAN_BIT_RATE == 250000)
         node->BTR =     (0U 			 << 16U) |
                        ((6U - 1U) 	     << 12U) |
                       (((3U + 6U) - 1U) << 8U)  |
                        ((4U - 1U) 		 << 6U)  |
                         19U;
    else if (CAN_BIT_RATE == 500000)
         node->BTR =     (0U             << 16U) |
                        ((2U - 1U)       << 12U) |
                       (((3U + 2U) - 1U) << 8U)  |
                        ((2U - 1U)       << 6U)  |
                         19U;

    else if (CAN_BIT_RATE == 750000)
         node->BTR =    (0U 			 << 16U) |
                       ((2U - 1U) 		 << 12U) |
                       (((7U + 2U) - 1U) << 8U)  |
                       ((2U - 1U) 		 << 6U)  |
                         8U;
    else if (CAN_BIT_RATE == 1000000)
         node->BTR =     (0U 			 << 16U) |
                        ((1U - 1U) 		 << 12U) |
                       (((7U + 1U) - 1U) << 8U)  |
                        ((1U - 1U) 		 << 6U)  |
                          7U;
#endif


    node->TIOC = 0x0000004CU;
    node->RIOC = 0x00000048U;
    node->CTL &= ~0x00000041U;

}

//*****************************************************************************
//
//! This function configures the message object used to receive commands.
//!
//! This function configures the message object used to receive all firmware
//! update messages.  This will not actually read the data from the message it
//! is used to prepare the message object to receive the data when it is sent.
//!
//! \return None.
//
//*****************************************************************************
static void
CANMessageSetRx(canBASE_t *node)
{
    uint8_t usCmdReg;
    uint32_t usMaskReg;
    uint32_t usArbReg;
    uint32_t usMsgCtrl;

    //
    // Wait for busy bit to clear
    //
    while(node->IF1STAT & CAN_IFCMD_BUSY)
    {
    }

    //
    // This is always a write to the Message object as this call is setting a
    // message object.  This call will also always set all size bits so it sets
    // both data bits.  The call will use the CONTROL register to set control
    // bits so this bit needs to be set as well.
    //
    // Set the MASK bit so that this gets transferred to the Message Object.
    // Set the Arb bit so that this gets transferred to the Message object.
    //
    usCmdReg = (CAN_IFCMD_WRNRD | CAN_IFCMD_DATAA | CAN_IFCMD_DATAB |
                    CAN_IFCMD_CONTROL | CAN_IFCMD_MASK | CAN_IFCMD_ARB);

    //
    // Set the UMASK bit to enable using the mask register.
    // Set the data length since this is set for all transfers.  This is also a
    // single transfer and not a FIFO transfer so set EOB bit.
    //
    usMsgCtrl = CAN_IFMCTL_UMASK | CAN_IFMCTL_EOB;

    //
    // Configure the Mask Registers.
    //
    usMaskReg = CAN_API_UPD << 18;

    //arb reg
    usArbReg = ((CAN_API_UPD << 18)& CAN_IFARB_ID_M) | (CAN_IFARB_MSGVAL);

    //
    // Write out the registers to program the message object.
    //
    node->IF1CMD  = usCmdReg;
    node->IF1MSK  = usMaskReg;
    node->IF1ARB  = usArbReg;
    node->IF1MCTL = usMsgCtrl;

    //
    // Transfer the message object to the message object specific by ID
    //
    node->IF1NO = (MSG_OBJ_BCAST_RX_ID & CAN_IFCMD_MNUM_M);  //1

}

//*****************************************************************************
//
//! This function reads data from the receive message object.
//!
//! \param pucData is a pointer to the buffer to store the data read from the
//! CAN controller.
//! \param pulMsgID is a pointer to the ID that was received with the data.
//!
//! This function will reads and acknowledges the data read from the message
//! object used to receive all CAN firmware update messages.  It will also
//! return the message identifier as this holds the API number that was
//! attached to the data.  This message identifier should be one of the
//! CAN_COMMAND_* definitions.
//!
//! \return The number of valid bytes returned in the \e pucData buffer or
//! 0xffffffff if data was overwritten in the buffer.
//
//*****************************************************************************
static uint32_t
CANMessageGetRx(canBASE_t *node, uint8_t *pucData, uint32_t *pulMsgID)
{
    uint8_t usCmdReg;
    uint32_t usArbReg;
    uint32_t usMsgCtrl;
    uint32_t ulBytes;
    uint8_t *pusData;
    uint32_t i;

    //
    // This is always a read to the Message object as this call is setting a
    // message object.
    // Clear a pending interrupt and new data in a message object.
    //
    usCmdReg = (CAN_IFCMD_DATAA | CAN_IFCMD_DATAB |
                    CAN_IFCMD_CONTROL | CAN_IFCMD_CLRINTPND |
                    CAN_IFCMD_ARB);

    //
    // Set up the request for data from the message object.
    //
    node->IF1CMD  = usCmdReg;

    //
    // Transfer the message object to the message object specific by
    // MSG_OBJ_BCAST_RX_ID.
    //
    node->IF1NO = (MSG_OBJ_BCAST_RX_ID & CAN_IFCMD_MNUM_M);  //msg box 1


    //
    // Wait for busy bit to clear
    //
    while(node->IF1STAT & CAN_IFCMD_BUSY)
    {
    }

    //
    // Read out the IF Registers.
    //
    usArbReg = node->IF1ARB;
    usMsgCtrl = node->IF1MCTL;

    //
    // Set the 29 bit version of the Identifier for this message object.
    //
    *pulMsgID = (usArbReg & CAN_IFARB_11ID_M) >> 18;

    //
    // See if there is new data available.
    //
    if((usMsgCtrl & (CAN_IFMCTL_NEWDAT | CAN_IFMCTL_MSGLST)) == CAN_IFMCTL_NEWDAT)
    {
        //
        // Get the amount of data needed to be read.
        //
        ulBytes = (usMsgCtrl & CAN_IFMCTL_DLC_M) ;

        //
        // Read out the data from the CAN registers 16 bits at a time.
        //
        pusData = (uint8_t*)pucData;

        /** - Copy RX data into destination buffer */
        for (i = 0U; i < ulBytes; i++)
        {
#ifdef __little_endian__
            *pusData++ = node->IF1DATx[i];
#else
            *pusData++ = node->IF1DATx[s_canByteOrder[i]];
#endif
        }

        //
        // Now clear out the new data flag.
        //
        node->IF1CMD =  CAN_IFCMD_NEWDAT;

        //
        // Transfer the message object to the message object specific by
        // MSG_OBJ_BCAST_RX_ID.
        //
        node->IF1NO = MSG_OBJ_BCAST_RX_ID;

        //
        // Wait for busy bit to clear
        //
        while(node->IF1STAT & CAN_IFCMD_BUSY)
        {
        }
    }
    else
    {
        //
        // Data was lost so inform the caller.
        //
        ulBytes = 0xffffffff;
    }
    return(ulBytes);
}

//*****************************************************************************
//
//! This function sends data using the transmit message object.
//!
//! \param ulId is the ID to use with this message.
//! \param pucData is a pointer to the buffer with the data to be sent.
//! \param ulSize is the number of bytes to send and should not be more than
//! 8 bytes.
//!
//! This function will reads and acknowledges the data read from the message
//! object used to receive all CAN firmware update messages.  It will also
//! return the message identifier as this holds the API number that was
//! attached to the data.  This message identifier should be one of the
//! CAN_COMMAND_* definitions.
//!
//! \return None.
//
//*****************************************************************************
static void
CANMessageSetTx(canBASE_t *node, uint32_t ulId, const uint8_t *pucData,
                uint32_t ulSize)
{
    uint8_t usCmdReg;
    uint32_t usArbReg, i;
    uint32_t usMsgCtrl;

    //
    // Wait for busy bit to clear
    //
    while(node->IF1STAT & CAN_IFCMD_BUSY)
    {
    }

    //
    // This is always a write to the Message object as this call is setting a
    // message object.  This call will also always set all size bits so it sets
    // both data bits.  The call will use the CONTROL register to set control
    // bits so this bit needs to be set as well.
    //
    usCmdReg = (CAN_IFCMD_WRNRD | CAN_IFCMD_DATAA | CAN_IFCMD_DATAB |
                    CAN_IFCMD_CONTROL | CAN_IFCMD_ARB);

    //
    // Mark the message as valid and set the extended ID bit.
    //
    usArbReg = (((ulId << 18) & CAN_IFARB_11ID_M) | (CAN_IFARB_DIR | CAN_IFARB_MSGVAL));

    //
    // Set the TXRQST bit and the reset the rest of the register.
    // Set the data length since this is set for all transfers.  This is also a
    // single transfer and not a FIFO transfer so set EOB bit.
    //
    //
    usMsgCtrl = (CAN_IFMCTL_TXRQST | CAN_IFMCTL_EOB | (ulSize & CAN_IFMCTL_DLC_M));

    //
    // Write the data out to the CAN Data registers if needed.
    //
    for (i = 0U; i < 8U; i++)
    {
#ifdef __little_endian__
        node->IF1DATx[i] = *pucData++;
#else
        node->IF1DATx[s_canByteOrder[i]] = *pucData++;
#endif
     }

    //
    // Write out the registers to program the message object.
    //
    node->IF1CMD = usCmdReg;
    node->IF1ARB = usArbReg;
    node->IF1MCTL = usMsgCtrl;

    //
    // Transfer the message object to the message object specifiec by
    // MSG_OBJ_BCAST_RX_ID.
    //
    node->IF1NO = (MSG_OBJ_BCAST_TX_ID) & (CAN_IFCMD_MNUM_M);  //msg box 2
}

//*****************************************************************************
//
// Reads the next packet that is sent to the boot loader.
//
//*****************************************************************************
static uint32_t
PacketRead(canBASE_t *node, uint8_t *pucData, uint32_t *pulSize)
{
    uint32_t ulMsgID;

    uint32_t messageBox = 1;

    uint32_t regIndex = (messageBox - 1U) >> 5U;
    uint32_t bitIndex = 1U << ((messageBox - 1U) & 0x1FU);

    //
    // Wait until a packet has been received.
    //
    while((node->NWDATx[regIndex] & bitIndex) == 0)
    {
    }

    //
    // Read the packet.
    //
    *pulSize = CANMessageGetRx(node, pucData, &ulMsgID);

    //
    // Return the message ID of the packet that was received.
    //
    return(ulMsgID);
}

//*****************************************************************************
//
// Sends a packet to the controller that is communicating with the boot loader.
//
//*****************************************************************************
static void
PacketWrite(canBASE_t *node, uint32_t ulId, const uint8_t *pucData, uint32_t ulSize)
{
    uint32_t ulIdx;

    uint32_t messageBox = 2;

    uint32_t regIndex = (messageBox - 1U) >> 5U;
    uint32_t bitIndex = 1U << ((messageBox - 1U) & 0x1FU);

    //
    // Wait until the previous packet has been sent, providing a time out so
    // that the boot loader does not hang here.
    //
    for(ulIdx = 1000; (ulIdx != 0) && ((node->TXRQx[regIndex] & bitIndex) != 0); ulIdx--)
    {
    }

    //
    // If the previous packet was sent, then send this packet.
    //
    if(ulIdx != 0)
    {
        CANMessageSetTx(node, ulId, pucData, ulSize);
    }
}

//*****************************************************************************
//
// This is the main routine for handling updating over CAN.
//
// This function accepts boot loader commands over CAN to perform a firmware
// update over the CAN bus.  This function assumes that the CAN bus timing
// and message objects have been configured elsewhere.
//
// \return None.
//
//*****************************************************************************
void
UpdaterCAN(canBASE_t *node)
{
    uint32_t ulBytes, ulCmd;
    uint32_t FLASH_FIRST_SECTOR_SIZE;
    uint32_t oReturnCheck, updateStatusBank;
	uint8_t ucUpdateStatus;
	uint8_t ucBank, ucStatus;
	uint32_t ulFlashSize;

    updateStatusBank = 0;
    ucUpdateStatus = 0;

    ucBank = 0;
    FLASH_FIRST_SECTOR_SIZE = BLInternalFlashFirstSectorSizeGet();

    // This ensures proper alignment of the global buffer so that the one byte
    // size parameter used by the packetized format is easily skipped for data
    // transfers.
    g_pucCommandBuffer = ((uint8_t *)g_pulDataBuffer);

    // Insure that the COMMAND_SEND_DATA cannot be sent to erase the boot
    // loader before the application is erased.
    g_ulTransferAddress = 0xffffffff;

    // Read any data from the serial port in use.

#ifdef ENABLE_UPDATE_CHECK
    //
    // Check the application is valid and check the pin to see if an update is
    // being requested.
    //
    if(g_ulForced == 1)
    {
        //
        // Send out the CAN request.
        //
        PacketWrite(node, CAN_COMMAND_REQUEST, 0, 0);
//        PacketWrite(node, 0x7F5, 0, 0);


        //
        // Wait only 50ms for the response and move on otherwise.
        //
        delay();

        //
        // Wait until a packet has been received.
        //
        if(node->NWDATx[0] == 0)
        {
            //
            // Call the application.
            //
           // StartApplication();
        }
    }
#endif

    //
    // Loop forever processing packets.
    //

    while(1)
    {
    	LITE_TOPRIGHT_LED;

		ulBytes = 0;
		ulCmd = PacketRead(node, g_pucCommandBuffer, &ulBytes);

			//
			// Handle this packet.
			//
		ucStatus = CAN_CMD_SUCCESS;       // 0x00
		switch(ulCmd)
		{
				//
			// This is an update request packet.
			case CAN_COMMAND_REQUEST:
			{
				PacketWrite(node, CAN_COMMAND_ACK, &ucStatus, 1);
				break;
			}

            //
            // This is a ping packet.
            // This command is used to receive an acknowledge command
            // from the boot loader indicating that communication has been established.
            // This command has no data. If the device is present it
            // will respond with a CAN_COMMAND_PING back to the CAN update application.
            // CAN_COMMAND_PING = 0x1F02,0000
            // ACK = 0x1F02,0100
            case CAN_COMMAND_PING:
            {
                PacketWrite(node, CAN_COMMAND_ACK, &ucStatus, 1);
                break;
            }

            case CAN_COMMAND_RUN: /*0x02*/
            {
                // Acknowledge that this command was received correctly.  This
                // does not indicate success, just that the command was
                // received.

                PacketWrite(node, CAN_COMMAND_ACK, &ucStatus, 1);

               // Get the address to which control should be transferred.
                g_ulTransferAddress = 0|(g_pucCommandBuffer[0]<<24)|(g_pucCommandBuffer[1]<<16)|(g_pucCommandBuffer[2]<<8)|(g_pucCommandBuffer[3]<<0);

                // This determines the size of the flash available on the
                // device in use.
                ulFlashSize = 0x300000;  //3MB, need to check the register to get the size automatically

                // Test if the transfer address is valid for this device.
                if(g_ulTransferAddress >= ulFlashSize)
                {
                    // Indicate that an invalid address was specified.
                	ucStatus = CAN_CMD_FAIL;

                    // This packet has been handled.
                    break;
                }

                // Branch to the specified address.  This should never return.
                // If it does, very bad things will likely happen since it is
                // likely that the copy of the boot loader in SRAM will have
                // been overwritten.

                ((void (*)(void))g_ulTransferAddress)();

                // In case this ever does return and the boot loader is still
                // intact, simply reset the device.
                // Use the reset in SYSECR register.
                systemREG1->SYSECR = (0x10) << 14;

                // The microcontroller should have reset, so this should
                // never be reached.  Just in case, loop forever.
                while(1)
                {
                }
            }

            //
            // This is a reset packet.
            //
            case CAN_COMMAND_RESET:
            {
                //
                // Perform a software reset request.  This will cause the
                // microcontroller to reset; no further code will be executed.
                //
                // Use the reset function in the flash patch if appropriate.
                //
                PacketWrite(node, CAN_COMMAND_ACK, &ucStatus, 1);

                //
                // Perform a software reset request.  This will cause the
                // microcontroller to reset; no further code will be executed.
                //
                // Use the reset in SYSECR register.
                //
                systemREG1->SYSECR = (0x10) << 14;

                //
                // The microcontroller should have reset, so this should never
                // be reached.  Just in case, loop forever.
                //
                while(1)
                {
                }
                break;
            }

            //
            // This is a data packet.
            //
            case CAN_COMMAND_SEND_DATA:
            {

            	LITE_BOTRIGHT_LED;

            	//
                // If this is overwriting the boot loader then the application
                // has already been erased so now erase the boot loader.
                //
                if(g_ulTransferAddress == 0)
                {
						/* Initialize the Flash Wrapper registers */
						oReturnCheck = 0;

						oReturnCheck = Fapi_BlockErase( 0, 0, FLASH_FIRST_SECTOR_SIZE);

						// Return an error if an access violation occurred.
						if(oReturnCheck)
						{
							// Setting g_ulTransferSize to zero makes
							// COMMAND_SEND_DATA fail to accept any more data.
							g_ulTransferSize = 0;

							// Indicate that the flash erase failed.
							ucStatus = CAN_CMD_FAIL;
						}
                }

                // Check if there are any more bytes to receive.
                if(g_ulTransferSize >= ulBytes)
                {
						  /* Initialize the Flash Wrapper registers */
						oReturnCheck = 0;
     					oReturnCheck = Fapi_BlockProgram( ucBank, g_ulTransferAddress, (uint32_t)&g_pulDataBuffer[0], ulBytes);

						// Return an error if an access violation occurred.
						if(oReturnCheck)
						{
							// Indicate that the flash programming failed.
							ucStatus = CAN_CMD_FAIL;
							ucUpdateStatus = 0;
							UART_putString(UART, "\r Program Flash failed:  ");
						}
						else
						{
							// Now update the address to program.
							g_ulTransferSize -= ulBytes;
							g_ulTransferAddress += ulBytes;
							ucUpdateStatus = 1;
						}
                }
                else
                {
						// This indicates that too much data is being sent to the device.
						ucStatus = CAN_CMD_FAIL;
						ucUpdateStatus = 0;
                }

                if (g_ulTransferSize == 0)
                {
                	if( ucUpdateStatus == 1)
                	{
                		oReturnCheck = Fapi_UpdateStatusProgram( updateStatusBank, g_ulUpdateStatusAddr, (uint32_t)&g_pulUpdateSuccess[0], g_ulUpdateBufferSize);
                		UART_putString(UART, "\r Application was loaded successful!  ");
                	}
            	}

                // Acknowledge that this command was received correctly.  This
                // does not indicate success, just that the command was received.
                PacketWrite(node, CAN_COMMAND_ACK, &ucStatus, 1);
               // Go back and wait for a new command.
                break;
            }

            //
            // This is a start download packet.
            //
            case CAN_COMMAND_DOWNLOAD:
            {
            	LITE_BOTLEFT_LED;

                 // A simple do/while(0) control loop to make error exits easier.
                 do
                 {
                     // See if a full packet was received.
                     if(ulBytes != 8)
                     {
                         //
                         // Set the code to an error to indicate that the last
                         // command failed.  This informs the updater program
                         // that the download command failed.
                         //
                         ucStatus = CAN_CMD_FAIL;

                         // This packet has been handled.
                         break;
                     }

                     // Get the address and size from the command.
                     // where to swap the bytes?
                     // The data is transferred most significant bit (MSB) first. This is used for RM48 which is little endian device
                     g_ulTransferAddress = 0|(g_pucCommandBuffer[0]<<24)|(g_pucCommandBuffer[1]<<16)|(g_pucCommandBuffer[2]<<8)|(g_pucCommandBuffer[3]<<0);
                     g_pulUpdateSuccess[1]= g_ulTransferAddress;

                     //Tell bootloader how many bytes the host will transfer for the whole application
                     g_ulTransferSize = 0|(g_pucCommandBuffer[4]<<24)|(g_pucCommandBuffer[5]<<16)|(g_pucCommandBuffer[6]<<8)|(g_pucCommandBuffer[7]<<0);
                     g_pulUpdateSuccess[2]= g_ulTransferSize;

                     // Check for a valid starting address and image size.
                     if(!BLInternalFlashStartAddrCheck(g_ulTransferAddress,  g_ulTransferSize))
                     {
                         // Set the code to an error to indicate that the last
                         // command failed.  This informs the updater program
                         // that the download command failed.
                         ucStatus = CAN_CMD_FAIL;

                         // This packet has been handled.
                         break;
                     }

                     /* Initialize the Flash Wrapper registers */
                     oReturnCheck = 0;
                     oReturnCheck = Fapi_BlockErase( ucBank, g_ulTransferAddress, g_ulTransferSize);

                     // Return an error if an access violation occurred.
                     //
                     if(oReturnCheck)
                     {
                     	ucStatus = CAN_CMD_FAIL;
                     }
                 }
                 while(0);

                 //
                 // See if the command was successful.
                 //
                 if(ucStatus != CAN_CMD_SUCCESS)
                 {
                     //
                     // Setting g_ulTransferSize to zero makes COMMAND_SEND_DATA
                     // fail to accept any data.
                     //
                     g_ulTransferSize = 0;
                 }

                 //
                 // Acknowledge that this command was received correctly.  This
                 // does not indicate success, just that the command was
                 // received.
                 //
                 PacketWrite(node, CAN_COMMAND_ACK, &ucStatus, 1);
                 //
                 // Go back and wait for a new command.
                 //
                 break;

            }

            //
            // This is an unknown packet.
            //
            default:
            {
                //
                // Set the status to indicate a failure.
                //
                ucStatus = CAN_CMD_FAIL;
                break;
            }
        }
    }
}


//*****************************************************************************
//
//! Generic configuration is handled in this function.
//!
//! This function is called by the start up code to perform any configuration
//! necessary before calling the update routine.
//!
//! \return None.
//
//*****************************************************************************
void
ConfigureCANDevice(canBASE_t *node)
{
   //
    // Init the CAN controller.
    //
	CANInit(node);

    //
    // Configure the broadcast receive message object.
    //
    CANMessageSetRx(node);
}

#endif
