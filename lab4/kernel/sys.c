#include "fork.h"
#include "libc.h"
#include "mm.h"
#include "sched.h"

int sys_getid() {
  return current->pid;
}

int sys_fork() {
  return do_fork();
}

void sys_write(char *buf) {
  uart_print("%s", buf);
}

int sys_clone(unsigned long stack) { return copy_process(0, 0, 0, stack); }

unsigned long sys_malloc() {
  unsigned long addr = get_user_free_page();
  if (!addr) {
    return -1;
  }
  return addr;
}

void sys_exit() { uart_println("task %d exit the process", sys_getid()); exit_process(); }

void *const sys_call_table[] = {sys_write, sys_malloc, sys_clone, sys_exit, sys_fork, sys_getid};
