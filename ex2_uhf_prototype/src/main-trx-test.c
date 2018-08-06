#include <stdio.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_emu.h"
#include "bsp.h"

#include "em_gpio.h"

#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"

#include "adf7021.h"


#include <string.h>
#include "transceiver-spi.h"

//
/* SPI Buffers */
char transmitBuffer[] = "socks";
#define            BUFFERSIZE    (sizeof(transmitBuffer) / sizeof(char))
//char receiveBuffer[BUFFERSIZE];
char receiveBuffer2[BUFFERSIZE];



void PrintTemperature( )
{
	//int nTemp10 = (int)(adf_readback_temp( ) * 10.0f);
	//printf( "Temp: %d.%d\n", nTemp10/10, nTemp10-(nTemp10/10 * 10) );
	printf( "Temp: %d\n", adf_readback_tempF( ) );
}

/******************************************************************************
 * @brief  initializing spi settings
 *
 *****************************************************************************/
void init(void)
{
  /* Enabling clock to USART 1 and 2*/
  //CMU_ClockEnable(cmuClock_USART1, true);
  CMU_ClockEnable(cmuClock_USART2, true);
  //CMU_ClockEnable(cmuClock_GPIO, true);

  /* Setup UART */
  //SPI_setup(USART1_NUM, GPIO_POS1, false);
  //SPI_setup(USART2_NUM, GPIO_POS1, false);
  SPI_setupSLAVE( );

  // xxx CAREFUL! Are you using PA9 for SWD ---and--- configuring it as SPI_CS?
  // Either change pin or PEN-out PA9 on USART2. Done?
}

int g_nTxBytesTotal = 0;

int main(void)
{
	EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
	CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;

	/* Chip errata */
	CHIP_Init();


	// Init DCDC regulator and HFXO with kit specific parameters
	EMU_DCDCInit(&dcdcInit);
	CMU_HFXOInit(&hfxoInit);

	/* Switch HFCLK tmastero HFXO and disable HFRCO */
	//CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	//CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
	// or...
	// Change clock speed for testing
	//CMU_HFRCOBandSet( cmuHFRCOFreq_1M0Hz );
	CMU_HFRCOBandSet( cmuHFRCOFreq_16M0Hz );
	//CMU_HFRCOBandSet( cmuHFRCOFreq_19M0Hz );
	//CMU_HFRCOBandSet( cmuHFRCOFreq_32M0Hz );

	/*
	// Setup SysTick Timer for 1 msec interrupts
	if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
		while (1) ;
	}
	*/

	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_GPIO, true);


	init( );

	  /* Data transmission to slave */
	  /* ************************** */
	  /*Setting up both RX and TX interrupts for slave */
//	  SPI1_setupSlaveInt(receiveBuffer2, BUFFERSIZE, NO_TX, NO_TX);


	  /* Data reception from slave */
	  /* ************************** */
	  /*Setting up both RX and TX interrupts for slave */
//	  SPI1_setupSlaveInt(NO_RX, NO_RX, transmitBuffer, BUFFERSIZE);

//	  SPI1_setupSlaveInt(receiveBuffer2, BUFFERSIZE, transmitBuffer, BUFFERSIZE);


	  /* Clearing the receive buffers */
//	  memset(receiveBuffer, '\0', BUFFERSIZE);
//	  memset(receiveBuffer2, '\0', BUFFERSIZE);


	  /* Transmitting data both ways simultanously */
	  /* ***************************************** */
	  /*Setting up both RX and TX interrupts for slave */
//	  SPI2_setupSlaveInt(receiveBuffer2, BUFFERSIZE, transmitBuffer, BUFFERSIZE);
	  /* Setting up RX interrupt for master */
	  //SPI3_setupRXInt(receiveBuffer, BUFFERSIZE);
	  /* Receiving data by transmitting dummy data to slave */
	  //USART3_sendBuffer(transmitBufferIncoming, BUFFERSIZE);



	// The code in this file should only be used for prototyping.
	EFM_ASSERT( EX2_DEVBOARD );


	// Init display module.
	DISPLAY_Init();

	// Retarget stdio to text display.
	if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK)
	{
		while (1) ;
	}

	printf("Test begins\n");

	// Test ADF7021 chip
	adf_set_power_on(XTAL_FREQ);
	printf("ADF powered on?\n");

	//PrintTemperature( );	// xxx Temperature is not valid in Rx mode?! See Reg7 defn.

	adf_configure();
	printf("ADF configured?\n");

	adf_set_rx_mode();
	// Set up SPI for receiving
	SPI2_setupSlaveInt(receiveBuffer2, BUFFERSIZE, NO_TX, NO_TX);
	printf("rx mode?\n");

	//delay_ms( 200 );

	// xxx when?
	// - before adf enable: spurrious interrupts
	// - after enable: ?
	// - after set receive mode: (is it possible to miss a message? I'd say no, because the chip was just turned on, and any real message would need to process a preamble etc)
