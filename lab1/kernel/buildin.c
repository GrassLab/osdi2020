#include "kernel.h"
#include "timer.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC (unsigned int *)0x3F10001c
#define PM_WDOG (unsigned int *)0x3F100024
#define set(a, b) (*a = b)

void hello()
{
	printk("Hello World!!\n");
}

void reset()
{ // reboot after watchdog timer expire
	float second = 1.5;
	printk("reset in %.2f second.\n", second);
	int wait_time = second * getFrequency();
	set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
	set(PM_WDOG, PM_PASSWORD | wait_time); // number of watchdog tick
}

void cancel_reset()
{
	set(PM_RSTC, PM_PASSWORD | 0); // full reset
	set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
}

void print_info()
{
	printk("Board: Raspberry Pi 3 B+\n"
	       "OS: Bare Metal\n"
	       "Maintainer: njt@2020\n"
	       "CNTFRQ: %d\n"
	       "CNTPCT: %d\n",
	       getFrequency(), getCurrentCount());
}

void getTimestamp()
{
	printk("[%.8f]\n", getCurrentTime());
}
