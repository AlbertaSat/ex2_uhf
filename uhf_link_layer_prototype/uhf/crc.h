#include <stdint.h>

void populateCRCTable16(uint16_t* crc_table);
uint16_t get_crc16(uint8_t* data, int data_length, uint16_t* crc_table);
