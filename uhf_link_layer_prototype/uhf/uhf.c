#include <csp/csp.h>
#include "crc.h"
// #include "spi.h"

//dummy values for now
#define PREAMBLE_2FSK 1
#define PREAMBLE_3FSK 2
#define PREAMBLE_4FSK 3

uint16_t crc_table[256];
populateCRCTable16(crc_table);
// #define SPI_NUMBER 0
//
// spiDAT1_t dataconfig1_t;
// dataconfig1_t.CS_HOLD = FALSE;
// dataconfig1_t.WDEL    = FALSE;
// dataconfig1_t.DFSEL   = SPI_FMT_0;
// dataconfig1_t.CSNR    = 0x00;

void break_short_into_byte(uint8_t* array, int index, uint16_t value){
  /*
  Convenience function for breaking a short into two byte values,
  MSByte goes first under big endian scheme.

  :param array: array to add bytes to
  :param index: index from which to start adding bytes
  :param value: 16 bit value to be broken up
  */
  array[index] = (uint8_t) ((value >> 8) & 0xFF);
  array[index+1] = (uint8_t) (value & 0xFF);
}

void compute_crc(uhf_frame_t* frame){
  /*
  Takes relevant parts of frame and calculates 16 bit crc in big endian order.

  :param frame: uhf frame to encode

  */
  int plaintext_size = sizeof(frame->data_length)
                      + sizeof(frame->fec)
                      + (int)(frame->data_length);
  uint8_t frame_array[plaintext_size];
  break_short_into_byte(frame_array, 0, frame->data_length);
  memcpy(&frame_array[2], frame->data[0], frame->data_length);
  break_short_into_byte(frame_array, 2+frame->data_length, frame->fec);
  frame->crc = get_crc16(frame_array, plaintext_size, crc_table);
}



int uhf_init(){
  //Should this include the init for the chip itself?


}

int uhf_send(uint8_t* data){

   /* Frame-ize the data */ //Maybe worth having seperate functions for a) framing large data buffers that need more than 1 frame,
                            //b) Sending multiple consecutive frames over SPI
   uhf_frame_t frame;
   frame.data_length = (uint16_t) sizeof(data);//added /2 to make length multiple of 16 bits
   frame.preamble = PREAMBLE_2FSK;
   frame.sync_word = 16; //unique consistent value needs to be chosen for sync word
   frame.data = data; //MTU to be enforced at interface level, or here
   //compute fec


   //Should FEC be done here? or before data is passed to uhf_send.
   //Does just data get FEC'd, or do other parts of frame too?


   //to do: add chip select gpio signal
   // spiTransmit(SPI_NUMBER, &dataconfig1_t, 1, frame->preamble);
   // spiTransmit(SPI_NUMBER, &dataconfig1_t, 2, frame->sync_word);
   // spiTransmit(SPI_NUMBER, &dataconfig1_t, frame->data_length, frame->data);//each block is 16 bits
   //what happens if data isn't a 16 bit multiple?
    //csp_uhf interface should add padding to csp_packet to fit desired multiple
   //Add return 1 for error

   return 0;
}


int uhf_receive(uint8_t* data){

   //MCU is only notified when sync word is detected
   //spi only receives bits after sync word



}
