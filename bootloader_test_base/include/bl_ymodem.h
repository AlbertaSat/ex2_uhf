/**
 ******************************************************************************
 * ymodem.h for Hercules MCU SCI Bootloader
 * Copytight (c) 2012 QJ Wang <qjwang@ti.com>
 *
 * Based on ymodem.h, copyright (c) 2001 John G Dorsey
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _YMODEM_H_
#define _YMODEM_H_


#include "hw_sci.h"
#include "sys_common.h"

#define PACKET_NUM_INDEX        (1)
#define PACKET_NUM_COMP_INDEX   (2)

#define PACKET_HEADER           (3)    /* SOH 00 FF */
#define PACKET_TRAILER          (2)    /* CRC CRC */
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)

#define FILE_NAME_LENGTH        (64)
#define FILE_SIZE_LENGTH        (16)

#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CAN                     (0x18)  /* two of these in succession aborts transfer */
#define CRC                     (0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define PACKET_TIMEOUT          (0x100000)
#define MAX_ERRORS              (5)

char Ymodem_Transmit (sciBASE_t *sci, uint8_t *buf, char* fileName, uint32_t fileSize);
int  Ymodem_Receive (sciBASE_t *sci, char *buf);

#endif  /* _YMODEM_H_ */

