#include "syscall.h"
#include "meta_macro.h"

int main(void)
{
  char ann[0x40] = ANSI_CYAN"[User test command 3] Page used: "ANSI_RESET;
  for(int print_iter = 0; print_iter < 6; ++print_iter)
  {
    /* qemu */
    //for(int i = 0; i < 100000000; ++i);
    /* pi */
    for(int i = 0; i < 1000000; ++i);
    syscall_uart_puts(ann);
    syscall_print_mmu_page_used();
    syscall_uart_puts("\n");
  }
  syscall_uart_puts(ann);
  syscall_uart_puts("Bye!\n");
  return 0;
}
