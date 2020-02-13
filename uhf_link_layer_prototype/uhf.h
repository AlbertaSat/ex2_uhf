
#ifndef _UHF_H_
#define _UHF_H_

#include <stdint.h>
#include <csp/csp.h>


/** Interface to csp for UHF link layer */
/** UHF Frame */
typedef struct {
  uint16_t preamble;
  uint32_t sync_word;
  uint16_t data_length; //allows for 65536 bytes of data, (acceptable UB)
  union __attribute__((__packed__)){
    uint8_t* data;
  };
  uint16_t fec;
  uint16_t crc;
} uhf_frame_t;

int uhf_init();
int uhf_send(uint8_t* data);
int uhf_receive(uint8_t* data);

#endif
