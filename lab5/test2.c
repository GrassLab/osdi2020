#include "syscall.h"
#include "meta_macro.h"

int main(void)
{
  char ann[] = ANSI_YELLOW"[User test command 2] "ANSI_RESET;
  if(syscall_fork() == 0)
  {
    syscall_uart_puts(ann);
    syscall_uart_puts("I'm child, prepare to page fault\n");
    unsigned * a = (unsigned *)0xffffdeadbeefcdcd; /* a non-mapped address. */
    *a = 0xdeadbeef; /* trigger simple page fault, child will die here. */
    syscall_uart_puts(ann);
    syscall_uart_puts("Impossible\n");
  }
  return 0;
}

