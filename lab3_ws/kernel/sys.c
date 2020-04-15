#include "stdint.h"
#include "uart.h"
int32_t sys_getfreq()
{
	int cntfrq = 0;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq)::);
	return cntfrq;
}

uint64_t sys_getcurrentcount()
{
	unsigned long long cntpct;
	/*
   * Get CNTPCT_EL0
   */
	asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct)::);
	return cntpct;
}

#define set(a, b) (*a = b)
#define LOCAL_TIMER_CONTROL (uint32_t *)0x40000034
#define get(a, b) (b = *a)
void sys_local_timer_init()
{
	unsigned int flag = 0x30000000; // enable timer and interrupt.
	unsigned int reload = 25000000;
	uart_puts("start\n");
	set(LOCAL_TIMER_CONTROL, flag | reload);
}

#define set(a, b) (*a = b)
#define get(a, b) (b = *a)
#define LOCAL_TIMER_CONTROL (uint32_t *)0x40000034
#define SYSTEM_TIMER_COMPARE1 (uint32_t *)0x3f003010
#define SYSTEM_TIMER_CLO (uint32_t *)0x3f003004
#define IRQ_ENABLE0 (uint32_t *)0x3f00b210

void sys_sys_timer_init()
{
	unsigned int t;
	get(SYSTEM_TIMER_CLO, t);
	set(SYSTEM_TIMER_COMPARE1, t + 2500000);
	set(IRQ_ENABLE0, 1 << 1);
}

#define ARM_TIMER_LOAD (uint32_t *)0x3f00b400
#define ARM_TIMER_CONTROL (uint32_t *)0x3f00b408

#define IRQ_ENABLE2 (uint32_t *)0x3f00b218

void sys_arm_timer_init()
{
	set(ARM_TIMER_CONTROL, (1 << 7) | (1 << 5) | (1 << 1));
	set(ARM_TIMER_LOAD, 500000);
	set(IRQ_ENABLE2, 1);
}

#define PM_PASSWORD 0x5a000000
#define PM_RSTC (unsigned int *)0x3F10001c
#define PM_WDOG (unsigned int *)0x3F100024

int sys_reset(int tick)
{
	int wait_time = tick;
	set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
	set(PM_WDOG, PM_PASSWORD | wait_time); // number of watchdog tick
	return wait_time;
}
void sys_cancel_reset()
{
	set(PM_RSTC, PM_PASSWORD | 0); // full reset
	set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
}