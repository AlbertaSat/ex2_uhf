/* bl_ymodem.c
 *
 * copyright (c) 2012 QJ Wang <qjwangti.com>
 *
 * based on ymodem.c, copyright (c) 2001 John G Dorsey
 * crc16 function from PIC CRC16, by Ashley Roll & Scott Dattalo
 *
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
 */
/*
   SENDER                                               RECEIVER
   ======                                               ========
                                                           C
                                                           C
   SOH 00 FF Filename NULs CRC CRC
                                                          ACK
                                                           C
   STX 01 FD Data[1024] CRC1 CRC2
                                                          ACK
   SOH 02 FC Data[128] CRC1 CRC2
                                                          ACK
   SOH 03 FB Data[100] ^Z[28] CRC1 CRC2
                                                          ACK
   EOT
                                                          NAK
   EOT
                                                          ACK
                                                           C
   SOH 00 FF NUL[128] CRC CRC
                                                          ACK
 */



#include "sci_common.h"
#include "bl_config.h"
#include "bl_flash.h"
#include "hw_sci.h"
#include "bl_ymodem.h"
#include "sys_common.h"

#if defined(UART_ENABLE_UPDATE)

char file_name[FILE_NAME_LENGTH];
unsigned int FlashDestination = ApplicationAddress ;            /* Flash user program offset */
extern unsigned long g_pulUpdateSuccess[8];
extern unsigned long g_ulUpdateStatusAddr;
extern uint32_t g_ulUpdateBufferSize;  //32 bytes or 8 32-bit words

static unsigned short Cal_CRC16(unsigned char *buf, unsigned long count)
{
        unsigned short crc = 0;
        int i;

        while(count--) {
             crc = crc ^ *buf++ << 8;
             for (i=0; i<8; i++) {
                 if (crc & 0x8000) {
                     crc = (crc << 1) ^ 0x1021;
                 } else {
                     crc = crc << 1;
                 }
             }
        }
        return crc;
}

/**
  * Description:   Receive a packet from sender
  * Parameters:    data, length, PACKET_TIMEOUT
  * length:
  *               0: end of transmission
  *              -1: abort by sender
  *              >0: packet length
  *
  * retyrn value: 0: normally return
  *              -1: PACKET_TIMEOUT or packet error
  *               1: abort by user
  */
static int receive_packet (sciBASE_t *sci, unsigned char *data, int *length)
{
	  unsigned short i, packet_size;
	  int key;
	  *length = 0;

	  key = UART_getChar(sci, PACKET_TIMEOUT);
	  if ( key < 0 ) {
		  return -1;
	  }
	  switch (key)
	  {
			case SOH:
			  packet_size = PACKET_SIZE;
			  break;
			case STX:
			  packet_size = PACKET_1K_SIZE;
			  break;
			case EOT:
			  return 0;
			case CAN:  //Cancel transfer.
				key = UART_getChar(sci, PACKET_TIMEOUT);
			    if (key == CAN)  {
				    *length = -1;
				     return 0;
			     }
			default:
			  *length = -1;
			  return -1;
	  }

	  *data = (char) key;

	  for (i = 1; i < (packet_size + PACKET_OVERHEAD); i++)
	  {
		  key = UART_getChar(sci, PACKET_TIMEOUT);
		  if( key < 0) {
			  return -1;
		  }
		  data[i] = (char) key;
	  }
	  if ( (data[PACKET_NUM_INDEX] + data[PACKET_NUM_COMP_INDEX]) != 255 )
	  {
		  return -1;
	  }
      if (Cal_CRC16(data + PACKET_HEADER, packet_size + PACKET_TRAILER) != 0) {
              return 1;
      }
	  *length = packet_size;
	  return 0;
}

/**
  * Description:   Receive a file using the ymodem protocol
  * Parameters:    buf: Address of the first byte
  * Return value:  The size of the file
  */
