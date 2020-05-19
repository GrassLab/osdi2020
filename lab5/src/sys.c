#include "io.h"
#include "irq.h"
#include "mm.h"
#include "sched.h"
#include "task.h"
#include "util.h"
#include "string.h"
#include "syscall.h"


extern Task *current_task, *tasks[TASK_SIZE];

char sys_read(){
  uart_read_enqueue(current_task);
  unsigned long elr, sp_el0, spsr_el1;
  __asm__ volatile("mrs %0, elr_el1" : "=r"(elr));
  __asm__ volatile("mrs %0, sp_el0" : "=r"(sp_el0));
  __asm__ volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
  enable_irq();
  if(preempt_reschedable()) schedule();
  disable_irq();
  __asm__ volatile("msr elr_el1, %0" ::"r"(elr));
  __asm__ volatile("msr sp_el0, %0" ::"r"(sp_el0));
  __asm__ volatile("msr spsr_el1, %0" ::"r"(spsr_el1));

  return current_task->buffer[0];
}

void sys_write(char *buf) {
  uart_puts(buf);
}

void sys_exec(unsigned long func){
  do_exec(func);
}

int sys_fork() {
  Task *ptr = privilege_task_create(0, 0, current_task->priority);
  return ptr ? ptr->pid : 0;
}

void sys_exit(){
  preempt_disable();
  current_task->status = zombie;
  free_task_pages(current_task);
  printf("============      [%d] exit->zombie      ============" NEWLINE, current_task->pid);
  preempt_enable();
}

void sys_signal(unsigned long pid, int code){
  for(int i = 0; i < TASK_SIZE; i++){
    if(tasks[i] && tasks[i]->status != none && tasks[i]->pid == pid){
      tasks[i]->signals |= code;
    }
  }
}

void sys_mutex_lock(Mutex *mtx){
  preempt_disable();
  if(mtx->lock){
    printf(NEWLINE "============      [%d] lcmtx failed      ============"  NEWLINE, current_task->pid);
    current_task->status = sleep;
    current_task->mtx = mtx;

    unsigned long elr, sp_el0, spsr_el1;
    __asm__ volatile("mrs %0, elr_el1" : "=r"(elr));
    __asm__ volatile("mrs %0, sp_el0" : "=r"(sp_el0));
    __asm__ volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
    preempt_enable();
    schedule();
    preempt_disable();
    __asm__ volatile("msr elr_el1, %0" ::"r"(elr));
    __asm__ volatile("msr sp_el0, %0" ::"r"(sp_el0));
    __asm__ volatile("msr spsr_el1, %0" ::"r"(spsr_el1));
  }
  mtx->lock = 1;
  mtx->pid = current_task->pid;
  preempt_enable();
}

void sys_mutex_unlock(Mutex *mtx){
  preempt_disable();
  printf(NEWLINE "============      [%d] releases mtx      ============"  NEWLINE, current_task->pid);
  if(mtx->pid == current_task->pid){
    mtx->lock = 0;
    for(int i = 0; i < TASK_SIZE; i++){
      if(tasks[i]->status == sleep && tasks[i]->mtx == mtx){
        tasks[i]->status = idle;
        tasks[i]->counter += 100;
        printf(NEWLINE "============      [%d] wakes [%d] up      ============"  NEWLINE, current_task->pid);
        break;
      }
    }

  }
  else puts("not the mutex owner");
  preempt_enable();
}


unsigned long sys_task_id(void) {
  return current_task->pid;
}

int syscall(unsigned int code, long x0, long x1, long x2, long x3, long x4,
    long x5) {

  switch (code) {
    case SYSNUM_READ:
      return sys_read();
    case SYSNUM_WRITE:
      sys_write((char*)x0);
      break;
    case SYSNUM_EXEC:
      puts("call exec");
      sys_exec(x0);
      break;
    case SYSNUM_FORK:
      puts("call fork");
      return sys_fork();
    case SYSNUM_EXIT:
      sys_exit();
      break;
    case SYSNUM_SIGNAL:
      sys_signal(x0, x1);
      break;
    case SYSNUM_MUTEX_LOCK:
      sys_mutex_lock((Mutex*)x0);
      break;
    case SYSNUM_MUTEX_UNLOCK:
      sys_mutex_unlock((Mutex*)x0);
      break;
    case SYSNUM_TASK_ID:
      return sys_task_id();
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

void *const sys_call_table[] = {sys_read, sys_write, sys_exec, sys_fork, sys_exit, sys_signal, sys_mutex_lock, sys_mutex_unlock};
