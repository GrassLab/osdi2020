#include "syscall.h"

int main(void)
{
  syscall_uart_puts("Hi I'm test in user mode\n");
}
