#include "syscall.h"
#include "meta_macro.h"
#include "string_util.h"

uint64_t get_current_task_id(void)
{
  uint64_t current_task_id;
  asm volatile("mrs %0, tpidr_el0\n":
               "=r"(current_task_id));
  return current_task_id;
}

int main(void)
{
  char ann[] = ANSI_BLUE"[User test command 1] "ANSI_RESET;
  char string_buff[0x20];
  unsigned cnt = 0;
  if(syscall_fork() == 0)
  {
    syscall_fork();
    syscall_fork();
    while(cnt < 3)
    {
      uint64_t current_task_id = get_current_task_id();
      syscall_uart_puts(ann);
      syscall_uart_puts(" task_id: ");
      string_longlong_to_char(string_buff, (int)current_task_id);
      syscall_uart_puts(string_buff);
      syscall_uart_puts(" cnt_addr: ");
      string_ulonglong_to_hex_char(string_buff, (unsigned long long)&cnt);
      syscall_uart_puts(string_buff);
      syscall_uart_puts(" cnt_val: ");
      string_longlong_to_char(string_buff, cnt);
      syscall_uart_puts(string_buff);
      syscall_uart_puts("\n");
      ++cnt;
      /* qemu */
      //for(int i = 0; i < 1000000; ++i);
      /* pi */
      for(int i = 0; i < 100000; ++i);
    }
    return 0; // all childs exit
  }
}
