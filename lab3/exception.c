#include "uart.h"

void show_exec() {
  uart_puts("exec");
  unsigned int elr_el1, esr_el1;
  unsigned int ec, iss;
  asm volatile("MRS %[result], elr_el1" : [result] "=r"(elr_el1));
  asm volatile("MRS %[result], esr_el1" : [result] "=r"(esr_el1));
  ec = esr_el1 >> 26;
  iss = esr_el1 & 0xffffff;
  printf("Exception return address 0x%x\r\n", elr_el1);
  printf("Exception class (EC) 0x%x\r\n", ec);
  printf("Instruction specific syndrome (ISS) 0x%x\r\n", iss);
}