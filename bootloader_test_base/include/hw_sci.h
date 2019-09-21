/** @file sci.h
*   @brief SCI Driver Definition File
*   @date 14.September.2009
*   @version 1.01.000
*   
*/

/* (c) Texas Instruments 2009-2010, All rights reserved. */


#ifndef __SCI_H__
#define __SCI_H__

#include "sys_common.h"

/** @enum testStatus
*   @brief pass or fail status for the test performed
*   This enumeration is used to provide alias names for the test pass/fail status:
*     - PASS
*     - Fail 
*/

enum testStatus
{
    FAIL = 0x00000000, /**< Alias for FAIL */
    PASS = 0x00000001 /**< Alias for PASS */
};

/** @enum sciIntFlags
*   @brief Interrupt Flag Definitions
*
*   Used with sciEnableNotification, sciDisableNotification
*/
enum sciIntFlags
{
    SCI_FE_INT    = 0x04000000,  /* framming error */
    SCI_OE_INT    = 0x02000000,  /* overrun error */
    SCI_PE_INT    = 0x01000000,  /* parity error */
    SCI_RX_INT    = 0x00000200,  /* receive buffer ready */
    SCI_TX_INT    = 0x00000100,  /* transmit buffer ready */
    SCI_WAKE_INT  = 0x00000002,  /* wakeup */
    SCI_BREAK_INT = 0x00000001   /* break detect */
};


/** @struct sciBase
*   @brief SCI Register Definition
*
*   This structure is used to access the SCI module egisters.
*/
/** @typedef sciBASE_t
*   @brief SCI Register Frame Type Definition
*
*   This type is used to access the SCI Registers.
*/
typedef volatile struct sciBase
{
    unsigned GCR0;          /**< 0x0000 Global Control Register 0 */
    unsigned GCR1;          /**< 0x0004 Global Control Register 1 */
    unsigned GCR2;          /**< 0x0008 Global Control Register 2 */
    unsigned SETINT;        /**< 0x000C Set Interrupt Enable Register */
    unsigned CLRINT;        /**< 0x0010 Clear Interrupt Enable Register */
    unsigned SETINTLVL;     /**< 0x0014 Set Interrupt Level Register */
    unsigned CLRINTLVL;     /**< 0x0018 Set Interrupt Level Register */
    unsigned FLR;           /**< 0x001C Interrupt Flag Register */
    unsigned INTVECT0;      /**< 0x0020 Interrupt Vector Offset 0 */
    unsigned INTVECT1;      /**< 0x0024 Interrupt Vector Offset 1 */
    unsigned LENGTH;        /**< 0x0028 Format Control Register */
    unsigned BAUD;          /**< 0x002C Baud Rate Selection Register */
#if defined (_TMS470_LITTLE) || defined(__little_endian__)
    unsigned char ED;       /**< 0x0037 Receive Data Buffer */
    unsigned : 24U;
    unsigned char RD;       /**< 0x0033 Emulation Register */
    unsigned : 24U;
    unsigned char TD;       /**< 0x003B Transmit Data Buffer */
    unsigned : 24U;
#else
    unsigned : 24U;
    unsigned char ED;       /**< 0x0033 Emulation Register */
    unsigned : 24U;
    unsigned char RD;       /**< 0x0037 Receive Data Buffer */
    unsigned : 24U;
    unsigned char TD;       /**< 0x003B Transmit Data Buffer */
#endif
    unsigned FUN;           /**< 0x003C Pin Function Register */
    unsigned DIR;           /**< 0x0040 Pin Direction Register */
    unsigned DIN;           /**< 0x0044 Pin Data In Register */
    unsigned DOUT;          /**< 0x0048 Pin Data Out Register */
    unsigned SET;           /**< 0x004C Pin Data Set Register */
    unsigned CLR;           /**< 0x0050 Pin Data Clr Register */
    unsigned ODR;           /**< 0x0054: Pin Open Drain Output Enable Register */
    unsigned PD;            /**< 0x0058: Pin Pullup/Pulldown Disable Register */
    unsigned PSL;           /**< 0x005C: Pin Pullup/Pulldown Selection Register */
} sciBASE_t;


/** @def sciREG1
*   @brief SCI1 Register Frame Pointer
*
*   This pointer is used by the SCI driver to access the sci module registers.
*/
#define sciREG1 ((sciBASE_t *)0xFFF7E400U)


/** @def sciPORT1
*   @brief SCI1 GIO Port Register Pointer
*
*   Pointer used by the GIO driver to access I/O PORT of SCI1
*   (use the GIO drivers to access the port pins).
*/
#define sciPORT1 ((gioPORT_t *)0xFFF7E440U)


/** @def sciREG2
*   @brief SCI2 Register Frame Pointer
*
*   This pointer is used by the SCI driver to access the sci module registers.
*/
#define sciREG2 ((sciBASE_t *)0xFFF7E500U)


/** @def sciPORT2
*   @brief SCI2 GIO Port Register Pointer
*
*   Pointer used by the GIO driver to access I/O PORT of SCI2
*   (use the GIO drivers to access the port pins).
*/
#define sciPORT2 ((gioPORT_t *)0xFFF7E540U)


/* SCI Interface Functions */
void sciInit(void);
void sciSetFunctional(sciBASE_t *sci, unsigned port);
void sciSetBaudrate(sciBASE_t *sci, unsigned baud);
int  sciIsTxReady(sciBASE_t *sci);
void sciSendByte(sciBASE_t *sci, unsigned char byte);
void sciSend(sciBASE_t *sci, unsigned length, unsigned char *data);
int  sciIsRxReady(sciBASE_t *sci);
int  sciRxError(sciBASE_t *sci);
int  sciReceiveByte(sciBASE_t *sci);
void sciReceive(sciBASE_t *sci, unsigned length, unsigned char *data);
void sciEnableNotification(sciBASE_t *sci, unsigned flags);
void sciDisableNotification(sciBASE_t *sci, unsigned flags);

/** @fn void sciNotification(sciBASE_t *sci, unsigned flags)
*   @brief Interrupt callback
*   @param[in] sci   - sci module base address
*   @param[in] flags - copy of error interrupt flags
*
* This is a callback that is provided by the application and is called apon
* an interrupt.  The parameter passed to the callback is a copy of the 
* interrupt flag register.
*/
void sciNotification(sciBASE_t *sci, unsigned flags);

#endif
