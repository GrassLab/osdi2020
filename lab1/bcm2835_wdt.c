#include "bcm2835_wdt.h"

void bcm2835_reboot(int ticks){
	*PM_RSTC = PM_PASSWORD | WRCFG_FULL_RESET;
	*PM_WDOG = PM_PASSWORD | ticks;
}