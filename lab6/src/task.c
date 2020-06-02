#include "task.h"
#include "util.h"
#include "sched.h"
#include "shell.h"
#include "io.h"
#include "info.h"
#include "string.h"
#include "mm.h"
#include "irq.h"
#include "allocator.h"
#include "syscall.h"

#define Task_pt_regs(tsk) ((struct pt_regs*)(tsk->mm.kernel_pages[0] + THREAD_SIZE - sizeof(struct pt_regs)))

Task init_task = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  {[0 ... TASK_BUFFER_SIZE - 1] = 0}, 0, 0, 0, 0, 0, 0, 0, none, {0,}
};

Task *current_task = &init_task, *next_task;
Task *tasks[TASK_SIZE] = { &init_task, };

void append_task(Task *task){
  static int ptr = 1;
  tasks[ptr] = task;
  ptr = (ptr + 1) % TASK_SIZE;
}

int unique_pid(){
  static int pid = 1;
  return pid++;
}

void preempt_enable() {
  //printf("[%d] enable preempt" NEWLINE, current_task->pid);
  current_task->preempt_count--;
}
int preempt_reschedable() { return !current_task->preempt_count; }
void preempt_disable() {
  current_task->preempt_count++;
  //printf("[%d] disable preempt" NEWLINE, current_task->pid);
}

Task *find_task(unsigned long pid){
  for(int i = 0; i < TASK_SIZE; i++)
    if(tasks[i] && tasks[i]->pid == pid)
      return tasks[i];
  return 0;
}

#define THREAD_SIZE 4096

void *task_pt_regs(Task *tsk) {
  unsigned long p = ((unsigned long)tsk) + THREAD_SIZE - sizeof(struct pt_regs);
  return (void*)p;
}

Task *privilege_task_create(void (*func)(), unsigned long arg, unsigned long priority) {

  preempt_disable();

  unsigned long kp = allocate_kernel_page();
  Task *p = (Task*)kp;

  if(p == 0){
    puts("create new task failed");
    preempt_enable();
    return 0;
  }

  struct pt_regs *childregs = task_pt_regs(p);
  memzero((unsigned long)childregs, sizeof(struct pt_regs));
  memzero((unsigned long)&p->cpu_ctx, sizeof(struct cpu_ctx));
  memzero((unsigned long)&p->mm, sizeof(struct mm_struct));

  //if(!func){ memzero(p, p + sizeof(Task)); }

  p->pid = unique_pid();
  p->flag = 0;
  p->signals = 0;
  p->counter = 1;
  p->priority = priority;
  p->preempt_count = 1; // disable preemtion until schedule_tail
  p->status = idle;

  if(func){

    //unsigned long kstack = allocate_kernel_page();
    //p->mm.kernel_pages[0] = kstack;
    //p->mm.kernel_pages_count = 1;
    //p->cpu_ctx.sp = (unsigned long)kstack_pool[p->pid % TASK_SIZE] + STACK_SIZE;
    //p->cpu_ctx.fp = (unsigned long)kstack_pool[p->pid % TASK_SIZE] + STACK_SIZE;
    p->cpu_ctx.sp = kp + STACK_SIZE - sizeof(struct pt_regs);
    p->cpu_ctx.fp = kp + STACK_SIZE - sizeof(struct pt_regs);

    p->cpu_ctx.x19 = (unsigned long)func; /* hold the funtion pointer */
    p->cpu_ctx.x20 = (unsigned long)arg;
    p->cpu_ctx.lr = (unsigned long)ret_from_fork;

  }
  else{ // null fptr means fork

    unsigned long ksp_off = current_task->cpu_ctx.sp
      - ((unsigned long)current_task);

    p->cpu_ctx.sp = ksp_off + kp;

    unsigned long kfp_off = current_task->cpu_ctx.fp
      - ((unsigned long)current_task);
    p->cpu_ctx.fp = kfp_off  + kp;

    /*
       p->cpu_ctx.x19 = current_task->cpu_ctx.x19;
       p->cpu_ctx.x20 = current_task->cpu_ctx.x20;
       */

    strncpy((void*)(kp + sizeof(Task)),
        (void*)(((unsigned long)current_task) + sizeof(Task)),
        STACK_SIZE - sizeof(Task));

    copy_virt_memory(p);

    struct pt_regs *cur_regs = task_pt_regs(current_task);
    *childregs = *cur_regs;     /* new pt_regs <- old pt_regs */
    childregs->regs[0] = 0;      /* aissgn the return value */

    /* TODO copy user stack */
    //unsigned long usp_off = cur_regs->sp
    //  - (unsigned long)ustack_pool[current_task->pid % TASK_SIZE];

    //childregs->sp = usp_off + (unsigned long)ustack_pool[p->pid % TASK_SIZE];

    p->cpu_ctx.lr = (unsigned long)ret_from_fork;
  }

  printf("new task [%d] sp = 0x%x pc = 0x%x" NEWLINE,
      p->pid, p->cpu_ctx.sp, p->cpu_ctx.x19);

  append_task(p);
  preempt_enable();
  return p;
}

void exit(){
  preempt_disable();
  current_task->status = zombie;
  preempt_enable();
  schedule();
}

int move_to_user_mode(unsigned long start, unsigned long size, unsigned long pc){

  struct pt_regs *regs = task_pt_regs(current_task);
  memzero((unsigned long)regs, sizeof(struct pt_regs));

  regs->pc = pc;
  regs->pstate = PSR_MODE_EL0t;

  if(mmap(0x0, size, PROT_READ, MAP_ANONYMOUS, start, 0x0) == MAP_FAILED){
    puts("cannot allocate text code page");
    exit();
    return 1;
  }

  //regs->spsr_el1 = 0x00000000; // copy to spsr_el1 for enter el0
  //regs->sp = 2 *  PAGE_SIZE;

#define allocate_sp_page 1
#define uni_page_stack 0
#if allocate_sp_page
  regs->sp = USER_MEM_LIMIT; //
  unsigned long stkbtm = USER_MEM_LIMIT - USER_STK_LIMIT;
#if uni_page_stack
  unsigned long
    stack_page = allocate_user_page(current_task, regs->sp - PAGE_SIZE);
  if(!stack_page){
    printf("[%d] allocate stack page failed" NEWLINE, current_task->pid);
  }
#else
  while(stkbtm < USER_MEM_LIMIT){
    unsigned long
      stack_page = allocate_user_page(current_task, stkbtm);
    if(!stack_page){
      printf("[%d] allocate stack page failed" NEWLINE, current_task->pid);
      exit();
      return 1;
    }
    stkbtm += PAGE_SIZE;
  }
#endif
#else
  regs->sp = USER_MEM_LIMIT - 1;
#endif
  set_pgd(current_task->mm.pgd);
  return 0;
}

void do_exec(unsigned long pc){
  puts("not support now, use move_to_user_mode instead.");
  exit();
}
