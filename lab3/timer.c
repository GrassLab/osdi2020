#include <stdint.h>
#include "timer.h"

void timer_enable_and_set_local_timer(void)
{
  /* default route to core 0 irq ? */
  /* quartz crystal operates at 19.2 MHz, counter +1 per edge -> 38.4 MHz */
  uint32_t flag = 0x30000000; /* enable timer and interrupt. [29] enable interrupt [28] timer enable */
  uint32_t reload = 27000000; /* Reload value 28-bit */
  *LOCAL_TIMER_CONTROL_REG = flag | reload;
}

void timer_disable_local_timer(void)
{
  *LOCAL_TIMER_CONTROL_REG = 0;
  return;
}

void timer_clear_local_timer_int_and_reload(void)
{
  *LOCAL_TIMER_IRQ_CLR = 0xc0000000; /* [31] clear int [30] reload timer */
  return;
}
