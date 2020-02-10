//#include <csp/csp.h>
#include "spi.h"
#include "uhf.h"

//dummy values for now
#define PREAMBLE_2FSK 1
#define PREAMBLE_3FSK 2
#define PREAMBLE_4FSK 3
#define SPI_NUMBER spiREG3

spiDAT1_t dataconfig1_t;
//dataconfig1_t.CS_HOLD = FALSE;
//dataconfig1_t.WDEL    = FALSE;
//dataconfig1_t.DFSEL   = SPI_FMT_0;
//dataconfig1_t.CSNR    = 0x00;


//Not yet working, need to convert frame into array
uint32_t get_crc32(uhf_frame_t* frame) {
   uint32_t i, j;
   uint32_t byte, crc, mask;
   static uint32_t crc_table[256];

   //establish lookup table
   if (crc_table[1] == 0) {
      for (byte = 0; byte <= 255; byte++) {
         crc = byte;
         for (j = 7; j >= 0; j--) {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
         }
         crc_table[byte] = crc;
      }
   }

   uint8_t frame_array[sizeof(frame)];//Should eventually be frame size - size of crc and fec
   //perform division
   i = 0;
   crc = 0xFFFFFFFF;
   while ((byte = frame_array[i]) != 0) {
      crc = (crc >> 8) ^ crc_table[(crc ^ byte) & 0xFF];
      i = i + 1;
   }
   //return bitwise NOT
   return ~crc;
}

int uhf_init(){

      //Should this include the init for the chip itself?

}

int uhf_send(uint8_t* data){

   /* Frame-ize the data */ //Maybe worth having seperate functions for a) framing large data buffers that need more than 1 frame,
                            //b) Sending multiple consecutive frames over SPI
   uhf_frame_t frame;
   frame.data_length = sizeof(data)/2;//added /2 to make length multiple of 16 bits
   frame.preamble = PREAMBLE_2FSK;
   frame.sync_word = 16; //unique consistent value needs to be chosen for sync word
   frame.data = data; //MTU to be enforced at interface level, or here
   uint32_t crc = get_crc32(&frame);


   //Should FEC be done here? or before data is passed to uhf_send.
   //Does just data get FEC'd, or do other parts of frame too?


   //add chip select gpio signal
   spiTransmit(SPI_NUMBER, &dataconfig1_t, 1, frame.preamble);
   spiTransmit(SPI_NUMBER, &dataconfig1_t, 2, frame.sync_word);
   spiTransmit(SPI_NUMBER, &dataconfig1_t, frame.data_length, frame.data);//each block is 16 bits
   //what happens if data isn't a 16 bit multiple?
   //Add return 1 for error

   return 0;
}


int uhf_receive(uint8_t* data){

   //MCU is only notified when sync word is detected
   //spi only receives bits after sync word



}
