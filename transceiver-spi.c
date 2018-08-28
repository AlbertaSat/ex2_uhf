/**************************************************************************//**
 * @file spi.c
 * @brief SPI
 * @author Silicon Labs
 * @version 1.13
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include "transceiver-spi.h"

#include "em_device.h"
//#include <efm32pg12b_usart.h>
#include "em_gpio.h"


/* Modifying the example code:
 * The example sets up SPI1 with USART1 as slave.
 * I need to use a slave, so SPI1 code, but with USART2 because its pins are exposed.
 * Therefore all original SPI2 code can be removed, and original USART2 code removed,
 * THEN USART1 converted to USART2...
 */

/* Buffer pointers and indexes */
char* slaveTxBuffer;
int slaveTxBufferSize;
volatile int slaveTxBufferIndex;
char* slaveRxBuffer;
int slaveRxBufferSize;
volatile int slaveRxBufferIndex;
//char* masterRxBuffer;
//int masterRxBufferSize;
//volatile int masterRxBufferIndex;


/**************************************************************************//**
 * @brief Setup a USART as SPI
 * @param spiNumber is the number of the USART to use (e.g. 1 USART3)
 * @param location is the GPIO location to use for the device
 * @param master set if the SPI is to be master
 *****************************************************************************/


// Which USART is used for TRX chip, and which pin routing location?
//xxx moved

#ifdef EX2_DEVBOARD

#define USE_USART2
#define TRX_USART			USART2
#define TRX_USART_RX_IRQn 	USART2_RX_IRQn
#define TRX_USART_TX_IRQn 	USART2_TX_IRQn
#define TRX_USE_PINROUTING_LOC1
// The following pins must match the selected USART and location, according to MCU manual.
#define TRX_PORT		gpioPortA	// Assuming all USART pins share one port.
#define TRX_PIN_MOSI	6
#define TRX_PIN_MISO	7
#define TRX_PIN_CLK		8
#define TRX_PIN_CS		9

#elif defined( EX2_GIANT_M3 )

//#define USE_USART2
#define TRX_USART			USART1
#define TRX_USART_RX_IRQn 	USART1_RX_IRQn
#define TRX_USART_TX_IRQn 	USART1_TX_IRQn
//#define TRX_USE_PINROUTING_LOC1
#define TRX_LEGACY_LOCNUM	1
// The following pins must match the selected USART and location, according to MCU manual.
#define TRX_PORT		gpioPortD	// Assuming all USART pins share one port.
#define TRX_PIN_MOSI	0
#define TRX_PIN_MISO	1
#define TRX_PIN_CLK		2
#define TRX_PIN_CS		3

#endif
void SPI_setupSLAVE( )
{
	USART_TypeDef *spi = TRX_USART;

	// baudrate
	spi->CLKDIV = 128 * (SPI_PERCLK_FREQUENCY / SPI_BAUDRATE - 2);

	// Configure SPI
	spi->CTRL = USART_CTRL_SYNC;	// SPI is synchronous
	spi->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;	// Clear old transfers/receptions
	spi->IEN = 0;	// Disable interrupts
	// Pin enable and routing...
//#ifdef USART_ROUTELOC0_TXLOC_DEFAULT	// GG11 PG12 etc per-pin routing...
#ifdef EX2_GIANT_M3
	// GG1 etc: PEN and ROUTE are in the same variable
	spi->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN | (TRX_LEGACY_LOCNUM << 8);
#else
	spi->ROUTEPEN = USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_CSPEN;
	/* Sorry, this just ADDS a layer of confusion to the already horrible ROUTELOC0 defines...
	//#define TRX_ROUTELOC_VAL	ROUTELOC_BITS( LOC1 )	// ROUTELOC_BITS is defined within code...

	// This obfuscated macro assembles a set of USART_ROUTELOC0_xxxLOC_LOCnnn #defines
#	define ROUTELOC_BITS( LOC )	(USART_ROUTELOC0_TXLOC_##LOC | USART_ROUTELOC0_RXLOC_##LOC \
		  | USART_ROUTELOC0_CLKLOC_##LOC  / *| USART_ROUTELOC0_CSLOC_##TRX_LOC * /)
	spi->ROUTELOC0 = TRX_ROUTELOC_VAL;
	*/

#	ifdef TRX_USE_PINROUTING_LOC1
	spi->ROUTELOC0 = USART_ROUTELOC0_TXLOC_LOC1 | USART_ROUTELOC0_RXLOC_LOC1
		  | USART_ROUTELOC0_CLKLOC_LOC1 | USART_ROUTELOC0_CSLOC_LOC1;
#	else
#	error No TRX routing location configured!
#	endif
#endif

	// Enable TX and RX
	spi->CMD = USART_CMD_TXEN | USART_CMD_RXEN;

	// Clear previous interrupts
	spi->IFC = _USART_IFC_MASK;

	// Setup pins, configuring SPI slave mode
	GPIO_PinModeSet( TRX_PORT, TRX_PIN_MOSI, gpioModeInput, 0);
	GPIO_PinModeSet( TRX_PORT, TRX_PIN_MISO, gpioModePushPull, 0);
	//GPIO_PinModeSet( TRX_PORT, TRX_PIN_CS,	gpioModeInput, 0);
	// If not connecting the CS pin, configure it for open drain so it's always pulled low.
	GPIO_PinModeSet( TRX_PORT, TRX_PIN_CS,	gpioModeWiredOrPullDown, 0);
	GPIO_PinModeSet( TRX_PORT, TRX_PIN_CLK, gpioModeInput, 0);
}



