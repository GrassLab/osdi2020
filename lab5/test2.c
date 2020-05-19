#include "syscall.h"
#include "meta_macro.h"

int main(void)
{
  char ann[] = ANSI_YELLOW"[User test command 2] "ANSI_RESET;
  while(1)
  {
    /* qemu */
    for(int i = 0; i < 800000; ++i);
    syscall_uart_puts(ann);
    syscall_uart_puts("Hi\n");
  }
  return 0;
}
