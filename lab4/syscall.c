#include "syscall.h"

int syscall_uart_puts(char * string)
{
  asm volatile(
    "mov x1, %0\n"
    "svc #3\n"
    : : "r"(string));
  return 0;
}

