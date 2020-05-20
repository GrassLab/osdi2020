#include "exc.h"
#include "uart.h"
#include "irq.h"
#include "sys.h"
#include "timer.h"

void exc_dispatcher(uint64_t identifier, struct trapframe_struct * trapframe)
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
    case 5:
      exc_EL1_same_level_EL_SP_EL1_irq();
      return;
    case 8:
      exc_EL1_lower_aa64_EL_SP_EL1_sync(trapframe);
      return;
    case 9:
      exc_EL1_lower_aa64_EL_SP_EL1_irq();
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

void exc_EL1_same_level_EL_SP_EL1_irq(void)
{
  irq_el1_handler();
  return;
}

void exc_EL1_lower_aa64_EL_SP_EL1_sync(struct trapframe_struct * trapframe)
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

  if(exception_class == 0x24)
  {
    char string_buff[0x20];

    uint64_t FAR_EL1, TPIDR_EL1;
    asm volatile("mrs %0, far_el1\n"
                 "mrs %1, tpidr_el1\n":
                 "=r"(FAR_EL1), "=r"(TPIDR_EL1));
    uart_puts(ANSI_BG_RED ANSI_BLACK"[Exception]"ANSI_RESET" Task ID: ");
    string_longlong_to_char(string_buff, (long)TPIDR_EL1);
    uart_puts(string_buff);
    uart_puts(" triggered page fault at(FAR_EL1) ");
    string_ulonglong_to_hex_char(string_buff, FAR_EL1);
    uart_puts(string_buff);
    uart_puts(". Initiate self destruct.\n");
    sys_exit(1);
  }

  if(exception_class != 0x15) /* Not via aarch64 svc */
  {
    exc_not_implemented(0x18);
  }

  switch(exception_imm)
  {
  case 1:
    sys_exc(ELR_EL1, exception_class, exception_iss);
    break;
  case 2:
    sys_timer_int();
    break;
  case SYS_UART_PUTS:
    sys_uart_puts((char *)trapframe -> x1);
    break;
  case SYS_UART_GETS:
    sys_uart_gets((char *)trapframe -> x1, (char)trapframe -> x2, (unsigned)trapframe -> x3);
    break;
  case SYS_EXEC:
    sys_exec_todo((void(*)())trapframe -> x1);
    break;
  case SYS_FORK:
    sys_fork(trapframe);
    break;
  case SYS_EXIT:
    sys_exit((int)trapframe -> x1);
    break;
  case SYS_SIGNAL:
    sys_signal((int)trapframe -> x1, (int)trapframe -> x2);
    break;
  case SYS_PRINT_MMU_PAGE:
    sys_print_mmu_page_used();
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

