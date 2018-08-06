# ex2_uhf
Ex-Alta 2 UHF module firmware

The 'prototype' branch contains code for a prototype built around a Gecko PG12 board and an ADF7021 eval board.
 
# IF YOU ARE GETTING BUILD ERRORS
If you get build errors like "undefined symbol `__Vectors' referenced in expression" with a fresh git clone,
it may be due to missing build directories and a buggy Simplicity Studios overwriting the project file with a bad one.
Fix:
1. Close the project
2. git checkout ex2_uhf_prototype/.project
3. Open and build the project again.


## Bugs, flaws, do me

### Not yet done
- Setup SWD interrupt on RISING edge

### CS pin is reserved for ADF7021 SPI, but it is unconnected.
The typical implementation of an SPI slave has the MCU selected with the CS pin, but the ADF7021 doesn't do this.
The current work-around is to pull the pin low so the MCU is always the selected slave.
A problem is that this reserves a GPIO pin for CS but doesn't use it. There may be a neater solution to get around using CS in code (however such a solution might instead be an ugly kludge?).
**Status:** Does not affect functionality, DNF.

### Transmission SPI is currently blocking?
Transmitting data loops until the transmission is complete.
It should be made threaded, or use polling and exit instead of waiting.
**Status:** Needs to be fixed before any other time-critical main-loop functionality is added (note that Tx and Rx are still concurrent---at least in code---because Rx is interrupt-driven).
Incorrect?

### Hardcoded timing variables
Some assume 14MHz, etc.

## Questions

How is data reception handled? Can a single transmission of several concatenated messages be received, or is each message its own transmission (the latter sounds bad due to preamble overhead, etc)? If the former, is a full reception completed and then processed, or are commands dispatched while data is incoming?

How are Tx and Rx interleaved? There are at least 2 classes of Tx data: Automated pings, and ground-requested data. Acks can be considered the latter.
1. Pings: Conditions for Tx: a) Not receiving, b) No pending requested Tx's, c) Ground station has signed off (this might be just "not receiving and no pending Tx" or the sat has been commanded to enter "ping mode" or there have been no data reception for n seconds, or other?)
2. Requested data: Conditions for Tx: a) Finished receiving, b) Tx not disabled (If ground wants to send a message and not receive requested data, either it can include a command to hold Tx's, or it could simply avoid requesting any data or Acks).

How is data Ack'd? Is ground-requested data packaged similarly into one blob transmission (the format is ground-controlled, ie. it can request data one at a time, or request multiple things at once). Then any requested data is kept in a ring buffer etc. until it is told to delete?
I think a reasonable design is: Any transmission is non-destructive. If eg. logs are requested, the sat sends them, but does not delete them. Thus ground can request the same data over and over.
- There could be exceptions, eg. Pings or momentary status does not need to be kept, and there's no way or need for ground to request the same data twice.
- Regular data is stored in ring buffers, and if ground doesn't specifically request something is deleted, it is just overwritten when room is needed.
- For things like regular housekeeping data, ground could request eg. "Get logs from t=679325", thus allowing resending of missed data, and sending of next data without the need for Ack. The time could default to last-timestamp+1, so ground could get logs in order and then specifically request any it missed.


