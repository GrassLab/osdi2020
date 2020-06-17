#include "gpio.h"

#define PM_WDOG_MAGIC 0x5a000000
#define PM_RSTC_FULLRST 0x00000020
#define PM_PASSWORD 0x5a000000
#define PM_WDOG ((volatile unsigned int*)(MMIO_BASE + 0x00100024))
#define PM_RSTC ((volatile unsigned int*)(MMIO_BASE + 0x0010001c))
#define PM_RSTS ((volatile unsigned int*)(MMIO_BASE + 0x00100020))

void reset()
{
    unsigned int r;
    // trigger a restart by instructing the GPU to boot from partition 0
    r = *PM_RSTS;
    r &= ~0xfffffaaa;
    *PM_RSTS = PM_WDOG_MAGIC | r; // boot from partition 0
    *PM_WDOG = PM_WDOG_MAGIC | 10;
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}
