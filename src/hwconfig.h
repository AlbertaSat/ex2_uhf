/*
	Contains settings for different possible hardware configurations of ex2 entities, including launch craft,
	prototype satellites, and prototype ground station hardware. Settings include pin configurations, MCU settings,
	enabling of optional HW features, and of debugging/dev features.

	The different configurations will be identified by having the following #define'd:

	???GG11???	First in-house designed prototype UHF board; initial satellite prototype.
	???PG12???	Pearl prototype board. To be used as prototype ground station attached to ADF7021 transceiver.
	???GG1???	Older M3 Giant Gecko GG1 boards. For general development; may be unsupported later.

*/

#ifndef HWCONFIG_H
#define HWCONFIG_H


// Some USART etc functions need to be named such as USART5_TX_IRQHandler,
// but are listed as CAT( TRX_USART_,TX_IRQHandler ) because the USARTn number is different in different configs.
// Indirect macro substitution is used so that TRX_USART_ etc. is expanded before being cat'd.
#define CAT_DIRECT( a, b, ... ) a##b##__VA_ARGS__
#define CAT( a, b, ... ) CAT_DIRECT( a, b, __VA_ARGS__ )


#ifdef GG11							// In-house UHF board with GG11 M4 MCU
#include "hwconfig-ex2-flatsat01.h"

#elif EFM32PG12B500F1024GL125		// Pearl Gecko SLSTK3402A PG12 M4 dev board
#include "hwconfig-pg12-groundstation.h"

#elif EFM32GG990F1024				// Older Giant Gecko EFM32GG-STK3700 M3 dev board
#include "hwconfig-gg-devboard.h"

#endif

#endif //HWCONFIG_H

