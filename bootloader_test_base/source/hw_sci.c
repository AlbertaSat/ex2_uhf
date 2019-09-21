/** @file sci.c 
*   @brief SCI Driver Inmplmentation File
*   @date 10.June.2010
*   @version 1.01.000
*
*/

/* (c) Texas Instruments 2009-2010, All rights reserved. */


#include "hw_sci.h"
#include "bl_config.h"
#include "sys_common.h"


/** @struct g_sciTransfer
*   @brief Interrupt mode globals
*
*/
struct g_sciTransfer
{
    unsigned       mode;
    unsigned       length;
    uint8_t *data;
} g_sciTransfer[2];


/** @fn void sciInit(void)
*   @brief Initializes the SCI Driver
*
*   This function initializes the SCI module.
*/
void sciInit(void)
{
    /** @b intalise @b SCI1 */
    unsigned f;
    double   vclk = SYS_CLK_FREQ * 1000000 / 2;

    /** - bring SCI out of reset */
    sciREG1->GCR0 = 1U;

    /** - Disable all interrupts */
    sciREG1->CLRINT    = 0xFFFFFFFF;
    sciREG1->CLRINTLVL = 0xFFFFFFFF;

    /** - global control 1 */
    sciREG1->GCR1 = (1 << 25)  /* enable transmit */
                  | (1 << 24)  /* enable receive */
                  | (1 << 5)   /* internal clock (device has no clock pin) */
                  | (0 << 4)   /* number of stop bits */
                  | (0 << 3)   /* even parity, otherwise odd */
                  | (0 << 2)   /* enable parity */
                  | (1 << 1)   /* asynchronous timing mode */
                  | (0 << 0); 
                   
    /** - set baudrate */
//    sciREG1->BAUD = 42;      /* 115.20K baudrate for 160MHz Vclk */
//    sciREG1->BAUD = 259;      /* 19.2K baudrate for 160MHz Vclk */
//    sciREG1->BAUD = 155;      /* 19.2K baudrate for 96MHz Vclk */
    
    f = sciREG1->GCR1 & 2 ? 16 : 1;
    sciREG1->BAUD = ((unsigned)((vclk /(f*UART_BAUDRATE) + 0.5)) - 1) & 0x00FFFFFF;


    /** - tranmision length */
    sciREG1->LENGTH = 7;  /* length is 7+1 */

    /** - set SCI pins functional mode */
    sciREG1->FUN = (1 << 2)  /* tx pin */
                 | (1 << 1)  /* rx pin */
                 | (0);  /* clk pin */

    /** - set SCI pins default output value */
    sciREG1->DOUT = (0 << 2)  /* tx pin */
                  | (0 << 1)  /* rx pin */
                  | (0);  /* clk pin */

    /** - set SCI pins output direction */
    sciREG1->DIR = (0 << 2)  /* tx pin */
                 | (0 << 1)  /* rx pin */
                 | (0);  /* clk pin */

    /** - set SCI pins open drain enable */
    sciREG1->ODR = (0 << 2)  /* tx pin */
                 | (0 << 1)  /* rx pin */
                 | (0);  /* clk pin */

    /** - set SCI pins pullup/pulldown enable */
    sciREG1->PD = (1 << 2)  /* tx pin */
                | (1 << 1)  /* rx pin */
                | (0);  /* clk pin */

    /** - set SCI pins pullup/pulldown select */
    sciREG1->PSL = (1 << 2)  /* tx pin */
                 | (1 << 1)  /* rx pin */
                 | (1);  /* clk pin */

    /** - set interrupt level */
    sciREG1->SETINTLVL = (0 << 26)  /* Framing error */
                       | (0 << 25)  /* Overrun error */
                       | (0 << 24)  /* Pariry error */
                       | (0 << 9)   /* Receive */
                       | (0 << 8)   /* Transmit */
                       | (0 << 1)   /* Wakeup */
                       | (0);       /* Break detect */

    /** - set interrupt enable */
    sciREG1->SETINT = (0 << 26)  /* Framing error */
                    | (0 << 25)  /* Overrun error */
                    | (0 << 24)  /* Pariry error */
                    | (0 << 9)   /* Receive */
                    | (0 << 1)   /* Wakeup */
                    | (0);  /* Break detect */

    /** - inialise global transfer variables */
    g_sciTransfer[0].mode   = 0 << 8;
    g_sciTransfer[0].length = 0;

    /** - Finaly start SCI1 */
    sciREG1->GCR1 |= 0x80;
}

/** @fn void sciSetFunctional(sciBASE_t *sci, unsigned port)
*   @brief Change functional behavoiur of pins at runtime.
*   @param[in] sci   - sci module base address
*   @param[in] port  - Value to write to FUN register
*
*   Change the value of the PCFUN register at runtime, this allows to
*   dynaimcaly change the functionality of the SCI pins between functional
*   and GIO mode.
*/
void sciSetFunctional(sciBASE_t *sci, unsigned port)
{
    sci->FUN = port;
}


/** @fn void sciSetBaudrate(sciBASE_t *sci, unsigned baud)
*   @brief Change baudrate at runtime.
*   @param[in] sci  - sci module base address
*   @param[in] baud - baudrate in Hz
*
*   Change the SCI baudrate at runtime.
*/
void sciSetBaudrate(sciBASE_t *sci, unsigned baud)
{
    double   vclk = 10.000 * 1000000.0;
    unsigned f    = sci->GCR1 & 2 ? 16 : 1;

    sci->BAUD = ((unsigned)((vclk /(f*baud) + 0.5)) - 1) & 0x00FFFFFF;
}


