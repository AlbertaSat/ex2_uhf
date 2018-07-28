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


void PrintTemperature( )
{
	int nTemp10 = (int)(adf_readback_temp( ) * 10.0f);
	printf( "Temp: %d.%d\n", nTemp10/10, nTemp10-(nTemp10/10 * 10) );
}

int main(void)
{
	EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
	CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;

	/* Chip errata */
	CHIP_Init();


	// Init DCDC regulator and HFXO with kit specific parameters
	EMU_DCDCInit(&dcdcInit);
	CMU_HFXOInit(&hfxoInit);

	/* Switch HFCLK to HFXO and disable HFRCO */
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
	printf("rx mode?\n");

	//delay_ms( 200 );


	printf( "Ver: %d\n", adf_readback_version( ) );
	//printf( "RSSI: %d\n", adf_readback_rssi( ) );
	//printf( "AFC: %d\n", adf_readback_afc( ) );

	//printf("Test complete!\n");

	// Configure PB1 as input
	GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);

	// Monitor data on the SPI pins...
	GPIO_PinModeSet(gpioPortA, 6, gpioModeInput, 0);
	GPIO_PinModeSet(gpioPortA, 7, gpioModeInput, 0);
	GPIO_PinModeSet(gpioPortA, 8, gpioModeInput, 0);

	int nPin6Count = 0;
	int nPin7Count = 0;
	int nPin8Count = 0;

	int bTxMode = 0;
	while (1)
	{
		int i;
		for (i=0; i<8192; ++i)
		{
			nPin6Count += GPIO_PinInGet( gpioPortA, 6 );
			nPin7Count += GPIO_PinInGet( gpioPortA, 7 );
			nPin8Count += GPIO_PinInGet( gpioPortA, 8 );
		}
		printf( "%x %x %x\n", nPin6Count, nPin7Count, nPin8Count );


		// No spam plz
		if (bTxMode)
		{
			// temperature and voltage are only valid in tx mode
			PrintTemperature( );
			int nV10 = (int)(adf_readback_voltage( ) * 10.0f);
			printf( "V: %d.%d\n", nV10/10, nV10-(nV10/10 * 10) );
		}
		else
		{
			printf( "RSSI: %x\n", adf_readback_rssi_raw( ) );
		}

		// Enable tx mode only while the button is pressed...
		if (GPIO_PinInGet( BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN ) == 0)
		{
			// Button is pressed
			if ( !bTxMode)
			{
				adf_set_tx_mode();
				bTxMode = 1;
				printf("TX MODE ON...\n");
			}
		}
		else
		{
			if (bTxMode)
			{
				adf_set_rx_mode();
				bTxMode = 0;
				printf("rx mode\n");
			}
		}

		delay_ms( 500 );
	}
}
