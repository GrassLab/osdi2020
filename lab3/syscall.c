#include "syscall.h"
#include "uart.h"
#include "string_util.h"
#include "timer.h"
#include "irq.h"

int syscall_exc(uint64_t ELR_EL1, uint8_t exception_class, uint32_t exception_iss)
{
  char string_buff[0x20];

  uart_puts("Exception return address: ");
  string_ulonglong_to_hex_char(string_buff, ELR_EL1);
  uart_puts(string_buff);
  uart_putc('\n');

  uart_puts("Exception class (EC): ");
  string_ulonglong_to_hex_char(string_buff, exception_class);
  uart_puts(string_buff);
  uart_putc('\n');

  uart_puts("Instruction specific syndrome (ISS): ");
  string_ulonglong_to_hex_char(string_buff, exception_iss);
  uart_puts(string_buff);
  uart_putc('\n');

  return 0;
}

int syscall_timer_int(void)
{
  static int core_timer_enabled = 0;

  if(!core_timer_enabled)
  {
    timer_enable_core_timer();
    core_timer_enabled = 1;
  }
  timer_set_core_timer(3);
  irq_el1_enable();

  return 0;
}

