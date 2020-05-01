#include "type.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile uint32_t *)0x3F10001c)
#define PM_WDOG ((volatile uint32_t *)0x3F100024)

void reset(uint32_t tick)
{                                  // reboot after watchdog timer expire
    *PM_RSTC = PM_PASSWORD | 0x20; // full reset
    *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
}

void cancelReset()
{
    *PM_RSTC = PM_PASSWORD | 0; // full reset
    *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}