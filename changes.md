
### Next (in reverse order):
Make a prototype flatsat: UHF comms from groundstation, message parsing/routing, prototype command interpreter. 
Incorporate FreeRTOS and I2C networking.
Diagnostics: RSSI, signal lock, ...
Begin framing messages: Rx: Detect beginning, read data, detect end, dispatch message. Tx: Package message incl SW, start Tx, shut down cleanly.
Set up SWD: Configure TRx; enable and handle interrupt; transmit SW.
	^^^ Handle irq: Disable SWD (int and or adf?), enable receiving (spi int?). Then enable spi when receive (or Tx!) is finished.
Configure multiple clients and port code if necessary; ensure sane radio configuration; enable variable power amp.
	^^^ Make a ex2_uhf_flatsat_v1 project. Copy/link the prototype code. Use add symbol "EX2_FLATSAT_V1". Configure pins for it.
	^^^ #define-out input, display etc. that's not on the flatsat. By default this should go into receive/print mode without need for input.
Add console output of received data so that it can be searched for test signals. Disable/reduce current printf spam.

### Major changes history:
2018-08-06:	Adapted SPI example code for TRx with MCU in slave mode. Bug fixes.
2018-07-28:	Configuring adf7021 to match dev board prototype wiring; test MCU configurations, TRx config and readback functions.
2018-07-26:	Ported adf7021 code to EFM.
2018-07-16:	Using bluebox adf7021 code as a starting point.
