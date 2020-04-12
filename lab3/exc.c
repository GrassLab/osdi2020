#include "exc.h"
#include "irq.h"

void exc_dispatcher(uint64_t identifier)
{
  /* identifier is 0xYZ */
  /* Y is exception level */
  /* Z is offset of Z * 0x80 */
  uint8_t level = (uint8_t)(identifier & 0x20) >> 4;
  uint8_t type = (uint8_t)(identifier & 0xf);
  switch(level)
  {
  case 3:
    break;
  case 2:
    switch(type)
    {
    case 4:
      exc_EL2_current_EL_SP_EL2_sync();
      return;
    case 5:
      exc_EL2_current_EL_SP_EL2_irq();
      return;
    default:
      exc_not_implemented();
      return;
    }
    break;
  case 1:
    break;
  case 0: /* EL 0 */
  default:
    break;
  }
  return;
}

void exc_not_implemented(void)
{
  uart_puts("Exception handler not Implement\n");
  return;
}

void exc_EL2_current_EL_SP_EL2_sync(void)
{
  char string_buff[0x20];
  uint64_t ELR_EL2, ESR_EL2;
  uint8_t exception_class;
  uint32_t exception_iss;
  asm volatile("mrs %0, elr_el2\n"
               "mrs %1, esr_el2\n":
               "=r"(ELR_EL2), "=r"(ESR_EL2));
  exception_class = (uint8_t)(ESR_EL2 >> 26);
  exception_iss = ESR_EL2 & 0x1ffffff;

  uart_puts("Exception return address: ");
  string_ulonglong_to_hex_char(string_buff, ELR_EL2);
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

  return;
}

void exc_EL2_current_EL_SP_EL2_irq(void)
{
  irq_el2_handler();
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

