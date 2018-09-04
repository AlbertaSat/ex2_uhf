
/***********************************************************
	framing.c -- Given a message to be sent over UHF, this frames the message into a series of bytes including everything that is sent to transmitter.
	This includes preamble, sync word, fwd error correction bytes, the message itself, plus any needed markers or post-amble.

	Requirements:
	The function FrameMessage( byte *pData, int nNumBytes) must take a message (any OSI layer 3 format or raw data including '\0's)
		and store it so it is ready for transmitting.
	???(TBD) Whether it stores a copy of the data or if the data is buffered externally may change in the future.
	??? If called while an existing message is still ready for transmitting (if GetTxByte has data to return),
		it should append this message to the existing one.
	??? The maximum message size is TBD. The maximum framed data storage is TBD.

	The function GetTxByte() returns subsequent bytes that are to be transferred (0-255),
	or -1 if there are no more bytes available (ie. end of transmission).
	It must be considered a real-time function and cannot block waiting for data, nor spend a long time processing the message.
	??? Max processing time is on the order of 0.5 ms (at least 2000 calls per second throughput).


**********************************************************/

#include <stdbool.h>

//#include "framing.h"
//#include "ringbuffer.h"



// The sizes, in bytes, of the fields that make up a UHF transmission:
#define NUM_PREAMBLE_BYTES	5	// At least 40 0-bits recommended
#define PREAMBLE_DATA_BYTE	0xaa
//#define SW_BYTES		// Sync word should be defined by adf7021.h
#define NUM_ERRCOR_BYTES	4
#define MSG_SIZE_BYTES	2	// Must accomodate max message size.
#define MAX_RAW_MESSAGE_BYTES	1024
#define POSTAMBLE_BYTES	1	// Must not stop transmission before transmitter has had a chance to transmit everything. xxx not sure how, yet.


int nTxIdx = 0;

// Frames the given message to transmitted when transmitter is able to.
// Returns true if success, false if failure (out of buffer space, etc.)
int FrameMessage( unsigned char *pData, int nNumBytes)
{


	return true;
}

// Returns the next available byte to be transmitted, or -1 when no more are available.
//old: unsigned char GetTxByte( int nTxIdx )
int GetTxByte( )
{
	if (nTxIdx < NUM_PREAMBLE_BYTES)
	{
		return PREAMBLE_DATA_BYTE;
	}


	// Nothing left to transmit
	return -1;
}

