
# UHF Firmware

The primary function of the UHF firmware is to implement the base communication channel between satellite and ground via the UHF transceiver.
The firmware accepts CSP packets from the network stack, destined either for the UHF module itself (and processes them)
or for the ground (and packages them for transmission and stores them for next available downlink transmission, whereupon it sends them to the transceiver
chip using interrupt-driven SPI).
The firmware receives framed CSP telecommands from the ground, and processes packets destined for itself, passing other packets to the network stack.

Secondary functions include temperature monitoring, fault detection and handling, as well as basic functions common to all the satellite's
modules like housekeeping data collection.

The ADF7021 transceiver chip configuration code has been ported from BlueBox open source code (https://github.com/satlab/bluebox/).

## Communications

This early design of communications protocols favours simplicity of code and minimal restrictions enforced by software, while
avoiding locking out the ability to add complexity or restrictions in later designs if needed.
A goal is to have communications working functionally and reliably early on, when used by well-behaved peripherals (ie. which follow the guidelines below),
after which additional feature design can be considered where necessary.
A goal is to avoid trying to provide a general-purpose networking link that can handle arbitrary peripheral behaviour and many edge cases,
to avoid overextending our development resources.

The firmware is designed assuming half-duplex, unreliable communications.

There are 5 modes that the transceiver firmware can be in:

- Off
- Listening, to receive an initial telecommand
- Waiting, to receive additional telecommand when more are expected
- Receiving telecommands
- Transmitting telemetry.

For beacons, the satellite can transmit only when beacons are enabled,
transmitting is enabled, and the transceiver is in Off or Listening mode.
For other telemetry, assuming transmitting is enabled, the satellite
should transmit data as soon as it is requested, and if the transceiver is not in Receiving mode.
The satellite returns to Listening mode if ground control ends a communications session, or after a configurable timeout has passed with no telecommands.


While it is transmitting, the satellite is expected not to be able to receive commands from ground control, so the timing of its transmissions
must be restricted, with precedence given to ground control.
Except in the case of beacons, ground control should never be in a situation
where it must wait for the satellite to finish transmitting unneeded data, before ground is able to issue commands.
To ensure this, the satellite should transmit data only when ground control has asked for it, via telecommands.
Since it would be infeasible for UHF firmware to verify that any data from other modules are only
responding to telecommands, this is a guideline for all of the satellite modules to implement.

In case the satellite has urgent information it needs to transmit, this state should be broadcast in beacons and in response to initiation
of a communications session. Ground control can then request the urgent data at its perogative.

In determining transmission timing, ground should be treated as master and the satellite as slave.

Ground control can further restrict the satellite from transmitting by disabling transmissions, in which case telemetry data
can remain in RAM until transmissions are re-enabled, ground control requests it to be purged, or the buffer is full and older messages
are overwritten by newer ones.

If this strategy is insufficient, other techniques could be added later, such as implementing a message priority system, etc.


### Transmission data format
Subject to future design changes, the framing of a single transmission in either direction consists of:

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

The Data field may be encrypted, with the intention that telecommands will be encrypted.

A risk is that errors may occur in the Data length field, which must be detected and handled reasonably.
If possible, the error correction may be applied to the length field as well.

The Data will likely consist exclusively of CSP packets.

Within this format, additional ways of increasing throughput include a) potentially allowing appending multiple (small) CSP packets into one Data block,
if this is feasible for the network stack, and b) using a command format that allows multple commands within a single CSP packet.


### Receiving telecommands
The firmware will dispatch a received CSP packet as soon as it is completely received, rather than waiting for the full transmission to finish.

### Transmitting telemetry
The satellite will begin transmitting as soon as a framed data block is buffered, and if the transceiver is ready to transmit.
The firmware may accept additional telemetry to be transmitted, while it is already transmitting, in which case the buffered data are
appended to the current transmission. The transmission ends when all buffered telemetry has been sent; the satellite
then sends an end marker and returns to Waiting mode.


### Unreliability
Due to the unreliable nature of radio communications, transmission success cannot be guaranteed.
If ground control expects a reply to a command, it may not receive one, and may not be able to tell if the reply was lost,
or if the satellite failed to receive the command.

To mitigate unreliability, all telecommands that request data should be repeatable. The following guidelines are suggested for all satellite telemetry:
In the case of acknowledgements of telecommands, ground should be able to request state information to verify execution of previously issued telecommands.
In the case of transient data, subsequent requests are assumed to return different data.
In the case of stored telemetry, the data should not be deleted after transmission, but be left for re-retrieval until the storage space is needed.


### Flexibility

The protocols outlined here allow some flexibility in how ground control interacts with the satellite.
For example, if increased reliability is needed, ground can issue one telecommand per transmission,
and receive telemetry in response before issuing further telecommands.
If increased throughput is needed, ground can append a list of telecommands in a single transmission and receive all requested telemetry at once.

