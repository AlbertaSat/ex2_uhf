
# UHF Firmware

The primary function of the UHF firmware is to implement the base communication channel between satellite and ground via the UHF transceiver.
The firmware accepts CSP packets from the network stack, destined either for the UHF module itself (and processes them)
or for the ground (and packages them for transmission and stores them for next available downlink transmission, whereupon it sends them to the transceiver
chip using interrupt-driven SPI).
The firmware receives framed CSP data from the ground, and processes packets destined for itself, passing other packets to the network stack.

Secondary functions include temperature monitoring, fault detection and handling, as well as basic functions common to all the satellite's
modules like housekeeping data collection.

The ADF7021 transceiver chip configuration code has been ported from BlueBox open source code (https://github.com/satlab/bluebox/).

## Communications

This early design of communications protocols favours simplicity and minimal restrictions enforced by software, while 
avoiding locking out the ability to add complexity or restrictions in later designs if needed.

The firmware is designed assuming half-duplex, unreliable communications.

There are 5 modes that the transceiver firmware can be in:

- Off
- Listening, to receive an initiation of a communications session from ground control
- Waiting, to receive additional data while in a communications session
- Receiving incoming data
- Transmitting outgoing data.

For beacons, the satellite can transmit only when beacons are enabled,
transmitting is enabled, and the transceiver is in Off or Listening mode. For normal data, assuming transmitting is enabled, the satellite
should transmit data as soon as it is requested, and if the transceiver is not in Receiving mode.
The satellite returns to Listening mode if ground control ends a communications session, or after a configurable timeout has passed with no received messages.


While it is transmitting, the satellite is expected not to be able to receive commands from ground control, so the timing of its transmissions
must be restricted, with precedence given to ground control.
Except in the case of beacons, ground control should never be in a situation
where it must wait for the satellite to finish transmitting unneeded data, before ground is able to issue commands.
To ensure this, the satellite should transmit data only when ground control has asked for it, via commands from
ground control that expect a response. Since it would be infeasible for UHF firmware to verify that any data from other modules are only
responding to ground-control commands, all satellite modules' would have to implement this policy for it to be successful.

In case the satellite has urgent information it needs to transmit, this state should be broadcast in beacons and in response to initiation
of a communications session. Ground control can then request the urgent data at its perogative.

In determining transmission timing, ground should be treated as master and the satellite as slave.

Ground control can further restrict the satellite from transmitting by disabling transmission, in which case data to be transmitted to ground
could remain in RAM until transmission is re-enabled, ground control requests it to be purged, or the buffer is full and older messages
are overwritten by newer ones.

If this strategy is insufficient, other techniques could be added later, such as implementing a message priority system, etc.


### Transmission data format
A single transmission in either direction consists of:

- preamble
- sync word
- one or more framed data blocks
- End marker

A framed data block consists of:

- Data length
- Forward error correction code
- Data

The End marker is a value of 0 instead of a valid Data length.

Preamble and sync word will follow recommendations given in the ADF7201 datasheet.
If useful, a sync word might be included between data blocks, to ensure the receiver remains in sync.

The Data field may be encrypted, with the intention that transmissions from ground control will be encrypted.

A risk is that errors may occur in the Data length field, which must be detected and handled reasonably.
If possible, the error correction may be applied to the length field as well.

The Data will likely exclusively consist of CSP packets.

Within this format, additional ways of increasing throughput include a) allowing appending multiple (small) CSP packets into one Data block,
if this is feasible for the network stack, and b) using a command format that allows multple commands within a single CSP packet.


### Receiving
The firmware will dispatch a received CSP packet as soon as it is completely received, rather than waiting for the full transmission to finish.

### Transmitting
The satellite will begin transmitting as soon as a framed data block is buffered, and the transceiver is ready to transmit.
The firmware may accept additional CSP packets for transmission while it is already transmitting, in which case they are
appended to the current transmission. The transmission ends when all buffered ground-bound data blocks have been sent; the satellite
then sends an end marker and returns to Waiting mode.


### Unreliability
Due to the unreliable nature of radio communications, transmission success cannot be guaranteed.
If ground control expects a reply to a command, it may not receive one, and may not be able to tell if the reply was lost,
or if the satellite failed to receive the command.

To mitigate unreliability, all requests for data from ground control should be repeatable.
In the case of acknowledgements of commands requested by ground control, ground should be able to request state information to verify execution of the command.
In the case of transient data, subsequent requests are assumed to return different data.
In the case of stored data, the data should not be deleted after transmission, but be left for re-retrieval until the storage space is needed.
This strategy should be implemented by all satellite modules.


### Flexibility

