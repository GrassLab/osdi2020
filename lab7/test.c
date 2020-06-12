#include "syscall.h"
#include "string_util.h"
#include "vfs.h"

int main(void)
{
  char buf[0x20];
  syscall_uart_puts("Hi I'm test in user mode\n");

  int a = syscall_open("/hello", O_CREAT);

  int b = syscall_open("/world", O_CREAT);
  syscall_write(a, "Hello ", 6);
  syscall_write(b, "World!", 6);
  syscall_close(a);
  syscall_close(b);
  b = syscall_open("/hello", 0);
  a = syscall_open("/world", 0);
  int sz;
  sz = syscall_read(b, buf, 100);
  sz += syscall_read(a, buf + sz, 100);
  buf[sz] = '\0';
  syscall_uart_puts(buf);
  syscall_uart_puts("\n");

  /* ls */
  int root = syscall_open("/", 0);
  syscall_list(root);
  syscall_close(root);
}

