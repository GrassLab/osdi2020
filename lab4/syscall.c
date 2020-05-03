#include "syscall.h"

int syscall_uart_puts(char * string)
{
  asm volatile(
    "mov x1, %0\n"
    "svc #3\n"
    : : "r"(string));
  return 0;
}

int syscall_uart_gets(char * string, char delimiter, unsigned length)
{
  asm volatile("mov x1, %0" : : "r"(string));
  asm volatile("mov x2, %0" : : "r"(delimiter));
  asm volatile("mov x3, %0" : : "r"(length));
  asm volatile("svc #4");
  return 0;
}

int syscall_exec(void(*start_func)())
{
  asm volatile(
    "mov x1, %0\n"
    "svc #5\n"
    : : "r"(start_func));
  return 0;
}

