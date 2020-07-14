#include "exc.h"
#include "uart.h"
#include "sys.h"
#include "string_util.h"
#include "mmu.h"

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
    exc_not_implemented(identifier);
    break;
  case 2:
    exc_not_implemented(identifier);
    break;
  case 1:
    switch(type)
    {
    case 4:
      exc_EL1_same_level_EL_SP_EL1_sync();
      return;
    case 8:
      exc_EL1_lower_aa64_EL_SP_EL1_sync();
      return;
    default:
      exc_not_implemented(identifier);
    }
    break;
  case 0: /* EL 0 */
  default:
    exc_not_implemented(identifier);
    break;
  }
  return;
}

void exc_not_implemented(uint64_t code)
{
  char string_buff[0x20];
  uint64_t ELR_EL1, ESR_EL1, FAR_EL1, TPIDR_EL1;
  asm volatile("mrs %0, elr_el1\n"
               "mrs %1, esr_el1\n"
               "mrs %2, far_el1\n"
               "mrs %3, tpidr_el1\n":
               "=r"(ELR_EL1), "=r"(ESR_EL1), "=r"(FAR_EL1), "=r"(TPIDR_EL1));
  uart_puts_blocking("Exception handler not implemented. Code: ");
  string_ulonglong_to_hex_char(string_buff, code);
  uart_puts_blocking(string_buff);
  uart_puts_blocking("\n");
  uart_puts_blocking("Exception return address: ");
  string_ulonglong_to_hex_char(string_buff, ELR_EL1);
  uart_puts_blocking(string_buff);
  uart_puts_blocking("\n");
  uart_puts_blocking("ESR_EL1: ");
  string_ulonglong_to_hex_char(string_buff, ESR_EL1);
  uart_puts_blocking(string_buff);
  uart_puts_blocking("\n");
  uart_puts_blocking("FAR_EL1: ");
  string_ulonglong_to_hex_char(string_buff, FAR_EL1);
  uart_puts_blocking(string_buff);
  uart_puts_blocking("\n");
  uart_puts_blocking("TPIDR_EL1: ");
  string_ulonglong_to_hex_char(string_buff, TPIDR_EL1);
  uart_puts_blocking(string_buff);
  uart_puts_blocking("\n");

  uart_puts_blocking("Enter busy infinite loop for debug purpose\n");
  while(1);
  return;
}

void exc_EL1_same_level_EL_SP_EL1_sync(void)
{
  char string_buff[0x20];
  uint64_t ELR_EL1, ESR_EL1, FAR_EL1, TPIDR_EL1;
  asm volatile("mrs %0, elr_el1\n"
               "mrs %1, esr_el1\n"
               "mrs %2, far_el1\n"
               "mrs %3, tpidr_el1\n":
               "=r"(ELR_EL1), "=r"(ESR_EL1), "=r"(FAR_EL1), "=r"(TPIDR_EL1));
  uart_puts_blocking("\nException return address: ");
  string_ulonglong_to_hex_char(string_buff, ELR_EL1);
  uart_puts_blocking(string_buff);
  uart_puts_blocking("\n");
  uart_puts_blocking("ESR_EL1: ");
  string_ulonglong_to_hex_char(string_buff, ESR_EL1);
  uart_puts_blocking(string_buff);
  uart_puts_blocking("\n");
  uart_puts_blocking("FAR_EL1: ");
  string_ulonglong_to_hex_char(string_buff, FAR_EL1);
  uart_puts_blocking(string_buff);
  uart_puts_blocking("\n");
  uart_puts_blocking("TPIDR_EL1: ");
  string_ulonglong_to_hex_char(string_buff, TPIDR_EL1);
  uart_puts_blocking(string_buff);
  uart_puts_blocking("\n");
}

void exc_EL1_lower_aa64_EL_SP_EL1_sync(void)
{
  uint64_t ELR_EL1, ESR_EL1;
  uint8_t exception_class;
  asm volatile("mrs %0, elr_el1\n"
               "mrs %1, esr_el1\n":
               "=r"(ELR_EL1), "=r"(ESR_EL1));
  exception_class = (uint8_t)(ESR_EL1 >> 26);

  if(exception_class != 0x15) /* Not via aarch64 svc */
  {
    exc_not_implemented(0x18);
  }

  return;
}

