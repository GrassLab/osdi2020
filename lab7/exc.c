#include "exc.h"
#include "uart.h"
#include "irq.h"
#include "sys.h"
#include "timer.h"
#include "exc.h"

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
    case 4:
      exc_EL1_same_level_EL_SP_EL1_sync();
      return;
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

void exc_set_x0(uint64_t retval, struct trapframe_struct * trapframe)
{
  *((uint64_t *)(((uint64_t)trapframe) + X0_TRAPFRAME_OFFSET)) = retval;
}

void exc_EL1_same_level_EL_SP_EL1_sync(void)
{
  uart_tx_flush();
  char string_buff[0x20];
  uint8_t exception_class;
  uint64_t ELR_EL1, ESR_EL1, FAR_EL1, TPIDR_EL1;
  asm volatile("mrs %0, elr_el1\n"
               "mrs %1, esr_el1\n"
               "mrs %2, far_el1\n"
               "mrs %3, tpidr_el1\n":
               "=r"(ELR_EL1), "=r"(ESR_EL1), "=r"(FAR_EL1), "=r"(TPIDR_EL1));
  exception_class = (uint8_t)(ESR_EL1 >> 26);
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

  if(exception_class == 33)
  {
    uint64_t TTBR0_EL1;
    uint64_t PMD0, PMD511;
    uint64_t text_pte_base, stack_pte_base;

    uart_puts_blocking("Instruction abort. dumping TTBR0\n");
    asm volatile("mrs %0, ttbr0_el1": "=r"(TTBR0_EL1));
    string_ulonglong_to_hex_char(string_buff, TTBR0_EL1);
    uart_puts_blocking(string_buff);

    uart_puts_blocking("\nPGD 0 & 511: ");

    PMD0 = *MMU_PA_TO_VA(TTBR0_EL1);
    PMD511 = *MMU_PA_TO_VA((TTBR0_EL1 + 511 * 8));
    string_ulonglong_to_hex_char(string_buff, PMD0);
    uart_puts_blocking(string_buff);
     uart_puts_blocking(" ");
    string_ulonglong_to_hex_char(string_buff, PMD511);
    uart_puts_blocking(string_buff);

    uart_puts_blocking("\nuser_space_mm_struct:\n");
    uart_puts_blocking("pte_text_base: ");
    text_pte_base = (uint64_t)kernel_task_pool[TPIDR_EL1 - 1].user_space_mm.pte_text_base;
    stack_pte_base = (uint64_t)kernel_task_pool[TPIDR_EL1 - 1].user_space_mm.pte_stack_base;
    string_ulonglong_to_hex_char(string_buff, text_pte_base);
    uart_puts_blocking(string_buff);
    uart_puts_blocking("\npte_stack_base: ");
    string_ulonglong_to_hex_char(string_buff, stack_pte_base);
    uart_puts_blocking(string_buff);

    uart_puts_blocking("\nText PTE 0 ~ 6\n");
    for(unsigned pd_idx = 0; pd_idx < 6; ++pd_idx)
    {
      string_ulonglong_to_hex_char(string_buff, *(MMU_PA_TO_VA(text_pte_base + pd_idx * 8)));
      uart_puts_blocking(string_buff);
      uart_puts_blocking(" ");
    }
    uart_puts_blocking("\nStack PTE 509, 510\n");
    string_ulonglong_to_hex_char(string_buff, *(MMU_PA_TO_VA(text_pte_base + 509 * 8)));
    uart_puts_blocking(string_buff);
    uart_puts_blocking(" ");
    string_ulonglong_to_hex_char(string_buff, *(MMU_PA_TO_VA(stack_pte_base + 510 * 8)));
    uart_puts_blocking(string_buff);

    uart_puts_blocking("\nEnter busy infinite loop for debug purpose\n");
    while(1);
  }
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

  irq_int_enable();

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
  case SYS_MALLOC:
    sys_malloc((unsigned)trapframe -> x1);
    break;
  case SYS_FREE:
    sys_free((uint64_t *)trapframe -> x1);
    break;
  case SYS_OPEN:
    exc_set_x0((uint64_t)sys_open((const char *)trapframe -> x1, (int)trapframe -> x2), trapframe);
    break;
  case SYS_CLOSE:
    exc_set_x0((uint64_t)sys_close((int)trapframe -> x1), trapframe);
    break;
  case SYS_WRITE:
    exc_set_x0((uint64_t)sys_write((int)trapframe -> x1, (const void *)trapframe -> x2, (size_t)trapframe -> x3), trapframe);
    break;
  case SYS_READ:
    exc_set_x0((uint64_t)sys_read((int)trapframe -> x1, (void *)trapframe -> x2, (size_t)trapframe -> x3), trapframe);
    break;
  case SYS_LIST:
    exc_set_x0((uint64_t)sys_list((int)trapframe -> x1), trapframe);
    break;
  case SYS_MKDIR:
    exc_set_x0((uint64_t)sys_mkdir((const char *)trapframe -> x1), trapframe);
    break;
  case SYS_CHDIR:
    exc_set_x0((uint64_t)sys_chdir((const char *)trapframe -> x1), trapframe);
    break;
  case SYS_MOUNT:
    exc_set_x0((uint64_t)sys_mount((const char *)trapframe -> x1, (const char *)trapframe -> x2, (const char *)trapframe -> x3), trapframe);
    break;
  case SYS_UMOUNT:
    exc_set_x0((uint64_t)sys_umount((const char *)trapframe -> x1), trapframe);
    break;
  default:
    break;
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

