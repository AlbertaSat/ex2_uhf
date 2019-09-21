/** @file spi.c
*   @brief SPI Driver Inmplmentation File
*   @date 04.October.2011
*   @version 1.02.000
*
*   (c) Texas Instruments 2011, All rights reserved.
*/



/* USER CODE BEGIN (0) */
/* USER CODE END */

#include "hw_spi.h"
#include "hw_sci.h"
#include "bl_config.h"

/* USER CODE BEGIN (1) */
/* USER CODE END */

#ifdef SPI_ENABLE_UPDATE

/** @struct g_spiPacket
*   @brief globals
*
*/
struct g_spiPacket
{
    uint32_t  mode;
    uint32_t  length;
    uint16_t  *data;
} g_spiPacket[2];

/* SPI Data Format */
spiDAT1_t g_spiDataFormat;


/** @fn void spiInit(void)
*   @brief Initializes the SPI Driver
*
*   This function initializes the SPI module.
*/
void spiInit(void)
{
/* USER CODE BEGIN (2) */
/* USER CODE END */
    /** @b intalise @b SPI2 */

    /** bring SPI out of reset */
    spiREG2->GCR0 = 1U;

    /** SPI2 master mode and clock configuration */
    spiREG2->GCR1 = (0 << 1)  /* CLOKMOD */
                  | 0;  /* MASTER */

    /** SPI2 enable pin configuration */
    spiREG2->ENAHIGHZ = 0;  /* ENABLE HIGHZ */

    /** - Delays */
    spiREG2->DELAY = (0 << 24)  /* C2TDELAY */
                   | (0 << 16)  /* T2CDELAY */
                   | (0 << 8)  /* T2EDELAY */
                   | 0;  /* C2EDELAY */

    /** - Data Format 0 */
    spiREG2->FMT0 = (8 << 24)  /* wdelay */
                  | (0 << 23)  /* parity Polarity */
                  | (0 << 22)  /* parity enable */
                  | (1 << 21)  /* wait on enable */
                  | (0 << 20)  /* shift direction */ /*0: MSB; 1:LSB*/
                  | (0 << 17)  /* clock polarity; high in-active */
                  | (0 << 16)  /* clock phase */
                  | (79 << 8) /* baudrate prescale */
                  | 8;  /* data word length */

    /** - Data Format 1 */
    spiREG2->FMT1 = (8 << 24)  /* wdelay */
                  | (0 << 23)  /* parity Polarity */
                  | (0 << 22)  /* parity enable */
                  | (1 << 21)  /* wait on enable */
                  | (0 << 20)  /* shift direction */
                  | (0 << 17)  /* clock polarity */
                  | (0 << 16)  /* clock phase */
                  | (79 << 8) /* baudrate prescale */
                  | 8;  /* data word length */

    /** - Data Format 2 */
    spiREG2->FMT2 = (8 << 24)  /* wdelay */
                  | (0 << 23)  /* parity Polarity */
                  | (0 << 22)  /* parity enable */
                  | (1 << 21)  /* wait on enable */
                  | (0 << 20)  /* shift direction */
                  | (0 << 17)  /* clock polarity */
                  | (0 << 16)  /* clock phase */
                  | (79 << 8) /* baudrate prescale */
                  | 8;  /* data word length */

    /** - Data Format 3 */
    spiREG2->FMT3 = (8 << 24)  /* wdelay */
                  | (0 << 23)  /* parity Polarity */
                  | (0 << 22)  /* parity enable */
                  | (1 << 21)  /* wait on enable */
                  | (0 << 20)  /* shift direction */
                  | (0 << 17)  /* clock polarity */
                  | (0 << 16)  /* clock phase */
                  | (79 << 8) /* baudrate prescale */
                  | 8;  /* data word length */

    /** - set interrupt levels */
    spiREG2->LVL = (0 << 9)  /* TXINT */
                 | (0 << 8)  /* RXINT */
                 | (0 << 6)  /* OVRNINT */
                 | (0 << 4)  /* BITERR */
                 | (0 << 3)  /* DESYNC */
                 | (0 << 2)  /* PARERR */
                 | (0 << 1) /* TIMEOUT */
                 | (0);  /* DLENERR */

    /** - clear any pending interrupts */
    spiREG2->FLG = 0xFFFFFFFFU;

    /** - enable interrupts */
    spiREG2->INT0 = (0 << 9)  /* TXINT */
                  | (0 << 8)  /* RXINT */
                  | (0 << 6)  /* OVRNINT */
                  | (0 << 4)  /* BITERR */
                  | (0 << 3)  /* DESYNC */
                  | (0 << 2)  /* PARERR */
                  | (0 << 1) /* TIMEOUT */
                  | (0);  /* DLENERR */

    /** @b initalise @b SPI2 @b Port */

    /** - SPI2 Port output values */
    spiREG2->PCDOUT =  0        /* SCS[0] */
                    | (0 << 1)  /* SCS[1] */
                    | (0 << 8)  /* ENA */
                    | (0 << 9)  /* CLK */
                    | (0 << 10)  /* SIMO */
                    | (0 << 11); /* SOMI */

    /** - SPI2 Port direction */
    spiREG2->PCDIR  =  1        /* SCS[0] */
                    | (1 << 1)  /* SCS[1] */
                    | (1 << 8)  /* ENA */
                    | (0 << 9)  /* CLK */
                    | (0 << 10)  /* SIMO */
                    | (1 << 11); /* SOMI */

    /** - SPI2 Port open drain enable */
    spiREG2->PCPDR  =  0        /* SCS[0] */
                    | (0 << 1)  /* SCS[1] */
                    | (0 << 8)  /* ENA */
                    | (0 << 9)  /* CLK */
                    | (0 << 10)  /* SIMO */
                    | (0 << 11); /* SOMI */

    /** - SPI2 Port pullup / pulldown selection */
    spiREG2->PCPSL  =  1        /* SCS[0] */
                    | (1 << 1)  /* SCS[1] */
                    | (1 << 8)  /* ENA */
                    | (1 << 9)  /* CLK */
                    | (1 << 10)  /* SIMO */
                    | (1 << 11); /* SOMI */

    /** - SPI2 Port pullup / pulldown enable*/
    spiREG2->PCDIS  =  0        /* SCS[0] */
                    | (0 << 1)  /* SCS[1] */
                    | (0 << 8)  /* ENA */
                    | (0 << 9)  /* CLK */
                    | (0 << 10)  /* SIMO */
                    | (0 << 11); /* SOMI */

    /* SPI2 set all pins to functional */
    spiREG2->PCFUN  =  1        /* SCS[0] */
                    | (0 << 1)  /* SCS[1] */
                    | (0 << 8)  /* ENA */
                    | (1 << 9)  /* CLK */
                    | (1 << 10)  /* SIMO */
                    | (1 << 11); /* SOMI */
                    
                    

    /** - Finaly start SPI2 */
    spiREG2->ENA = 1U;
}