int Ymodem_Receive (sciBASE_t *sci, char *buf)
{
	  unsigned char packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
	  unsigned char file_size[FILE_SIZE_LENGTH];
	  unsigned char *file_ptr;
	  char *buf_ptr;
	  int i, packet_length, file_done, packets_received;
	  int session_done=0, errors=0, size = 0;
	  int retValue;
	  unsigned int oReturnCheck, updateStatusBank;
	  unsigned char ucBank=0;

	  updateStatusBank = 0;

	  /* Initialize FlashDestination variable */
	  FlashDestination = ApplicationAddress;
	  g_pulUpdateSuccess[1] = FlashDestination;

	  while( session_done == 0 )
      {
			packets_received = 0;
			file_done = 0;
			buf_ptr = buf;

			while( file_done == 0 )
			{
				  retValue =  receive_packet(sci, packet_data, &packet_length);
				  switch ( retValue )
				  {
					case 0:
						  errors = 0;
						  switch (packet_length)
						  {
								case - 1:   /* Abort by sender */
									UART_txByte(sci, ACK);
									return 0;
								case 0:     /* End of transmission */
									UART_txByte(sci, ACK);
									file_done = 1;
									break;
								default: /* Normal packet */
									  if ((packet_data[PACKET_NUM_INDEX] & 0xff) != (packets_received & 0xff))
									  {
										  UART_txByte(sci, NAK);
									  }
									  else
									  {
											if (packets_received == 0)
											{
												  /* Filename packet */
												  if (packet_data[PACKET_HEADER] != 0)
												  {
														/* Filename packet has valid data */
														for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);)
														{
															file_name[i++] = *file_ptr++;
														}
														file_name[i++] = '\0';
														for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);)
														{
														  file_size[i++] = *file_ptr++;
														}
														file_size[i++] = '\0';
														Str2Int(file_size, &size);
														g_pulUpdateSuccess[2] = (uint32_t) size;

														/* Test the size of the image to be sent */
														/* Image size is greater than Flash size */
														/* Erase the FLASH pages */
														if(!BLInternalFlashStartAddrCheck(FlashDestination,  size))
														{
															UART_txByte(sci, CAN);
															UART_txByte(sci, CAN);
															return -1;
														}

														/* Initialize the Flash Wrapper registers */
														oReturnCheck = 0;
														oReturnCheck = Fapi_BlockErase( ucBank, FlashDestination, size);

														// Return an error if an access violation occurred.
														if(oReturnCheck)
														{
															UART_txByte(sci, CAN);
															UART_txByte(sci, CAN);
															return -2;
														}

														UART_txByte(sci, ACK);
														UART_txByte(sci, CRC);
												  }
												  /* packet_data[PACKET_HEADER] = 0: Filename packet is empty, end session */
												  else
												  {
														UART_txByte(sci, ACK);
														file_done = 1;
														session_done = 1;
														break;
												  }  //end of "if (packet_data[PACKET_HEADER] != 0), else"
											}
											/* packets_received != 0: Data packet */
											else
											{
													memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
													oReturnCheck = Fapi_BlockProgram( ucBank, FlashDestination, (unsigned long)buf, packet_length);
													if(oReturnCheck)
													{
														// Indicate that the flash programming failed.
														UART_txByte(sci, CAN);
														UART_txByte(sci, CAN);
														return -2;
													}
													// Now update the address to program.
													FlashDestination += packet_length;
													UART_txByte(sci, ACK);
											}// end of "if (packets_received == 0), else"

											packets_received ++;
									  } /* sequence number ok */
								  break;
						  } //end of switch (packet_length)
						  break;
					case 1:    /*ABORT1 and ABORT2*/
						UART_txByte(sci, CAN);
						UART_txByte(sci, CAN);
						return -3;      //Aborted by user
					default:
						  if (packets_received != 0)
						  {
							  errors ++;
							  if (errors > MAX_ERRORS)
							  {
								UART_txByte(sci, CAN);
								UART_txByte(sci, CAN);
								return 0;
							  }
						  }
						  UART_txByte(sci, CRC);
						  break;
				  }//end of switch
			}//end of 2nd while()
	  }//end of 1st while()
	  if( errors == 0)
      {
      		oReturnCheck = Fapi_BlockProgram( updateStatusBank, g_ulUpdateStatusAddr, (unsigned long)&g_pulUpdateSuccess, g_ulUpdateBufferSize);
      }
	  return (int)size;
}


