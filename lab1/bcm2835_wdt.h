#ifndef BCM2835_WDT
#define BCM2835_WDT

#define PM_PASSWORD 0x5A000000

#define WRCFG_NO_RESET 0x00
#define WRCFG_QUICK_RESET 0x10
#define WRCFG_FULL_RESET 0x20
#define WRCFG_HARD_RESET 0x30

#define PM_RSTC ((volatile unsigned int*)0x3F10001C)
#define PM_WDOG ((volatile unsigned int*)0x3F100024)

void bcm2835_reboot(int ticks);

#endif