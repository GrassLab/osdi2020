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
