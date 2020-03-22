#define PM_PASSWORD 0x5a000000
#define PM_RSTC (int *)0x3F10001c
#define PM_WDOG (int *)0x3F100024
#define set(a, b) *a = b

void reset(int tick);
void cancel_reset();