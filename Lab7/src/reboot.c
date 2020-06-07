#include "include/peripherals/reboot.h"
#include "include/utils.h"

void reset(int tick){ // reboot after watchdog timer expire
  put32(PM_RSTC, PM_PASSWORD | 0x20); // full reset
  put32(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick
}

void cancel_reset() {
  put32(PM_RSTC, PM_PASSWORD | 0); // full reset
  put32(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
}
