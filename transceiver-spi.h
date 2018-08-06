
// main
#define HFRCO_FREQUENCY         14000000
#define SPI_PERCLK_FREQUENCY    HFRCO_FREQUENCY
#define SPI_BAUDRATE            1000000


// SPI

#define TRX_USART			USART2


void SPI_setupSLAVE( );
//void SPI3_setupRXInt(char* receiveBuffer, int bytesToReceive);
void SPI2_setupSlaveInt(char* receiveBuffer, int receiveBufferSize, char* transmitBuffer, int transmitBufferSize);


// USART

#define NO_RX                    0
#define NO_TX                    NO_RX

void USART3_sendBuffer(char*, int);
