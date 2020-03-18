#include "gpio.h"

#define PM_RSTC         ((volatile unsigned int*)MMIO_BASE + 0x0010001c)
#define PM_RSTS         ((volatile unsigned int*)MMIO_BASE + 0x00100020)
#define PM_WDOG         ((volatile unsigned int*)MMIO_BASE + 0x00100024)
#define PM_RSTC_FULLRST 0x00000020
#define PM_WDOG_MAGIC   0x5a000000 
/**
 * Reboot
 */
void reset()
{
    unsigned int r;
    r = *PM_RSTS;
    r &= ~0xffffaaa;
    *PM_RSTS = PM_WDOG_MAGIC | r;
    *PM_WDOG = PM_WDOG_MAGIC | 1;
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}