/** @fn void spiSetFunctional(spiBASE_t *spi, uint32_t port)
*   @brief Change functional behavoiur of pins at runtime.
*   @param[in] spi   - Spi module base address
*   @param[in] port  - Value to write to PCFUN register
*
*   Change the value of the PCFUN register at runtime, this allows to
*   dynaimcaly change the functionality of the SPI pins between functional
*   and GIO mode.
*/
void spiSetFunctional(spiBASE_t *spi, uint32_t port)
{
/* USER CODE BEGIN (3) */
/* USER CODE END */

    spi->PCFUN = port;

/* USER CODE BEGIN (4) */
/* USER CODE END */
}


/** @fn uint32_t spiReceiveData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32_t blocksize, uint16_t *destbuff)
*   @brief Receives Data using polling method
*   @param[in] spi   		- Spi module base address
*   @param[in] dataconfig_t	- Spi DAT1 register configuration
*   @param[in] blocksize	- number of data
*   @param[in] destbuff		- Pointer to the destination data (16 bit).
*
*   @return flag register value.
*
*   This function transmits blocksize number of data from source buffer using polling method.
*/
uint32_t spiReceiveData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32_t blocksize, uint8_t *destbuff)
{
/* USER CODE BEGIN (5) */
/* USER CODE END */

    do
    {
        if(spi->FLG & 0x000000FF)
        {
          //break;
        }
        spi->DAT1 = dataconfig_t->DFSEL   << 24 |
                    dataconfig_t->CSNR    << 16 |
                    dataconfig_t->WDEL    << 26 |
                    dataconfig_t->CS_HOLD << 28 |
                    0x0000;
       while((spi->FLG & 0x00000100) != 0x00000100)
       {
       }
       *destbuff++ = spi->BUF;
    }while(--blocksize > 0);

/* USER CODE BEGIN (6) */
/* USER CODE END */

    return (spi->FLG & 0xFF);
}


