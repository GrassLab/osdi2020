#include "string.h"
#include "uart.h"

void show_exec() {
  unsigned int elr_el2, esr_el2;
  unsigned int ec, iss;
  asm volatile("MRS %[result], elr_el2" : [result] "=r"(elr_el2));
  asm volatile("MRS %[result], esr_el2" : [result] "=r"(esr_el2));
  ec = esr_el2 >> 26;
  iss = esr_el2 & 0xffffff;
  printf("Exception return address 0x%x\r\n", elr_el2);
  printf("Exception class (EC) 0x%x\r\n", ec);
  printf("Instruction specific syndrome (ISS) 0x%x\r\n", iss);
}