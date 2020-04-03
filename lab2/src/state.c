#include "state.h"
#define MMIO_BASE       0x3F000000
#define PM_PASSWORD 0x5a000000
#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_RSTC_FULLRST 0x00000020
#define set(a, b) *a = b

void reset()
{ // reboot after watchdog timer expire
  // set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
  // set(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick
  // trigger a restart by instructing the GPU to boot from partition 0
  *PM_WDOG = PM_PASSWORD | 100;
  *PM_RSTC = PM_PASSWORD | PM_RSTC_FULLRST;
}

void cancel_reset() 
{
  set(PM_RSTC, PM_PASSWORD | 0); // full reset
  set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
}