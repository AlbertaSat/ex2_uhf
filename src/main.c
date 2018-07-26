#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
//#include "em_emu.h"

#include "adf7021.h"

int main(void)
{
  /* Chip errata */
  CHIP_Init();

  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Infinite loop */
  while (1) {
  }
}
