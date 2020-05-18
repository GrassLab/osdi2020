#include "task.h"
#include "util.h"
#include "sched.h"
#include "shell.h"
#include "io.h"
#include "info.h"
#include "string.h"
#include "sys.h"
#include "mm.h"

//#define task_pt_regs(tsk) (void*)(kstack_pool[tsk->pid % TASK_SIZE] + STACK_SIZE)
//#define current get_current();
//extern Task *get_current();

//char kstack_pool[TASK_SIZE][STACK_SIZE];
//char ustack_pool[TASK_SIZE][STACK_SIZE];

//static Task init_task = INIT_TASK;
//Task task_pool[TASK_SIZE] = {
//  [0 ... TASK_SIZE - 1] = {
//    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//    {[0 ... TASK_BUFFER_SIZE - 1] = 0}, 0, 0, 0, 0, 0, 0, 0, none, {0,}
//  }
//};

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
  printf("dispatch pid %d" NEWLINE, pid);
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

//int talloc(){
//  for (int i = 0; i < TASK_SIZE; i++)
//    if (task_pool[i].status == none) // || task_pool[i].status == zombie
//      return i;
//  return -1;
//}

#define THREAD_SIZE 4096
struct pt_regs *task_pt_regs(Task *tsk) {
  unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
  return (struct pt_regs *)p;
}

Task *privilege_task_create(void (*func)(), unsigned long arg, unsigned long priority) {

  preempt_disable();

  Task *p = (Task*) allocate_kernel_page();

  //int free_idx = talloc();
  //if(free_idx < 0){
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

    unsigned long kstack = allocate_kernel_page();
    p->mm.kernel_pages[0] = kstack;
    p->mm.kernel_pages_count = 1;
    //p->cpu_ctx.sp = (unsigned long)kstack_pool[p->pid % TASK_SIZE] + STACK_SIZE;
    //p->cpu_ctx.fp = (unsigned long)kstack_pool[p->pid % TASK_SIZE] + STACK_SIZE;
    p->cpu_ctx.sp = kstack + STACK_SIZE;
    p->cpu_ctx.fp = kstack + STACK_SIZE;

    p->cpu_ctx.x19 = (unsigned long)func; /* hold the funtion pointer */
    p->cpu_ctx.x20 = (unsigned long)arg;
    p->cpu_ctx.lr = (unsigned long)ret_from_fork;

  }
  else{ // null fptr means fork
    unsigned long kstack = allocate_kernel_page();
    p->mm.kernel_pages[0] = kstack;
    p->mm.kernel_pages_count = 1;

    /* TODO copy user stack */
    //unsigned long ustack = allocate_kernel_page();
    //p->mm.user_pages[0] = ustack;
    //p->mm.user_pages_count = 1;

    unsigned long ksp_off = current_task->cpu_ctx.sp
      - current_task->mm.kernel_pages[0];
      //- (unsigned long)kstack_pool[current_task->pid % TASK_SIZE];

    p->cpu_ctx.sp = ksp_off + kstack;

    unsigned long kfp_off = current_task->cpu_ctx.fp
      - current_task->mm.kernel_pages[0];
      //- (unsigned long)kstack_pool[current_task->pid % TASK_SIZE];
    p->cpu_ctx.fp = kfp_off  + kstack;

    /*
       p->cpu_ctx.x19 = current_task->cpu_ctx.x19;
       p->cpu_ctx.x20 = current_task->cpu_ctx.x20;
       */
    strcpy((void*)kstack,
        (void*)current_task->mm.kernel_pages[0],
        STACK_SIZE);

    /* TODO copy user stack */
    //strcpy(ustack_pool[p->pid % TASK_SIZE],
    //    ustack_pool[current_task->pid % TASK_SIZE],
    //    STACK_SIZE);

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
  return p->pid == current_task->pid ? 0 : p;
}


int *show_sp(){
  int *sp;
  __asm__ volatile("mov %0, sp":"=r"(sp));
  printf("sp = 0x%x" NEWLINE, sp);
  return sp;
}

#if 0
char* getline(char *buffer, char c){
  char *p = buffer;
  while(!strchr("\r\n", (*p++ = call_sys_read())))
    if(c) print(c);
  //printf("-------------------- %c == %c<", *(p - 1), current_task->buffer[0]);
  *--p = 0;
  return buffer;
}
#else
#define getline(buffer, c) { \
  char *p = buffer; \
  while(!strchr("\r\n", (*p++ = call_sys_read()))) \
    if(c) printf("%c", c); \
  *--p = 0; \
}

