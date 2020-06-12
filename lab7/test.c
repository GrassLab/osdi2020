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

  syscall_mkdir("mnt");
  int fd = syscall_open("/mnt/a.txt", O_CREAT);
  syscall_write(fd, "Hi", 2);
  syscall_close(fd);

  /* ls */
  int root = syscall_open("/", 0);
  syscall_list(root);
  syscall_close(root);

  syscall_chdir("mnt");
  fd = syscall_open("a.txt", 0);
  syscall_read(fd, buf, 2);
  buf[3] = '\0';
  syscall_uart_puts(buf);
  syscall_uart_puts("\n");

  if(syscall_fork() == 0)
  {
    syscall_uart_puts("Hi I'm child\n");
    a = syscall_open("/world", 0);
    b = syscall_open("/hello", 0);
    int sz;
    sz = syscall_read(a, buf, 100);
    sz += syscall_read(b, buf + sz, 100);
    buf[sz] = '\0';
    syscall_uart_puts(buf);
    syscall_uart_puts("\n");
  }
  else
  {
    syscall_uart_puts("Hi I'm parent\n");
    b = syscall_open("/hello", 0);
    a = syscall_open("/world", 0);
    int sz;
    sz = syscall_read(b, buf, 100);
    sz += syscall_read(a, buf + sz, 100);
    buf[sz] = '\0';
    syscall_uart_puts(buf);
    syscall_uart_puts("\n");
  }

}

