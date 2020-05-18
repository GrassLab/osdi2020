#include "fork.h"
#include "libc.h"
#include "miniuart.h"
#include "mm.h"
#include "sched.h"

char sys_read() { return getc(); }

int sys_exec(void(*func)) { return do_exec(func); }

/* timestamp: display the "system" timer */
void sys_timestamp() {
  unsigned long long freq, cnt;

  /* get the current counter frequency / counter */
  asm volatile("mrs %0, cntfrq_el0\n"
               "mrs %1, cntpct_el0\n"
               : "=r"(freq), "=r"(cnt));
  unsigned long long i_part = cnt / freq;
  unsigned long long f_part = cnt * 100000000 / freq % 100000000;

  uart_print("[%d.%d] ", i_part, f_part);
}

int sys_getid() { return current->pid; }

int sys_fork() { return do_fork(); }


void sys_write(char *buf) {
  {
    uart_println(buf);
    /* current->print_buffer = (unsigned long)strcat((char *)current->print_buffer, buf); */
  }
}

int sys_clone(unsigned long stack) { return copy_process(0, 0, 0); }

unsigned long sys_malloc() {
  /* unsigned long addr = allocate_user_page(current, ); */
  /* if (!addr) { */
  /*   return -1; */
  /* } */
  /* return addr; */
  return 0;
}


void sys_exit() {
#ifdef DEBUG
  uart_println("task %d exit the process", sys_getid());
#endif
  exit_process();
}

void *const sys_call_table[] = {sys_write,     sys_malloc, sys_clone,
                                sys_exit,      sys_fork,   sys_getid,
                                sys_timestamp, sys_exec,   sys_read};
