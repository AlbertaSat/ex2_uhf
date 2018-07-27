#include <stdio.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_emu.h"
#include "bsp.h"

#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"

#include "adf7021.h"

int main(void)
{
  /* Chip errata */
  CHIP_Init();

  // The code in this file should only be used for prototyping.
  EFM_ASSERT( EX2_DEVBOARD );

  // Init display module.
  DISPLAY_Init();

  // Retarget stdio to text display.
  if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK)
  {
    while (1) ;
  }

  printf("Test\n");

  // Test ADF7021 chip
  adf_set_power_on(XTAL_FREQ);
  adf_configure();
  adf_set_rx_mode();

  printf("Test complete!\n");
}
