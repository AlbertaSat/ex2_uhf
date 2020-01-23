
#ifndef _UHF_H_
#define _UHF_H_

#include <stdint.h>



/** CAN Frame */
typedef struct {
  uint16_t preamble;
  unint32_t sync_word;
  uint8_t data_length;
  //**< Frame Data - MTU TBD, setting to 8 bytes for now > */
  union __attribute__((aligned(8))){
    uint8_t data[8];
    uint16_t data16[4];
    uint32_t data32[2];
  };
} uhf_frame_t;

int frame_data(uint32_t* data, uhf_frame_t* uhf_frame);

#endif
