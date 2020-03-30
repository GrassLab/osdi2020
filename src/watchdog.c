/*
 * watchdog.c
 * Provides watchdog interactions such as reboot and poweroff
 */

#include "mm.h"
#include "time.h"

enum {
    PM_PASSWORD      = 0x5a000000,
    PM_RSTC          = 0x3F10001c,
    PM_RSTS          = 0x3F100020,
    PM_WDOG          = 0x3F100024
};

void reboot(){ // reboot after watchdog timer expire
    mm_write(PM_WDOG, PM_PASSWORD | 10); // reset after 10 ticks
    mm_write(PM_RSTC, PM_PASSWORD | 0x20); // full reset
    delay(1000); // idle for 1 second
}


void shutdown() {
    mm_write(PM_RSTS, PM_PASSWORD | 0x555); // halt
    reboot();
}