/**************************************************************************//**
 * @brief USARTx RX IRQ Handler Setup
 * @param receiveBuffer points to where to place recieved data
 * @param receiveBufferSize indicates the number of bytes to receive
 *****************************************************************************/
void SPI2_setupRXInt(char* receiveBuffer, int receiveBufferSize)
{
  USART_TypeDef *spi = TRX_USART;

  /* Setting up pointer and indexes */
  slaveRxBuffer      = receiveBuffer;
  slaveRxBufferSize  = receiveBufferSize;
  slaveRxBufferIndex = 0;

  /* Clear RX */
  spi->CMD = USART_CMD_CLEARRX;

  /* Enable interrupts */
  NVIC_ClearPendingIRQ(TRX_USART_RX_IRQn);
  if (receiveBuffer)
  {
	  NVIC_EnableIRQ(TRX_USART_RX_IRQn);
	  spi->IEN |= USART_IEN_RXDATAV;
  }
}



/**************************************************************************//**
 * @brief USARTx TX IRQ Handler Setup
 * @param transmitBuffer points to the data to send
 * @param transmitBufferSize indicates the number of bytes to send
 *****************************************************************************/
void SPI2_setupTXInt(char* transmitBuffer, int transmitBufferSize)
{
  USART_TypeDef *spi = TRX_USART;

  /* Setting up pointer and indexes */
  slaveTxBuffer      = transmitBuffer;
  slaveTxBufferSize  = transmitBufferSize;
  slaveTxBufferIndex = 0;

  /* Clear TX */
  spi->CMD = USART_CMD_CLEARTX;

  /* Enable interrupts */
  NVIC_ClearPendingIRQ(TRX_USART_TX_IRQn);
  if (transmitBuffer)
  {
	  NVIC_EnableIRQ(TRX_USART_TX_IRQn);
	  spi->IEN |= USART_IEN_TXBL;
  }
}





/**************************************************************************//**
 * @brief USARTx IRQ Handler Setup
 * @param receiveBuffer points to where received data is to be stored
 * @param receiveBufferSize indicates the number of bytes to receive
 * @param transmitBuffer points to the data to send
 * @param transmitBufferSize indicates the number of bytes to send
 *****************************************************************************/
void SPI2_setupSlaveInt(char* receiveBuffer, int receiveBufferSize, char* transmitBuffer, int transmitBufferSize)
{
	// xxx There should be no need to do both together, and reset the Rx buffers when Tx is enabled.
	// xxx As far as the MCU is concerned, I think it can behave as if the TRx chip is duplex.
  SPI2_setupRXInt(receiveBuffer, receiveBufferSize);
  SPI2_setupTXInt(transmitBuffer, transmitBufferSize);
}



/**************************************************************************//**
 * @brief USARTx RX IRQ Handler
 *****************************************************************************/
#ifdef USE_USART2
void USART2_RX_IRQHandler(void)
#else
void USART1_RX_IRQHandler(void)
#endif
{
  USART_TypeDef *spi = TRX_USART;
  uint8_t       rxdata;

  if (spi->STATUS & USART_STATUS_RXDATAV)
  {
    /* Reading out data */
    rxdata = spi->RXDATA;

    if (slaveRxBufferIndex < slaveRxBufferSize)
    {
      /* Store Data */
      slaveRxBuffer[slaveRxBufferIndex] = rxdata;
      slaveRxBufferIndex++;
    }
  }
}



/**************************************************************************//**
 * @brief USARTx TX IRQ Handler
 *****************************************************************************/
#ifdef USE_USART2
void USART2_TX_IRQHandler(void)
#else
void USART1_TX_IRQHandler(void)
#endif
{
  USART_TypeDef *spi = TRX_USART;

  if (spi->STATUS & USART_STATUS_TXBL)
  {
    /* Checking that valid data is to be transferred */
    if (slaveTxBuffer != 0)
    {
      /* Writing new data */
      spi->TXDATA = slaveTxBuffer[slaveTxBufferIndex];
      slaveTxBufferIndex++;

      extern int g_nTxBytesTotal;
      g_nTxBytesTotal++;

      /*Checking if more data is to be transferred */
      if (slaveTxBufferIndex == slaveTxBufferSize)
      {
        slaveTxBuffer = 0;
      }
    }
    else
    {
      /* Sending 0 if no data to send */
      spi->TXDATA = 0;
    }
  }
}






/******************************************************************************
 * @brief sends data using USARTn
 * @param txBuffer points to data to transmit
 * @param bytesToSend bytes will be sent
 *****************************************************************************/
void TRX_USART_sendBuffer(char* txBuffer, int bytesToSend)
{
  USART_TypeDef *uart = TRX_USART;
  int           ii;

  /* Sending the data */
  for (ii = 0; ii < bytesToSend;  ii++)
  {
    /* Waiting for the usart to be ready */
    while (!(uart->STATUS & USART_STATUS_TXBL)) ;

    if (txBuffer != 0)
    {
      /* Writing next byte to USART */
      uart->TXDATA = *txBuffer;
      txBuffer++;
    }
    else
    {
      uart->TXDATA = 0;
    }
  }

  /*Waiting for transmission of last byte */
  while (!(uart->STATUS & USART_STATUS_TXC)) ;
}