The protocols outlined here allow some flexibility in how ground control interacts with the satellite.
For example, if increased reliability is needed, ground can issue one command per transmission, and receive replies to individual commands before issuing further commands.
If increased throughput is needed, ground can append a list of commands in a single transmission and receive all replies at once.

Likely, adjustable maximum transmission lengths will need to be implemented. How this will be handled is TBD.

## Commands

The set of ground-initiated commands implemented in the firmware is TBD.

It will include general commands common to all boards, such as retrieval of current state and logged information, setting configuration data,
and uploading firmware. The commands should follow a standard format that is shared by all boards.

The set of transceiver parameters that is ground-configurable is TBD. A risk is that it may be possible to incorrectly configure the satellite so that
further communication is impossible. A last-defence solution is to allow watchdog code to detect this and 
force a boot using a known working configuration vector.

The protocols do not enforce a command and data format on the other boards, including whether either are binary or text messages.
However, the protocols strongly encourage that all responses or returned
data are atomic and independent of context.
A command such as "eps.get-battery-v" should not
return a context-dependent message such as "15.0", but instead something like "eps.battv 15.0".
Log data should include what board (may be there already in CSP header) and log type is being returned, etc.

### Interaction

A typical interaction between ground and satellite depends on how ground control uses the system.

Scenario 1:

- Ground transmits a single command and waits to receive data.
- The UHF module receives the data and passes it to the network stack, and returns to Waiting mode.
- A single CSP message is received by the UHF module from the network, and is transmitted to ground, and the firmware continues Waiting.

Scenario 2:

- Ground transmits several commands in a single transmission, and waits to receive data.
- The UHF module receives multiple data blocks, and passes them to the network stack while it continues receiving.
- While it is still receiving, the UHF module begins to receive ground-bound data from the network, which it stores in a transmit buffer until it can be later transmitted.
- The received message ends, and the satellite immediately begins to transmit buffered ground-bound data.
- The UHF module continues to receive ground-bound data, which is appended to the end of the buffer.
- Once all of the buffered data has been transmitted, the firmware returns to Waiting.


When there are messages waiting to be sent by either ground or satellite, either will begin transmitting as soon as the end of a reception has been reached.
It remains possible for the UHF to send one CSP packet to ground, finish the transmission, and then have another CSP packet to transmit,
for example if a reply to a command was significantly delayed. As designed above, the satellite would send the first
transmission, return to Waiting, and if it hasn't begun Receiving a message from ground, it will immediately transmit the second message.
If ground has begun transmitting its own message after receiving the first from the satellite, then both ground and satellite will be
transmitting, and both will fail to receive the other's message.

To avoid this situation:

- Commands should generally immediately respond to requests for data without significant delay.
- Commands that are known to have a delay can be waited on by ground control.
- If desired, commands that take some time to process can be split into "begin execution" and "request data" commands, or can be designed
to be called multiple times, ie. a command that responds with data or a "data not ready" reply.
- If the above steps are insufficient, other rules might be implemented. For example, the satellite might be restricted from sending two
transmissions in a row, and instead buffer the second transmission until after receiving another message. This way, ground knows that they
can (and should) send another transmission as soon as it has completed receiving a message from the satellite. The satellite would strictly
follow a turn-taking policy of transmission and reception, while the ground would do so selectively; this way, the ground is exclusively responsible for
handling policy breakdown (eg. missed replies, etc.).

These protocols should be torture-tested in the lab with a wide range of short and long CSP packets, individual and groups of data blocks,
and various artificial delays, to determine if the design is robust enough without needing to add additional rules for the satellite
to enforce.


## Fault Detection

The UHF hardware includes a watchdog timer chip that must be reset once every 1.12 to 2.24 s (value depends on chip manufacturing variation),
otherwise the MCU will be reset.
The watchdog will be reset once per main loop of the firmware, and only if several checkpoints are properly reached in code.
A risk is that an interrupt handler may malfunction without starving out the main loop, and may go undetected.
To mitigate this, any critical interrupt handlers should include their own watchdog flags that the main loop is able to monitor.

The firmware should also monitor time since last contact with ground control, and consider it a fault if it exceeds some ground-configured
maximum time.

A low-priority task will also routinely perform a CRC check of stored firmware and any copies of satellite configuration vectors
(likely stored in ReRAM), and mark them as unsafe if a fault is detected.

The boot loader is expected to be able to select from several different firmware images, including at least the latest uploaded image,
and the original launch image. Flags stored in ReRAM (for reliability) will indicate things such as whether a particular image is considered
safe, or how many times it has been loaded without successful communication with ground, etc. Exact criteria for choosing which image
to boot are TBD.

For reliability, images should be run directly from long-term storage (likely Flash) rather than RAM, which is much more susceptible to radiation.

