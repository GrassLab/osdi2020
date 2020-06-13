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

int syscall_fork(void)
{
  int retval;
  asm volatile("svc #6");
  asm volatile("mov %0, x0" : "=r"(retval));
  return retval;
}

int syscall_exit(int status)
{
  asm volatile("mov x1, %0" : : "r"(status));
  asm volatile("svc #7");
  return 0;
}

int syscall_signal(int task_id, int signalno)
{
  asm volatile("mov x1, %0" : : "r"(task_id));
  asm volatile("mov x2, %0" : : "r"(signalno));
  asm volatile("svc #8");
  return 0;
}

int syscall_malloc(unsigned bytes)
{
  asm volatile("mov x1, %0" : : "r"(bytes));
  asm volatile("svc #9");
  return 0;
}

int syscall_free(uint64_t * va)
{
  asm volatile("mov x1, %0" : : "r"(va));
  asm volatile("svc #10");
  return 0;
}

int syscall_open(const char * pathname, int flags)
{
  int retval;
  asm volatile("mov x1, %0" : : "r"(pathname));
  asm volatile("mov x2, %0" : : "r"(flags));
  asm volatile("svc #11");
  asm volatile("mov %0, x0" : "=r"(retval));
  return retval;
}

int syscall_close(int fd)
{
  int retval;
  asm volatile("mov x1, %0" : : "r"(fd));
  asm volatile("svc #12");
  asm volatile("mov %0, x0" : "=r"(retval));
  return retval;
}

int syscall_write(int fd, const void * buf, size_t len)
{
  int retval;
  asm volatile("mov x1, %0" : : "r"(fd));
  asm volatile("mov x2, %0" : : "r"(buf));
  asm volatile("mov x3, %0" : : "r"(len));
  asm volatile("svc #13");
  asm volatile("mov %0, x0" : "=r"(retval));
  return retval;
}

int syscall_read(int fd, void * buf, size_t len)
{
  int retval;
  asm volatile("mov x1, %0" : : "r"(fd));
  asm volatile("mov x2, %0" : : "r"(buf));
  asm volatile("mov x3, %0" : : "r"(len));
  asm volatile("svc #14");
  asm volatile("mov %0, x0" : "=r"(retval));
  return retval;
}

int syscall_list(int fd)
{
  int retval;
  asm volatile("mov x1, %0" : : "r"(fd));
  asm volatile("svc #15");
  asm volatile("mov %0, x0" : "=r"(retval));
  return retval;
}

int syscall_mkdir(const char * pathname)
{
  int retval;
  asm volatile("mov x1, %0" : : "r"(pathname));
  asm volatile("svc #16");
  asm volatile("mov %0, x0" : "=r"(retval));
  return retval;
}

int syscall_chdir(const char * pathname)
{
  int retval;
  asm volatile("mov x1, %0" : : "r"(pathname));
  asm volatile("svc #17");
  asm volatile("mov %0, x0" : "=r"(retval));
  return retval;
}

int syscall_mount(const char * device, const char * mountpoint, const char * filesystem)
{
  int retval;
  asm volatile("mov x1, %0" : : "r"(device));
  asm volatile("mov x2, %0" : : "r"(mountpoint));
  asm volatile("mov x3, %0" : : "r"(filesystem));
  asm volatile("svc #18");
  asm volatile("mov %0, x0" : "=r"(retval));
  return retval;
}

