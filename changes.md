### Later:
- Encryption of uplink transmissions (and whitening of unencrypted?)
- Board state handling (power, TRx mode, etc)
- Interface with board peripherals (temperature, USB, PA, etc...)
<br>**Shared or overlap with other boards firmware:**
- Fault detection, safe mode, boot image selection, boot image upload (UHF board might need to update directly from Rx while other boards from I2C network).
- Command processing and transmit data queuing.
- Housekeeping data logging and transmission.

### Next (in reverse order):
- Make a prototype flatsat: UHF comms from groundstation, message parsing/routing, prototype command interpreter.
- Incorporate FreeRTOS and I2C networking.
- Diagnostics: RSSI, signal lock, ...
- Begin framing messages: Rx: Detect beginning, read data, detect end, dispatch message. Tx: Package message incl SW, start Tx, shut down cleanly.
- Set up SWD: Configure TRx; enable and handle interrupt; transmit SW.
<br>	Handle irq: Disable SWD (int and or adf?), enable receiving (spi int?). Then enable spi when receive (or Tx!) is finished.
- Configure multiple clients and port code if necessary; ensure sane radio configuration; <strike>enable variable power amp (done-ish).</strike>
<br>	Make a ex2\_uhf\_flatsat\_v1 project. Copy/link the prototype code. Use add symbol "EX2\_FLATSAT\_V1". Configure pins for it.
<br>	#define-out input, display etc. that's not on the flatsat. By default this should go into receive/print mode without need for input.
- Add console output of received data so that it can be searched for test signals. Disable/reduce current printf spam.

### Major changes history:
- 2018-08-06:	Adapted SPI example code for TRx with MCU in slave mode. Bug fixes.
- 2018-07-28:	Configuring adf7021 to match dev board prototype wiring; test MCU configurations, TRx config and readback functions.
- 2018-07-26:	Ported adf7021 code to EFM.
- 2018-07-16:	Using bluebox adf7021 code as a starting point.
