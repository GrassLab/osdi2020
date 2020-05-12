#include "timer.h"

void
local_timer_init ()
{
  unsigned int flag = 0x30000000;	// enable timer and interrupt.
  unsigned int reload = 25000000;
  *LOCAL_TIMER_CONTROL_REG = flag | reload;
}

void
local_timer_handler ()
{
  *LOCAL_TIMER_IRQ_CLR = 0xc0000000;	// clear interrupt and reload.
}

void
core_timer_enable ()
{
  asm volatile ("mov x8, #0\n" "svc #0\n");
}

double
get_time ()
{
  size_t cnt, freq;
  asm volatile ("mov x1, %0\n" "mov x0, %1\n" "mov x8, #1\n"
		"svc #0\n"::"r" (&freq), "r" (&cnt):"x0", "x1", "x8");
  return (double) cnt / (double) freq;
}

void
sys_get_time (size_t *cnt, size_t *freq)
{
  asm volatile ("mrs %0, CNTFRQ_EL0\n"
		"mrs %1, CNTPCT_EL0\n":"=r" (*freq), "=r" (*cnt));
}
