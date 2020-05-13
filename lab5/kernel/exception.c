#include "miniuart.h"
#include "timer.h"
#include "stdint.h"
#include "libc.h"
#include "bh.h"

const char *entry_error_messages[] = {
    "SYNC_INVALID_EL1t",
    "IRQ_INVALID_EL1t",
    "FIQ_INVALID_EL1t",
    "ERROR_INVALID_EL1T",

    "SYNC_INVALID_EL1h",
    "IRQ_INVALID_EL1h",
    "FIQ_INVALID_EL1h",
    "ERROR_INVALID_EL1h",

    "SYNC_INVALID_EL0_64",
    "IRQ_INVALID_EL0_64",
    "FIQ_INVALID_EL0_64",
    "ERROR_INVALID_EL0_64",

    "SYNC_INVALID_EL0_32",
    "IRQ_INVALID_EL0_32",
    "FIQ_INVALID_EL0_32",
    "ERROR_INVALID_EL0_32"
};



/**
 * show the invalid message with some informations
 */
void show_invalid_entry_message(int type,
                                unsigned long esr,
                                unsigned long elr) {
  if (type != -1)
    uart_println("Type: %s", entry_error_messages[type]);
  uart_println("Exception return address 0x%x\r\n"
               "Exception class (EC) 0x%x\r\n"
               "Instruction specific syndrome (ISS) 0x%x",
               elr , esr>>26, esr & 0xfff);
  return;
}

/**
 * system call handler
 *   syscall number is passed in x8
 *   reference: https://thog.github.io/syscalls-table-aarch64/latest.html
 */
void syscall(unsigned int code, int64_t x0, int64_t x1, int64_t x2, int64_t x3,
             int64_t x4, int64_t x5) {

  switch (code) {
  case 0:
    sys_core_timer_enable();
    asm volatile("mov x0, #0");
    break;
  case 1:
    sys_print_time();
    uart_puts("\r\n");
    asm volatile("mov x0, #0");
    break;
  case 2:
    /* this will enable bottom half mode */
    bottom_half_enable();
    /* sys_core_timer_enable(); */
    asm volatile("mov x0, #0");
    break;
  default:
    asm volatile("mov x0, #1");
    break;
  }
  return;
}


/**
 * common exception handler
 */
void exception_handler(int x0, int x1, int x2, int x3, int x4, int x5) {
  unsigned long elr, esr, code, ret = 1;

  asm volatile("mov %0, x8\n"
               "mrs %1, elr_el1\n"
               "mrs %2, esr_el1" :
               "=r"(code), "=r"(elr), "=r"(esr));

  unsigned int ec  = esr >> 26;
  unsigned int iss = esr & 0xfff;

  switch (ec) {
  case 0x15:
    /* svc #0 */
    /* corresponding to system call */
    if (iss == 0) {
      uart_println("===================\r\n"
                   "syscall code: %d" , code);

      syscall(code, x0, x1, x2, x3, x4, x5);
      asm volatile("mov %0, x0" : "=r"(ret));
      /* the return value will stored in x0 register */

      uart_println("syscall return value %d", ret);
      uart_println("===================");
      if (ret == 0) return;
      uart_println("syscall failed with code number: %d", code);
    }
    break;
  default:
    break;
  }

  show_invalid_entry_message(-1, esr, elr);

  return;
}
