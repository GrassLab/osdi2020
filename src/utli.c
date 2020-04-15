#include "shared_variables.h"

#define PM_PASSWORD     0x5a000000
#define PM_RSTC         ((volatile unsigned int*)0x3F10001c)
#define PM_WDOG         ((volatile unsigned int*)0x3F100024)

float get_timestamp() {
    asm volatile ("svc #5");
    return (float) cntpct_el0 / cntfrq_el0;
}

void reset(){ // reboot after watchdog timer expire
    *PM_RSTC = PM_PASSWORD | 0x20; // full reset
    *PM_WDOG = PM_PASSWORD | 100;   // number of watchdog tick
}

void cancel_reset() {
    *PM_RSTC = PM_PASSWORD | 0; // full reset
    *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}