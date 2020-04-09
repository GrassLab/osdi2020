#define PM_PASSWORD 0x5a000000
#define PM_RSTC (int *)0x3F10001c
#define PM_WDOG (int *)0x3F100024
#define set(a, b) *a = b

void reset(int tick)
{ // reboot after watchdog timer expire
  set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
  set(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick
}

void cancel_reset() 
{
  set(PM_RSTC, PM_PASSWORD | 0); // full reset
  set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
}