#include "io.h"
#include "sys.h"
#include "mm.h"
#include "sched.h"
#include "task.h"


extern Task *current_task;

void sys_read(){

}

void sys_write(char *buf) {
  printf("%s", buf);
}

void sys_exec(unsigned long func){
  do_exec(func);
}

int sys_fork(unsigned long pc) {
  privilege_task_create(do_exec, pc);
  return 0;
}

void sys_exit(){
  preempt_disable();
  current_task->status = zombie;
  preempt_enable();
  schedule();
}

int syscall(unsigned int code, long x0, long x1, long x2, long x3, long x4,
    long x5) {
  switch (code) {
    case SYSNUM_READ:
      puts("call read");
      break;
    case SYSNUM_WRITE:
      sys_write((char*)x0);
      break;
    case SYSNUM_EXEC:
      puts("call exec");
      sys_exec(x0);
      break;
    case SYSNUM_FORK:
      puts("call fork");
      sys_fork(x0);
      break;
    case SYSNUM_EXIT:
      sys_exit();
      break;
      // case 0:
      // sys_core_timer_enable();
      // __asm__ volatile("mov x0, #0");
      // break;
      // case 1:
      // sys_timestamp();
      // __asm__ volatile("mov x0, #0");
      // break;
      // case 2:
      // get_current_el();
      // puts("interrupt disabled");
      // *DISABLE_IRQS_1 = (SYSTEM_TIMER_IRQ_1 | AUX_IRQ_MSK);
      // __asm__ volatile("msr daifclr, #0x2");
      // __asm__ volatile("mov x0, #0");
      // break;
      // case 4:
      // while (task_ptr && !exec_ptr) {
      // #define record_elr
      // #ifdef record_elr
      // unsigned long elr, nelr;
      // __asm__ volatile("mrs %0, elr_el1" : "=r"(elr));
      // #endif
      //
      // enable_irq();
      // void (*task)(void) = pop_deffered();
      // printf("task addr %x" NEWLINE, task);
      // task();
      // disable_irq();
      //
      // #ifdef record_elr
      // __asm__ volatile("mrs %0, elr_el1" : "=r"(nelr));
      // printf("%x vs %x" NEWLINE, elr, nelr);
      // if (elr != nelr) {
      // puts(NEWLINE NEWLINE "BOTTOM DIFFERENCE!!" NEWLINE NEWLINE);
      // __asm__ volatile("msr elr_el1, %0" ::"r"(elr));
      // }
      // #endif
      // }
      // __asm__ volatile("mov x0, #0");
      // break;
    default:
      return 1;
  }
  return 0;
}

void *const sys_call_table[] = {sys_read, sys_write, sys_exec, sys_fork, sys_exit};
