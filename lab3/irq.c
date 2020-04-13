#include <stdint.h>
#include "meta_macro.h"
#include "string_util.h"
#include "irq.h"
#include "uart.h"
#include "timer.h"

void irq_el2_enable(void)
{
  // Clear interrupt mask for d, a, (i), f
  asm volatile("msr daifclr, #0x2");
}

void irq_el1_handler(void)
{
#define DISABLE_TIMER_COUNT 5
  char string_buff[0x10];
  static int core_timer_count = 0;
  static int local_timer_count = 0;
  if(CHECK_BIT(*LOCAL_TIMER_CONTROL_REG, 31))
  {
    ++local_timer_count;
    uart_puts("ARM local time interrupt \"");
    string_longlong_to_char(string_buff, local_timer_count);
    uart_puts(string_buff);
    uart_puts("\" received\n");
    timer_clear_local_timer_int_and_reload();
    if(local_timer_count == DISABLE_TIMER_COUNT)
    {
      timer_disable_local_timer();
      local_timer_count = 0;
    }
  }
  else
  {
    ++core_timer_count;
    uart_puts("ARM core time interrupt \"");
    string_longlong_to_char(string_buff, core_timer_count);
    uart_puts(string_buff);
    uart_puts("\" received\n");
    if(core_timer_count == DISABLE_TIMER_COUNT)
    {
      timer_expire_core_timer();
      core_timer_count = 0;
    }
    else
    {
    timer_set_core_timer(1);
    }
  }
  return;
}

