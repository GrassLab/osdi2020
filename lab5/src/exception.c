#include "info.h"
#include "io.h"
#include "irq.h"
#include "sys.h"
#include "map.h"
#include "time.h"
#include "timer.h"
#include "sched.h"

const char *entry_error_messages[] = {
  "SYNC_INVALID_EL1t",   "IRQ_INVALID_EL1t",
  "FIQ_INVALID_EL1t",    "ERROR_INVALID_EL1T",

  "SYNC_INVALID_EL1h",   "IRQ_INVALID_EL1h",
  "FIQ_INVALID_EL1h",    "ERROR_INVALID_EL1h",

  "SYNC_INVALID_EL0_64", "IRQ_INVALID_EL0_64",
  "FIQ_INVALID_EL0_64",  "ERROR_INVALID_EL0_64",

  "SYNC_INVALID_EL0_32", "IRQ_INVALID_EL0_32",
  "FIQ_INVALID_EL0_32",  "ERROR_INVALID_EL0_32"};

void show_invalid_entry_message(int type, unsigned long esr,
    unsigned long elr) {
  if (type != -1)
    printf("Type: %s" NEWLINE, entry_error_messages[type]);
  printf("Exception return address 0x%x" NEWLINE
      "Exception class (EC) 0x%x" NEWLINE
      "Instruction specific syndrome (ISS) 0x%x" NEWLINE,
      elr, esr >> 26, esr & 0xfff);
  return;
}

int exception_handler(long x0, long x1, long x2, long x3, long x4, long x5) {
  unsigned long elr, esr, code;

  __asm__ volatile("mov %0, x8\n"
      "mrs %1, elr_el1\n"
      "mrs %2, esr_el1"
      : "=r"(code), "=r"(elr), "=r"(esr));

  unsigned int ec = esr >> 26;
  unsigned int iss = esr & 0xfff;

  switch(ec){
    case 0x15:
      if (iss == 0) {
        int ret = syscall(code, x0, x1, x2, x3, x4, x5);
        if (ret < 0) printf("syscall failed with code number: %d" NEWLINE, code);
        return ret;
      }
      break;
    default:
      break;
  }
  show_invalid_entry_message(-1, esr, elr);
  return -1;
}
