#include "exc.h"
#include "irq.h"
#include "syscall.h"
#include "timer.h"

void exc_dispatcher(uint64_t identifier)
{
  /* identifier is 0xYZ */
  /* Y is exception level */
  /* Z is offset of Z * 0x80 */
  uint8_t level = (uint8_t)(identifier & 0x30) >> 4;
  uint8_t type = (uint8_t)(identifier & 0xf);
  switch(level)
  {
  case 3:
    break;
  case 2:
    break;
  case 1:
    switch(type)
    {
    case 8:
      exc_EL1_lower_aa64_EL_SP_EL1_sync();
      return;
    case 9:
      exc_EL1_lower_aa64_EL_SP_EL1_irq();
      return;
    default:
      exc_not_implemented(identifier);
      return;
    }
    break;
  case 0: /* EL 0 */
  default:
    break;
  }
  return;
}

void exc_not_implemented(uint64_t code)
{
  char string_buff[0x20];
  uint64_t ELR_EL1, ESR_EL1;
  asm volatile("mrs %0, elr_el1\n"
               "mrs %1, esr_el1\n":
               "=r"(ELR_EL1), "=r"(ESR_EL1));
  uart_puts("Exception handler not implemented. Code: ");
  string_ulonglong_to_hex_char(string_buff, code);
  uart_puts(string_buff);
  uart_putc('\n');
  uart_puts("Exception return address: ");
  string_ulonglong_to_hex_char(string_buff, ELR_EL1);
  uart_puts(string_buff);
  uart_putc('\n');
  uart_puts("ESR_EL1: ");
  string_ulonglong_to_hex_char(string_buff, ESR_EL1);
  uart_puts(string_buff);
  uart_putc('\n');

  uart_puts("Enter busy infinite loop for debug purpose\n");
  while(1);
  return;
}

void exc_EL1_lower_aa64_EL_SP_EL1_sync(void)
{
  uint64_t ELR_EL1, ESR_EL1;
  uint8_t exception_class;
  uint32_t exception_iss;
  uint16_t exception_imm;
  asm volatile("mrs %0, elr_el1\n"
               "mrs %1, esr_el1\n":
               "=r"(ELR_EL1), "=r"(ESR_EL1));
  exception_class = (uint8_t)(ESR_EL1 >> 26);
  exception_iss = ESR_EL1 & 0x1ffffff;
  exception_imm = ESR_EL1 & 0xffff;

  if(exception_class != 0x15) /* Not via aarch64 svc */
  {
    uart_puts("Unhandled exception class\n");
    return;
  }

  switch(exception_imm)
  {
  case 1:
    syscall_exc(ELR_EL1, exception_class, exception_iss);
    break;
  case 2:
    syscall_timer_int();
    break;
  default:
    uart_puts("Unhandled svc immediate value\n");
    break;
  }

  return;
}


void exc_EL1_lower_aa64_EL_SP_EL1_irq(void)
{
  irq_el1_handler();
  return;
}

void exc_EL2_enable_physical_interrupt(void)
{
  asm volatile(
    "mrs x0, hcr_el2\n"
    "orr x0, x0 ,#0x10\n"
    "msr hcr_el2, x0"); // IMO is bit 4
  return;
}