Likely, adjustable maximum transmission lengths will need to be implemented. How this will be handled is TBD.

## Commands

The set of telecommands implemented in the firmware is TBD.

It will include general commands common to all boards, such as retrieval of current state and logged information, setting configuration data,
and uploading firmware. The commands should follow a standard format that is shared by all boards.
The firmware could potentially be able to process commands without distinguishing which CSP node it came from (whether ground or another satellite
module), and send the reply back to the same node, if this is deemed worth implementing.

The set of transceiver parameters that is ground-configurable is TBD. A risk is that it may be possible to incorrectly configure the satellite so that
further communication is impossible. A last-defence solution is to allow watchdog code to detect this and 
force a boot using a known-working configuration vector.

The protocols do not enforce a command and data format on the other boards, including whether either are binary or text messages.
To handle interspersed telemetry from different nodes on the CSP network, the CSP header indicates the source of the telemetry.
An additional guideline is that the context of replies to telecommands should be identifyable independent of the telecommand.
For example, a command such as "eps.get-battery-v" should not
return a context-dependent message such as "15.0", but instead something like "eps.battv 15.0".

### Interaction

A typical interaction between ground and satellite depends on how ground control uses the system.

Scenario 1:

- Ground transmits a single telecommand and waits to receive data.
- The UHF module receives the data and passes it to the network stack, and returns to Waiting mode.
- A single CSP message is received by the UHF module from the network, and is transmitted to ground, and the firmware continues Waiting.

Scenario 2:

- Ground transmits several telecommands in a single transmission, and waits to receive telemetry.
- The UHF module receives multiple data blocks, and passes them to the network stack while it continues receiving.
- While it is still receiving telecommands, the UHF module begins to receive telemetry from the network, which it stores in the transmit buffer
until it can be later transmitted.
- The reception of telecommand data ends, and the satellite immediately begins to transmit buffered telemetry.
- The UHF module continues to receive telemetry from other modules, which is appended to the transmit buffer.
- Once all of the buffered data has been transmitted, the firmware returns to Waiting.


When either ground or satellite is receiving data, and has buffered transmit data, it 
will begin transmitting as soon as it finishes receiving data.
It remains possible for the UHF to send one CSP packet to ground, finish the transmission, and then have another CSP packet to transmit,
for example if a reply to a telecommand was significantly delayed. As designed above, the satellite would send the first
telemetry, return to Waiting, and if it hasn't begun Receiving a telecommand from ground, it will immediately transmit the second telemetry.
If ground has begun transmitting a telecommand after receiving the first telemetry, then both ground and satellite will be
transmitting, and both will fail to receive the other's message.

To avoid this situation:

- Satellite modules should generally immediately respond to requests for data without significant delay.
- Commands that are known to ground to have a delay can be waited for by ground control.
- If desired, commands that take some time to process can be split into "begin execution" and "request data" commands, or can be designed
to be called multiple times, ie. a command that responds with data or a "data not ready" reply.
- If the above steps are insufficient, other rules might be implemented. For example, the satellite might be restricted from sending two
transmissions in a row, and instead buffer the second transmission until after receiving another telecommand. This way, ground knows that they
can (and should) send another transmission as soon as it is done receiving telemetry. The satellite would strictly
follow a turn-taking policy of transmission and reception, while the ground could do so selectively; this way, the ground is exclusively responsible for
handling policy breakdown (eg. missed replies, etc.).

These protocols should be torture-tested in the lab with a wide range of short and long CSP packets, individual and groups of data blocks,
and various artificial delays, to determine if the design is robust enough without needing to add additional rules for the satellite
to enforce.


## Fault Detection

The UHF hardware includes a watchdog timer chip that must be reset once every 1.12 to 2.24 s (depending on chip manufacturing variation),
otherwise the MCU will be reset.
The watchdog will be reset once per main loop of the firmware, and only if several checkpoints are properly reached in code.
A risk is that an interrupt handler may malfunction without starving out the main loop, which may go undetected.
To mitigate this, any critical interrupt handlers should include their own watchdog flags that the main loop is able to monitor.

The firmware should also monitor time since last contact with ground control, and consider it a fault if it exceeds some ground-configured
maximum time.

A low-priority task will routinely perform a CRC check of stored firmware and any copies of satellite configuration vectors
(likely stored in ReRAM), and mark them as unsafe if a fault is detected.

The boot loader is expected to be able to select from several different firmware images, including at least the latest uploaded image,
and the original launch image. Flags stored in ReRAM (for reliability) will indicate things such as whether a particular image is considered
safe, or how many times it has been loaded without successful communication with ground, etc. Exact criteria for choosing which image
to boot are TBD.

For reliability and reduced susceptibility to radiation, images should be run directly from long-term storage (likely Flash) rather than RAM.

