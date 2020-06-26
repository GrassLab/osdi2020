#include "sys.h"
#include "vfs.h"
#include "uart.h"

void test_init(void)
{
  uart_puts("open /HELLO\n");
  struct vfs_file_struct * hello_file = vfs_open("/HELLO", 0);
  if(hello_file == 0)
  {
    uart_puts("Assert hello_file != 0\n");
    while(1);
  }
  uart_puts("---\n");
  return;
}