/** @fn int sciIsTxReady(sciBASE_t *sci)
*   @brief Check if Tx buffer empty
*   @param[in] sci - sci module base address
*
*   @return The TX ready flag
*
*   Checks to see if the Tx buffer ready flag is set, returns
*   0 is flags not set otherwise will return the Tx flag itself.
*/
int sciIsTxReady(sciBASE_t *sci)
{
    return sci->FLR & SCI_TX_INT;
}


/** @fn void sciSendByte(sciBASE_t *sci, uint8_t byte)
*   @brief Send Byte
*   @param[in] sci  - sci module base address
*   @param[in] byte - byte to transfer
*
*   Sends a single byte in polling mode, will wait in the
*   routine until the transmit buffer is empty before sending
*   the byte.  Use sciIsTxReady to check for Tx buffer empty
*   before calling sciSendByte to avoid waiting.
*/
void sciSendByte(sciBASE_t *sci, uint8_t byte)
{
    while ((sci->FLR & SCI_TX_INT) == 0) { /* wait */ };
    sci->TD = byte;
}


/** @fn void sciSend(sciBASE_t *sci, unsigned length, uint8_t *data)
*   @brief Send Data
*   @param[in] sci    - sci module base address
*   @param[in] length - number of data words to transfer
*   @param[in] data   - pointer to data to send
*
*   Send a block of data pointed to by 'data' and 'length' bytes
*   long.  If interrupts have been enabled the data is sent using
*   interrupt mode, otherwise polling mode is used.  In interrupt
*   mode transmition of the first byte is started and the routine
*   returns imediatly, sciSend must not be called again until the
*   transfer is complete, when the sciNotification callback will
*   be called.  In polling mode, sciSend will not return  until 
*   the transfer is complete.
*
*   @note if data word is less than 8 bits, then the data must be left
*         aligned in the data byte.
*/
void sciSend(sciBASE_t *sci, unsigned length, uint8_t *data)
{
    int index = sci == sciREG1 ? 0 : 1;

    if (g_sciTransfer[index].mode & SCI_TX_INT)
    {
        /* we are in interrupt mode */
        
        g_sciTransfer[index].length = length;
        g_sciTransfer[index].data   = data;

        /* start transmit by sending first byte */        
        sci->TD     = *g_sciTransfer[index].data++;
        sci->SETINT = SCI_TX_INT;
    }
    else
    {
        /* send the data */
        while (length-- > 0)
        {
            while ((sci->FLR & SCI_TX_INT) == 0) { /* wait */ };
            sci->TD = *data++;
        }
    }
}


/** @fn int sciIsRxReady(sciBASE_t *sci)
*   @brief Check if Rx buffer full
*   @param[in] sci - sci module base address
*
*   @return The Rx ready flag
*
*   Checks to see if the Rx buffer full flag is set, returns
*   0 is flags not set otherwise will return the Rx flag itself.
*/
int sciIsRxReady(sciBASE_t *sci)
{
    return sci->FLR & SCI_RX_INT;
}


/** @fn int sciRxError(sciBASE_t *sci)
*   @brief Return Rx Error flags
*   @param[in] sci - sci module base address
*
*   @return The Rx error flags
*
*   Returns the Rx framing, overun and parity errors flags,
*   also clears the error flags before returning.
*/
int sciRxError(sciBASE_t *sci)
{
    int status = sci->FLR & (SCI_FE_INT | SCI_OE_INT |SCI_PE_INT);

    sci->FLR = SCI_FE_INT | SCI_OE_INT | SCI_PE_INT;

    return status;
}


/** @fn unsigned sciReceiveByte(sciBASE_t *sci)
*   @brief Receive Byte
*   @param[in] sci - sci module base address
*
*   @return Received byte
*
*    Recieves a single byte in polling mode.  If there is
*    not a byte in the receive buffer the routine will wait
*    until one is received.   Use sciIsRxReady to check to
*    see if the buffer is full to avoid waiting.
*/
int sciReceiveByte(sciBASE_t *sci)
{
    while ((sci->FLR & SCI_RX_INT) == 0) { /* wait */ };

    return sci->RD;
}


/** @fn void sciReceive(sciBASE_t *sci, unsigned length, uint8_t *data)
*   @brief Receive Data
*   @param[in] sci    - sci module base address
*   @param[in] length - number of data words to transfer
*   @param[in] data   - pointer to data buffer
*
*   Receive a block of 'length' bytes long and place it into the 
*   data buffer pointed to by 'data'.  If interrupts have been 
*   enabled the data is received using interrupt mode, otherwise
*   polling mode is used.  In interrupt mode receive is setup and
*   the routine returns imediatly, sciReceive must not be called 
*   again until the transfer is complete, when the sciNotification 
*   callback will be called.  In polling mode, sciReceive will not
*   return  until the transfer is complete.
*/
void sciReceive(sciBASE_t *sci, unsigned length, uint8_t *data)
{

    if (sci->SETINT & SCI_RX_INT)
    {
        /* we are in interrupt mode */
        int index = sci == sciREG1 ? 0 : 1;
        
        /* clear error flags */
        sci->FLR = SCI_FE_INT | SCI_OE_INT | SCI_PE_INT;

        g_sciTransfer[index].length = length;
        g_sciTransfer[index].data   = data;
    }
    else
    {   
        while (length-- > 0)
        {
            while ((sci->FLR & SCI_RX_INT) == 0) { /* wait */ };
            *data++ = sci->RD;
        }
    }
}
