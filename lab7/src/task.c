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

#define THREAD_SIZE 4096
#define Task_pt_regs(tsk) ((struct pt_regs*)(tsk->mm.kernel_pages[0] + THREAD_SIZE - sizeof(struct pt_regs)))

Task *init_task = 0;
Task *current_task = 0, *next_task;
Task **tasks = 0, **read_tasks = 0;

void task_init(){
  show_task_msg("task init");
  init_task = (Task*)kmalloc(sizeof(Task));
  *init_task = (Task){
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    {[0 ... TASK_BUFFER_SIZE - 1] = 0}, 0, 0, 0, 0, 0, 0, 0, none, {0,}
  };
  tasks = (Task**)kmalloc(sizeof(Task*) * TASK_SIZE);
  tasks[0] = current_task = init_task;
  for(int i = 1; i < TASK_SIZE; i++) tasks[i] = 0;
  read_tasks = (Task**)kmalloc(sizeof(Task*) * TASK_SIZE);
  for(int i = 0; i < TASK_SIZE; i++) read_tasks[i] = 0;
  show_task_msg("task init done");
}

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
  current_task->preempt_count--;
}
int preempt_reschedable() { return !current_task->preempt_count; }
void preempt_disable() {
  current_task->preempt_count++;
}

Task *find_task(unsigned long pid){
  for(int i = 0; i < TASK_SIZE; i++)
    if(tasks[i] && tasks[i]->pid == pid)
      return tasks[i];
  return 0;
}

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

  p->pid = unique_pid();
  p->flag = 0;
  p->signals = 0;
  p->counter = 1;
  p->priority = priority;
  p->preempt_count = 1; // disable preemtion until schedule_tail
  p->status = idle;

  if(func){

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

void show_task_msg(char *s){
  int w = 24 - strlen(s);
  int l = w / 2, r = w - w / 2;
  printf(NEWLINE "============");
  while(l--) putchar(' ');
  printf("[%d] %s", current_task->pid, s);
  while(r--) putchar(' ');
  printf("============" NEWLINE);
}
