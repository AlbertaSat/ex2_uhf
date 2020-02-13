#include "crc.h"
#include <assert.h>
#include <stdio.h>

void populateCRCTable16(uint16_t* crc_table){
  /*
  Precomputes crc encoding for each possible byte and stores in lookup table.
  Values are stored as 16 bit in order to allow account for overlap during crc
  calculation

  :param crc_table: 16 bit array where precomputed values will be stored.
  */
  const uint16_t generator = 0x1021;
  for(uint16_t i = 0; i < 256; i++){
    uint16_t current_byte = (uint16_t)(i << 8);
    for(uint8_t bit = 0; bit < 8; bit++){
      if((current_byte & 0x8000) != 0){ //check if MSB is 1
        current_byte <<= 1;
        current_byte ^= generator;
      }
      else{
        current_byte <<= 1;
      }
    }
    crc_table[i] = current_byte;
  }
}

uint16_t get_crc16(uint8_t* data, int data_length, uint16_t* crc_table){
  /*
  Computes the crc of sequence of bits using a lookup table with precomputed
  byte encodings.

  :param data: array of bytes constituting the sequence bits to encode.
  :param data_length: number of bytes in input.
  :param crc_table: lookup table with precomputed byte encodings.
  */
  uint16_t crc = 0;
  for(int i = 0; i < data_length; i++){
    uint8_t byte = (uint8_t)((crc >> 8) ^ data[i]);
    crc = (uint16_t)((crc << 8) ^ (uint16_t)(crc_table[byte]));
  }
  return crc;
}


int main(){
  //crc testing
  uint16_t crc_table[256];
  populateCRCTable16(crc_table);
  uint8_t data[2] = {0x01, 0x02};
  uint16_t crc = get_crc16(data, 2, crc_table);
  assert((crc ^ 0x1373) == 0);
  printf("CRC test passed \n");
  return 0;
}