/**
  * Description: Transmit a data packet using the ymodem protocol
  * Parameters:  data, length
  * Return value:  None
  */
static void send_packet(sciBASE_t *sci, unsigned char *data, unsigned int blockNo)
{
	  unsigned short i = 0;
	  int crc, packet_size;

	  if (blockNo==0){
		  packet_size = PACKET_SIZE;
	  }else{
		  packet_size = PACKET_1K_SIZE;
	  }

	  crc = Cal_CRC16(data, packet_size);

	  UART_txByte(sci, (blockNo==0)? SOH:STX);
	  UART_txByte(sci, blockNo & 0xFF);
	  UART_txByte(sci, ~blockNo & 0xFF);

	  while (i < packet_size)
	  {
			UART_txByte(sci, data[i]);
			i++;
	  }
	  UART_txByte(sci, (crc >> 8) & 0xFF);
	  UART_txByte(sci, crc & 0xFF);
}



static void send_filePacket(sciBASE_t *sci, char* fileName, unsigned int size)
{
       unsigned long i = 0;
       unsigned char data[PACKET_SIZE];
       const char* num;

       if (fileName) {
           while (*fileName && (i < PACKET_SIZE-FILE_SIZE_LENGTH-2)) {
                  data[i++] = *fileName++;
           }
           data[i++] = 0;

           //Int2Str(num, size);
           num = U32toStr(size);
           while(*num) {
                data[i++] = *num++;
           }
       }

       while (i < PACKET_SIZE) {
               data[i++] = 0;
       }
       send_packet(sci, data, 0);

}

void send_dataPacket(sciBASE_t *sci, unsigned char *data, unsigned int size)
{
	  unsigned int send_size;
	  int blockno =1;
	  int key;

	  while (size > 0)
	  {
		  if (size > PACKET_1K_SIZE)
			  send_size = PACKET_1K_SIZE;
		  else
			  send_size = size;

		  send_packet(sci, data, blockno);

		  key = UART_getChar(sci, PACKET_TIMEOUT);

		  if ( key == ACK )
		  {
			  blockno++;
			  data += send_size;
			  size -= send_size;
		  } else {
			  if((key == CAN) ||(key == -1 )){
				  return;
    		  }
		  }
	  }

	  do{
		  UART_txByte(sci,EOT);
		  key = UART_getChar(sci, PACKET_TIMEOUT);
	  } while((key != ACK) && (key != -1));

      if(key == ACK){
    	  key = UART_getChar(sci, PACKET_TIMEOUT);
    	  if (key == CRC){
    		  do{
    			  send_filePacket(sci, 0, 0);
    			  key = UART_getChar(sci, PACKET_TIMEOUT);
    		  }while((key !=ACK) && (key != -1));
           }
       }
}


char Ymodem_Transmit (sciBASE_t *sci, unsigned char *buf, char* fileName, unsigned int size)
{
	  int key;
	  unsigned int crc_nake = 1;

	  do
	  {
			/* Send file name Packet */
			send_filePacket(sci, fileName, size);

			/* Wait for ACK and 'C' */
			key = UART_getChar(sci, PACKET_TIMEOUT);

			if (key == ACK){
				 key = UART_getChar(sci, PACKET_TIMEOUT);
				 if (key == CRC) {
					 send_dataPacket(sci, buf, size);
					 return size;
				 }
			}else if ((key == CRC) && (crc_nake)){
					crc_nake = 0;
					continue;
			}else if (( key != NAK) || (crc_nake)){
					break;
			}
	  }while(1);

	  UART_txByte(sci, CAN);
	  UART_txByte(sci, CAN);
	  return 0;
}


#endif
