#include "system.h"
/*****
This snippet of code only works on real rpi3, not on QEMU.
*****/
void reset(int tick)
{ // reboot after watchdog timer expire
  unsigned int r;
  r = *PM_RSTS;
  r &= ~0xfffffaaa;
  *PM_RSTC = PM_PASSWORD | 0x00000020; // full reset
  *PM_RSTS = PM_PASSWORD | r;
  *PM_WDOG = PM_PASSWORD | 10; // number of watchdog tick
}

void cancel_reset()
{
  unsigned int r;
  r = *PM_RSTS;
  r &= ~0xfffffaaa;
  *PM_RSTC = PM_PASSWORD | 0; // full reset
  *PM_RSTS = PM_PASSWORD | r;
  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}
