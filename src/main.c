#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "bsp.h"

#include "adf7021.h"

int main(void)
{
  /* Chip errata */
  CHIP_Init();

#if defined(EX2_DEVBOARD)
  // Show debug info

#endif //defined(EX2_DEVBOARD)

  // Test ADF7021 chip
  adf_set_power_on(XTAL_FREQ);
  adf_configure();
  adf_set_rx_mode();


  /* Infinite loop */
  while (1) {
  }
}