//later:	SPI1_setupSlaveInt(receiveBuffer2, BUFFERSIZE, transmitBuffer, BUFFERSIZE);
	// No! - only when transmit mode is enabled.
	// I think this is how it works: The interrupt occurs when the transmit buffer is ready for data,
	// and the handler responds by giving it a byte. Then the USART deals with transmitting the data.
	// Here, since we're a slave, USART should wait on the ADF.
	// So, we should only enable the interrupt when we actually have data to send.
	// Due to noted spurious interrupts??? when writing an ADF register, we can follow bluebox and enable the interrupt after entering TX mode.
	// OR simply enable the interrupt when transmittable data is available.


	printf( "Ver: %x\n", adf_readback_version( ) );
	//printf( "RSSI: %d\n", adf_readback_rssi( ) );
	//printf( "AFC: %d\n", adf_readback_afc( ) );

	//printf("Test complete!\n");

	// Configure PB1 (dev board button) as input
	GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
	GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);

	// Monitor data on the SPI pins...
	/* dumbass
	GPIO_PinModeSet(gpioPortA, 6, gpioModeInput, 0);
	GPIO_PinModeSet(gpioPortA, 7, gpioModeInput, 0);
	GPIO_PinModeSet(gpioPortA, 8, gpioModeInput, 0);
	*/

	int nPin6Count = 0;
	int nPin7Count = 0;
	int nPin8Count = 0;
	int nPin8OffCount = 0;
	int nPin8PulseCount = 0;
	int P8On = 0;

	int bTxMode = 0;
	int nResetState = 0;
	while (1)
	{
		/**/
		int i;
		for (i=0; i<8192; ++i)
		{
			nPin6Count += GPIO_PinInGet( gpioPortA, 6 );
			nPin7Count += GPIO_PinInGet( gpioPortA, 7 );
			//nPin8Count += GPIO_PinInGet( gpioPortA, 8 );
			if (GPIO_PinInGet( gpioPortA, 8 ))
			{
				P8On = 1;
				nPin8Count++;
			}
			else
			{
				if (P8On) nPin8PulseCount++;
				P8On = 0;
				nPin8OffCount++;
			}
		}
		printf( "%x %x %x %x %x\n", nPin6Count, nPin7Count, nPin8Count, nPin8OffCount, nPin8PulseCount );
		/**/

		// No spam plz
//		if (bTxMode)
		{
			// temperature and voltage are only valid in tx mode
			PrintTemperature( );
			int nV10 = (int)(adf_readback_voltage( ) * 10.0f);
			printf( "V: %d.%d\n", nV10/10, nV10-(nV10/10 * 10) );
		}
//		else
		{
			printf( "RSSI: %x\n", adf_readback_rssi_raw( ) );
			printf( "TX: %x\n", g_nTxBytesTotal );
			printf( "Ver: %x\n", adf_readback_version( ) );
		}

		if (GPIO_PinInGet( BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN ) == 0)
		{
#	define PINSET( PINNAME ) GPIO_PinOutSet( ADF_PIN_##PINNAME )
#	define PINCLEAR( PINNAME ) GPIO_PinOutClear( ADF_PIN_##PINNAME )
#	define PINREAD( PINNAME )  GPIO_PinInGet( ADF_PIN_##PINNAME )

			// Reset the ADF...
			switch( nResetState++ ) {
			case 0:
				// Cycle CE
				PINCLEAR( CE );
				PINSET( CE );
				printf( "*** CE cycled\n" );
				break;

			case 1:
				adf_set_power_on(XTAL_FREQ);
				printf("**** Power'd\n");
				break;

			case 2:
				adf_configure();
				printf( "**** Config'd\n");
				//no break; fall through
			default:
				nResetState = 0;
			}
		}

		// Enable tx mode only while the button is pressed...
		if (GPIO_PinInGet( BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN ) == 0)
		{
			// Button is pressed
			if ( !bTxMode)
			{
				adf_set_tx_mode();
				// Set up SPI for transmitting: Enable SPI interrupt "now that there's data available" for it.
				//SPI2_setupSlaveInt(receiveBuffer2, BUFFERSIZE, transmitBuffer, BUFFERSIZE);
				SPI2_setupSlaveInt(NO_RX, NO_RX, transmitBuffer, BUFFERSIZE);	// xxx there's no need to disable Rx
				bTxMode = 1;
				printf("TX MODE ON...\n");
			}
		}
		else
		{
			if (bTxMode)
			{
				TRX_USART->IEN = 0;	// Disable interrupts

				adf_set_rx_mode();
				// Set up SPI for receiving
				SPI2_setupSlaveInt(receiveBuffer2, BUFFERSIZE, NO_TX, NO_TX);
				bTxMode = 0;
				printf("rx mode\n");
			}
		}

		delay_ms( 500 );
	}
}
