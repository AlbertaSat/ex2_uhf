/*
	GG11-based in-house designed prototype board settings.
	Configured as a prototype satellite.

	PG12 dev board settings.
	Configured as a prototype or production ground station transceiver interface (receiving telecommands via UART/USB).
*/


// ---- Optional hardware

#define ENABLE_UART_COMMANDDATA		// Used for debugging only, when used with launch prototypes.


// ---- Dev board features

#define ENABLE_TEXT_DISPLAY


// ---- Pin configurations

// ADF7021 transceiver
#define ADF_PIN_SWD			8
#define ADF_PORTPIN_SWD		gpioPortB, ADF_PIN_SWD

#define ADF_PORTPIN_SCLK	gpioPortC, 11
#define ADF_PORTPIN_SREAD	gpioPortD, 10
#define ADF_PORTPIN_SDATA	gpioPortC, 10
#define ADF_PORTPIN_SLE		gpioPortD, 11
#define ADF_PORTPIN_MUXOUT	gpioPortD, 12
#define ADF_PORTPIN_CE		gpioPortD, 8


// TRx SPI
#define TRX_USART			USART2	// #defined typedef
#define USARTn_				USART2_	// Literal name
#define TRXLOC				LOC1
#define TRXCSLOC			TRXLOC	// In some configs this pin needs to be moved to a different location.

// The following pins must match the selected USART and location, according to MCU manual.
#define TRX_PORT		gpioPortA	// Assuming USART pins share one port...
#define TRX_PIN_MOSI	6
#define TRX_PIN_MISO	7
#define TRX_PIN_CLK		8
#define TRX_PORT_CS		TRX_PORT	// ... except CS may be rerouted in some configurations.
#define TRX_PIN_CS		9


// Command/data UART (for console/USB telecommands and telemetry interface)
#define COMMDATA_UART_PORTPIN_TX	gpioPortD, 9	not coded!!!!!!!!!!!!
#define COMMDATA_UART_PORTPIN_RX	gpioPortC, 9

#define COMMDATA_UART_TXLOC			TXLOC_LOC17		not coded!!!!!!!!!!!!
#define COMMDATA_UART_RXLOC			RXLOC_LOC13