#endif

void task_do_exec(unsigned long pc){
  do_exec(pc);
}

void zombie_reaper(){
  printf(NEWLINE "============      [%d] zombie reaper     ============"  NEWLINE, current_task->pid);
  while(1){
    printf(NEWLINE "============     [%d] zreaper running    ============"  NEWLINE, current_task->pid);
    for(int i = 0; i < TASK_SIZE; i++){
      if(tasks[i] && tasks[i]->status == zombie){
        printf(NEWLINE "============ zombie_reaper kill zombie %d ============" NEWLINE, tasks[i]->pid);
        tasks[i]->status = none;
        tasks[i] = 0;
      }
    }
    delay(5000000);
    schedule();
  }
}

void exit(){
  printf(NEWLINE "============     [%d] becomes zombie     ============" NEWLINE, current_task->pid);
  preempt_disable();
  current_task->status = zombie;
  preempt_enable();
}

#define TASK_(n) void task_ ## n () { \
  printf(NEWLINE "============      [%d] TASK" #n " daemon      ============"  NEWLINE, current_task->pid); \
  while(1){ \
    if(current_task->signals & SIGKILL){ \
      printf(NEWLINE "============      [%d] TASK" #n " sigkill     ============"  NEWLINE, current_task->pid); \
      exit(); \
      if(preempt_reschedable())schedule(); \
    } \
    printf(NEWLINE "============      [%d] TASK" #n " running     ============"  NEWLINE, current_task->pid); \
    delay(5000000); \
    check_resched(); \
  } \
}

TASK_(1); TASK_(2); TASK_(3); TASK_(4);

void kernel_process(){
  puts("kernel process begin...");
  //printf("kstack: %x, ustack %x" NEWLINE, kstack_pool, ustack_pool);
  //privilege_task_create(task_do_exec, (UL)user_login, 3);
  //privilege_task_create(task_do_exec, (UL)user_shell, 2);
  privilege_task_create(task_1, 0, current_task->priority);
  privilege_task_create(task_2, 0, current_task->priority);
  privilege_task_create(task_3, 0, current_task->priority);
  privilege_task_create(task_4, 0, current_task->priority);
  //privilege_task_create(task_do_exec, (UL)user_fork, current_task->priority);
  //privilege_task_create(task_do_exec, (UL)user_exec, current_task->priority);
  //privilege_task_create(task_do_exec, (UL)user_mutex, current_task->priority);
  //privilege_task_create(task_do_exec, (UL)user_mutex, current_task->priority);
  //privilege_task_create(task_do_exec, (UL)user_mutex, current_task->priority);
  //privilege_task_create(task_do_exec, (UL)user_write, current_task->priority);
  //privilege_task_create(task_do_exec, (UL)user_hang, current_task->priority);
  privilege_task_create(zombie_reaper, 0, current_task->priority);
  //privilege_task_create(task_2, 0);
  //privilege_task_create(task_3, 0);
  //privilege_task_create(task_4, 0);
  //irq_shell_loop();
  //puts("kernel stack:");
  //show_sp();
  /* do_exec((unsigned long)user_exit); */
  //exit();
  //do_exec((unsigned long)user_write);
  exit();
  while(1){
   puts("kernel process scheduling");
   schedule();
  }
}

void do_exec(unsigned long pc){
  printf("[%d] do exec" NEWLINE, current_task->pid);
  struct pt_regs *regs = task_pt_regs(current_task);
  memzero((unsigned long)regs, sizeof(struct pt_regs));

  regs->pc = pc;
  regs->pstate = PSR_MODE_EL0t;
	//regs->spsr_el1 = 0x00000000; // copy to spsr_el1 for enter el0
	regs->sp =  0x0000ffffffffe000; // why

	unsigned long
    stack_page = allocate_user_page(current_task, regs->sp - 8),
	  code_page = allocate_user_page(current_task, pc);

  if(!code_page || !stack_page){
    printf("[%d] do exec FAILED" NEWLINE, current_task->pid);
  }

  //memcpy(code_page, start, size);
	set_pgd(current_task->mm.pgd);
  /* TODO adjust user stack */
  //regs->sp = (unsigned long)ustack_pool[current_task->pid % TASK_SIZE] + STACK_SIZE;
}