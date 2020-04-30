#include "fork.h"
#include "libc.h"
#include "mm.h"
#include "sched.h"

void sys_write(char *buf) {
  uart_print("%s", buf);
}

int sys_clone(unsigned long stack) { return copy_process(0, 0, 0, stack); }

unsigned long sys_malloc() {
  unsigned long addr = get_free_page();
  if (!addr) {
    return -1;
  }
  return addr;
}

void sys_exit() { exit_process(); }

void *const sys_call_table[] = {sys_write, sys_malloc, sys_clone, sys_exit};