/** @fn uint32_t spiGetData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32_t blocksize, uint16_t *destbuff)
*   @brief Receives Data using interrupt method
*   @param[in] spi   		- Spi module base address
*   @param[in] dataconfig_t	- Spi DAT1 register configuration
*   @param[in] blocksize	- number of data
*   @param[in] destbuff		- Pointer to the destination data (16 bit).
*
*   @return flag register value.
*
*   This function transmits blocksize number of data from source buffer using polling method.
*/
void spiGetData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32_t blocksize, uint16_t *destbuff)
{
     int index = spi == spiREG2 ? 0 : 1;

/* USER CODE BEGIN (7) */
/* USER CODE END */

     g_spiPacket[index].length = blocksize;
     g_spiPacket[index].data   = destbuff;

     spi->INT0 |= 0x0100;

     while(g_spiPacket[index].length-- > 0)
     {
       if(spi->FLG & 0x000000FF)
       {
          break;
       }
       spi->DAT1 = dataconfig_t->DFSEL   << 24 |
                   dataconfig_t->CSNR    << 16 |
                   dataconfig_t->WDEL    << 26 |
                   dataconfig_t->CS_HOLD << 28 |
                   0x0000;
     };
     spi->INT0 &= ~(0x0100);

/* USER CODE BEGIN (8) */
/* USER CODE END */
}


/** @fn uint32_t spiTransmitData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32_t blocksize, void *srcbuff)
*   @brief Transmits Data using polling method
*   @param[in] spi   		- Spi module base address
*   @param[in] dataconfig_t	- Spi DAT1 register configuration
*   @param[in] blocksize	- number of data
*   @param[in] srcbuff		- Pointer to the source data ( 16 bit).
*
*   @return flag register value.
*
*   This function transmits blocksize number of data from source buffer using polling method.
*/
uint32_t spiTransmitData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32_t blocksize, uint8_t *srcbuff)
{

    volatile uint32_t SpiBuf;

/* USER CODE BEGIN (9) */
/* USER CODE END */

    do
    {
       if(spi->FLG & 0x000000FF)
       {
         // break;
       }
       spi->DAT1 = dataconfig_t->DFSEL   << 24 |
                   dataconfig_t->CSNR    << 16 |
                   dataconfig_t->WDEL    << 26 |
                   dataconfig_t->CS_HOLD << 28 |
                   *srcbuff++;
       while((spi->FLG & 0x00000100) != 0x00000100)
       {
       }
       SpiBuf = spi->BUF;
    
    }while(--blocksize > 0);

/* USER CODE BEGIN (10) */
/* USER CODE END */

    return (spi->FLG & 0xFF);
}


/** @fn void spiSendData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32_t blocksize, void *srcbuff)
*   @brief Transmits Data using interrupt method
*   @param[in] spi   		- Spi module base address
*   @param[in] dataconfig_t	- Spi DAT1 register configuration
*   @param[in] blocksize	- number of data
*   @param[in] srcbuff		- Pointer to the source data ( 16 bit).
*
*   @return flag register value.
*
*   This function transmits blocksize number of data from source buffer using polling method.
*/
void spiSendData(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint32_t blocksize, uint16_t *srcbuff)
{
     int index = spi == spiREG2 ? 0 : 1;

/* USER CODE BEGIN (11) */
/* USER CODE END */

     g_spiPacket[index].length = blocksize;
     g_spiPacket[index].data   = srcbuff;

     g_spiDataFormat = *dataconfig_t;

     spi->INT0 = 0x0200;

/* USER CODE BEGIN (12) */
/* USER CODE END */
}


#endif
