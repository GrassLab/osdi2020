#include "syscall.h"
#include "string_util.h"
#include "vfs.h"

int main(void)
{
  char buf[0x80];
  syscall_uart_puts("Hi I'm test in user mode\n");

  /* ls content of root */
  syscall_uart_puts("=====\n");
  syscall_uart_puts("ls / (files and directories created by initramfs)\n");
  int root = syscall_open("/", 0);
  syscall_list(root);
  syscall_close(root);
  syscall_uart_puts("=====\n");

  syscall_uart_puts("Write /foo with \"bar2000\"\n");
  syscall_uart_puts("Write /bar with \"Foo\"\n");
  int a = syscall_open("/foo", O_CREAT);
  int b = syscall_open("/bar", O_CREAT);
  syscall_write(a, "bar2000", 7);
  syscall_write(b, "Foo", 3);
  syscall_close(a);
  syscall_close(b);
  syscall_uart_puts("=====\n");

  syscall_uart_puts("syscall_fork()\n");
  syscall_uart_puts("=====\n");
  if(syscall_fork() == 0)
  {
    syscall_uart_puts("Hi I'm child\n");
    syscall_uart_puts("Read /bar and /foo\n");
    a = syscall_open("./bar", 0);
    b = syscall_open("./foo", 0);
    int sz;
    sz = syscall_read(a, buf, 100);
    sz += syscall_read(b, buf + sz, 100);
    buf[sz] = '\0';
    syscall_uart_puts(buf);
    syscall_uart_puts("\n");
    syscall_uart_puts("Child will exit\n");
    syscall_uart_puts("=====\n");
    syscall_exit(1);
  }
  syscall_uart_puts("Hi I'm parent\n");
  syscall_uart_puts("Read hello and world create by initramfs\n");
  b = syscall_open("hello", 0);
  a = syscall_open("world", 0);
  int sz;
  sz = syscall_read(b, buf, 100);
  sz += syscall_read(a, buf + sz, 100);
  buf[sz] = '\0';
  syscall_uart_puts(buf);
  syscall_uart_puts("\n");
  syscall_close(a);
  syscall_close(b);
  syscall_uart_puts("=====\n");

  syscall_uart_puts("Create /mnt/a.txt and write \"Hi\"\n");
  int fd = syscall_open("/mnt/a.txt", O_CREAT);
  syscall_write(fd, "Hi", 2);
  syscall_close(fd);
  syscall_uart_puts("=====\n");

  syscall_uart_puts("Chdir to mnt\n");
  syscall_chdir("mnt");
  syscall_uart_puts("=====\n");

  syscall_uart_puts("Read ./a.txt\n");
  fd = syscall_open("./a.txt", 0);
  syscall_read(fd, buf, 2);
  buf[2] = '\0';
  syscall_uart_puts(buf);
  syscall_uart_puts("\n");
  syscall_close(fd);
  syscall_uart_puts("=====\n");

  syscall_uart_puts("Chdir ..\n");
  syscall_chdir("..");
  syscall_uart_puts("=====\n");

  syscall_uart_puts("Mount tmpfs to mnt\n");
  syscall_mount("tmpfs", "mnt", "tmpfs");
  syscall_uart_puts("=====\n");

  fd = syscall_open("mnt/a.txt", 0);
  if(fd >= 0)
  {
    syscall_uart_puts("Assertion syscall_open(\"mnt/a.txt\", 0) >= 0\n");
    while(1);
  }
  else
  {
    syscall_uart_puts("syscall_open(\"mnt/a.txt\", 0) < 0\n");
    syscall_uart_puts("=====\n");
  }

  syscall_uart_puts("umount /mnt\n");
  syscall_umount("/mnt");
  syscall_uart_puts("=====\n");

  syscall_uart_puts("open mnt/a.txt\n");
  fd = syscall_open("mnt/a.txt", 0);
  if(fd < 0)
  {
    syscall_uart_puts("Assertion syscall_open(\"mnt/a.txt\", 0) < 0");
    while(1);
  }
  else
  {
    syscall_uart_puts("syscall_open(\"mnt/a.txt\", 0) >= 0\n");
    syscall_uart_puts("=====\n");
  }
  syscall_close(fd);

  /* procfs */
  syscall_uart_puts("mount procfs to proc\n");
  syscall_mount("procfs", "proc", "procfs");
  syscall_uart_puts("=====\n");

  syscall_uart_puts("Write 0 to /proc/switch\n");
  fd = syscall_open("/proc/switch", 0);
  syscall_write(fd, "0", 1);
  syscall_close(fd);
  syscall_uart_puts("=====\n");

  syscall_uart_puts("Read /proc/hello\n");
  fd = syscall_open("/proc/hello", 0);
  sz = syscall_read(fd, buf, 16);
  buf[sz++] = '\n';
  buf[sz] = '\0';
  syscall_uart_puts(buf);
  syscall_close(fd);
  syscall_uart_puts("=====\n");

  syscall_uart_puts("Write 1 to /proc/switch\n");
  fd = syscall_open("/proc/switch", 0);
  syscall_write(fd, "1", 1);
  syscall_close(fd);
  syscall_uart_puts("=====\n");

  syscall_uart_puts("Read /proc/hello\n");
  fd = syscall_open("/proc/hello", 0);
  sz = syscall_read(fd, buf, 16);
  buf[sz++] = '\n';
  buf[sz] = '\0';
  syscall_uart_puts(buf);
  syscall_close(fd);
  syscall_uart_puts("=====\n");

  syscall_uart_puts("Read /proc/3/status\n");
  fd = syscall_open("/proc/3/status", 0);
  sz = syscall_read(fd, buf, 16);
  buf[sz++] = '\n';
  buf[sz] = '\0';
  syscall_uart_puts(buf);
  syscall_close(fd);
  syscall_uart_puts("=====\n");

  syscall_uart_puts("Open /proc/999/status\n");
  fd = syscall_open("/proc/999/status", 0);
  if(fd >= 0)
  {
    syscall_uart_puts("Assertion syscall_open(\"/proc/999/status\", 0) >= 0");
    while(1);
  }
  else
  {
    syscall_uart_puts("syscall_open(\"/proc/999/status\", 0) < 0\n");
    syscall_uart_puts("=====\n");
  }
}

