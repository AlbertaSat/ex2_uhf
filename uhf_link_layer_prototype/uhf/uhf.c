#include <csp/csp.h>

//dummy values for now
#define PREAMBLE_2FSK 1
#define PREAMBLE_3FSK 2
#define PREAMBLE_4FSK 3


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

int uhf_send(uint8_t* data){
  uhf_frame_t frame;
  frame.data_length = sizeof(data);
  frame.preamble = PREAMBLE_2FSK;
  frame.sync_word = 16; //unique consistent value needs to be chosen for sync word
  frame.data = data; //MTU to be enforced at interface level, or here
  uint32_t crc = get_crc32(frame);

  //spi send to ADF here
}
