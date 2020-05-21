#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define PM_PASSWORD 0x5a000000

#define PM_RSTC ((volatile unsigned int *)(0x3F10001c))
#define PM_RSTS ((volatile unsigned int *)(0x3F100020))
#define PM_WDOG ((volatile unsigned int *)(0x3F100024))

/*****
This snippet of code only works on real rpi3, not on QEMU.
*****/
void reset(int tick);

void cancel_reset();
#endif
