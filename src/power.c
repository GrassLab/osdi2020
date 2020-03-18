//#define PM_PASSWORD 0x5a000000
//#define PM_RSTC 0x3F10001c
//#define PM_WDOG 0x3F100024

#include "util.h"
#include "mini_uart.h"

#define PM_RSTC         ((volatile unsigned int*)(BASE + 0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(BASE + 0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(BASE + 0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020

void reset(int tick){
    unsigned int r;
    // trigger a restart by instructing the GPU to boot from partition 0
    r = *PM_RSTS; r &= ~0xfffffaaa;
    *PM_RSTS = PM_WDOG_MAGIC | r;
    *PM_WDOG = PM_WDOG_MAGIC | tick;
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}

//void reset(int tick){ // reboot after watchdog timer expire
//  put32(PM_RSTC, PM_PASSWORD | 0x20); // full reset
//  put32(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick
//}

//void cancel_reset() {
//  put32(PM_RSTC, PM_PASSWORD | 0); // full reset
//  put32(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
//}

