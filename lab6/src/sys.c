#include "io.h"
#include "irq.h"
#include "mm.h"
#include "sched.h"
#include "task.h"
#include "util.h"
#include "string.h"
#include "syscall.h"

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

extern unsigned long _binary____usrbuild_user_img_start;
extern unsigned long _binary____usrbuild_user_img_end;

void sys_exec(unsigned long func){
  free_task_pages(current_task);
  unsigned long begin = (unsigned long)&_binary____usrbuild_user_img_start;
  unsigned long end = (unsigned long)&_binary____usrbuild_user_img_end;
  move_to_user_mode(begin, end - begin, func);
}

int sys_fork() {
  return privilege_task_create(0, 0, current_task->priority)->pid;
}

void sys_exit(){
  preempt_disable();
  current_task->status = zombie;
  free_task_pages(current_task);
  printf(NEWLINE "============      [%d] exit->zombie      ============" NEWLINE, current_task->pid);
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

unsigned long sys_page_num(void) {
  return current_task->mm.user_pages_count;
}

void *sys_mmap(void* addr, unsigned long len,
    int prot, int flags, int file_start, int file_offset){
  return mmap(addr, len, prot, flags, file_start, file_offset);
}

void sys_pages(unsigned long pid) {
  Task *task = find_task(pid);
  printf("[%d]" NEWLINE, task->pid);
  printf(" ├── pgd: 0x%x" NEWLINE, task->mm.pgd);
  printf(" ├── kernel pages:" NEWLINE);
  for(int i = 0; i < task->mm.kernel_pages_count; i++){
    printf(" │   %s── [%d] 0x%x" NEWLINE,
        i == task->mm.kernel_pages_count - 1 ? "└" : "├",
        i,
        task->mm.kernel_pages[i]);
  }
  printf(" └── user pages ( vir -> phy {attr} ):" NEWLINE);
  for(int i = 0; i < task->mm.user_pages_count; i++){
    printf("     %s── [%d] 0x%x -> 0x%x {%x}" NEWLINE,
        i == task->mm.user_pages_count - 1 ? "└" : "├",
        i,
        task->mm.user_pages[i].virt_addr,
        task->mm.user_pages[i].phys_addr,
        task->mm.user_pages[i].attr
      );
  }
}

long int syscall(unsigned int code, long x0, long x1, long x2, long x3, long x4,
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
    case SYSNUM_PAGE_NUM:
      return sys_page_num();
      break;
    case SYSNUM_MMAP:
      return (long int)sys_mmap((void*)x0, (unsigned long)x1,
          (int)x2, (int)x3, (int)x4, (int)x5);
      break;
    case SYSNUM_PAGES:
      sys_pages(x0);
      break;
    default:
      printf("syscall failed with code number: %d" NEWLINE, code);
      return -1;
  }
  return 0;
}

void *const sys_call_table[] = {sys_read, sys_write, sys_exec, sys_fork, sys_exit, sys_signal, sys_mutex_lock, sys_mutex_unlock};
