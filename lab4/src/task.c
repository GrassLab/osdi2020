#include "task.h"
#include "util.h"
#include "sched.h"
#include "shell.h"
#include "io.h"
#include "info.h"
#include "sys.h"
#include "mm.h"

//#define current get_current();
//extern Task *get_current();

char kstack_pool[TASK_SIZE][STACK_SIZE];
char ustack_pool[TASK_SIZE][STACK_SIZE];

//static Task init_task = INIT_TASK;
Task task_pool[TASK_SIZE] = {
  [0 ... TASK_SIZE - 1] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {{[0 ... 30] = 0}, 0, 0, 0}, 0, 0, 0, 0, 0, none
  }
};

Task init_task = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  {{[0 ... 30] = 0}, 0, 0, 0}, 0, 0, 0, 1, 0, none
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

void preempt_enable() { current_task->preempt_count--; }
void preempt_disable() { current_task->preempt_count++; }

int talloc(){
  for (int i = 0; i < TASK_SIZE; i++)
    if (task_pool[i].status == none || task_pool[i].status == zombie)
      return i;
  return -1;
}

Task *privilege_task_create(void (*func)(), unsigned long arg) {
  preempt_disable();
  int free_idx = talloc();
  if(free_idx < 0)
    puts("create new task failed");
  else{
    Task *p = task_pool + free_idx;
    p->pid = unique_pid();
    p->flag = 0;
    p->counter = p->priority = current_task->priority;
    p->preempt_count = 1; // disable preemtion until schedule_tail
    p->status = pending;

    p->cpu_ctx.sp = (unsigned long)kstack_pool[p->pid % TASK_SIZE] + STACK_SIZE;
    p->cpu_ctx.fp = (unsigned long)kstack_pool[p->pid % TASK_SIZE] + STACK_SIZE;

    p->cpu_ctx.x19 = (unsigned long)func; /* hold the funtion pointer */
    p->cpu_ctx.x20 = (unsigned long)arg;
    p->cpu_ctx.lr = (unsigned long)ret_from_fork;

    printf("new task [%d] sp = 0x%x pc = 0x%x" NEWLINE,
        p->pid, p->cpu_ctx.sp, func);
    append_task(p);
  }
  preempt_enable();
  return free_idx < 0 ? 0 : task_pool + free_idx;
}

int *show_sp(){
  int *sp;
  __asm__ volatile("mov %0, sp":"=r"(sp));
  printf("sp = 0x%x" NEWLINE, sp);
  return sp;
}

#define TASK_(n) void task_ ## n () { \
  puts(#n " begin"); \
  while(1){ \
    get_current_el(); \
    puts(#n " running ..."); delay(500000000); \
    show_sp(); \
    check_resched(); \
  } \
}

TASK_(1); TASK_(2); TASK_(3); TASK_(4);

void user_process() {
  puts("achieve user_process");
  while(1){
    //puts("user process doing");
    call_sys_write("hello" NEWLINE);
    delay(100000000);
    //puts("user process scheduling");
    //schedule();
  }
  //char buf[30] = {0};
  /* tfp_sprintf(buf, "User process started\n\r"); */
  //call_sys_write("User prcoess started\r\n");
  //unsigned long stack = call_sys_malloc();
  //if (stack < 0) {
  //  uart_println("Error while allocating stack for process 1");
  //  return;
  //}
  //int err = call_sys_clone((unsigned long)&user_process1,
  //                         (unsigned long)"12345", stack);
  //if (err < 0) {
  //  uart_println("Error while clonning process 1");
  //  return;
  //}
  //stack = call_sys_malloc();
  //if (stack < 0) {
  //  uart_println("Error while allocating stack for process 1\n\r");
  //  return;
  //}
  //err = call_sys_clone((unsigned long)&user_process1, (unsigned long)"abcd",
  //                     stack);
  //if (err < 0) {
  //  uart_println("Error while clonning process 2\n\r");
  //  return;
  //}
  //call_sys_exit();
}

void kernel_process(){
  puts("kernel process begin...");
  privilege_task_create(task_1, 0);
  privilege_task_create(task_2, 0);
  //privilege_task_create(irq_shell_loop, 0);
  //privilege_task_create(task_3, 0);
  //privilege_task_create(task_4, 0);
  //do_exec((unsigned long)user_process);
  //irq_shell_loop();
  do_exec((unsigned long)user_process);
  //while(1){
  // puts("kernel process scheduling");
  // schedule();
  //}
}

void do_exec(unsigned long pc){
  //printf("the ");
  //show_addr(current_task->cpu_ctx.sp);
  struct pt_regs *regs = (struct pt_regs *)(current_task->cpu_ctx.sp);
  memzero((unsigned long)regs, (unsigned long)regs + sizeof(struct pt_regs));
  regs->pc = pc;
  regs->pstate = PSR_MODE_EL0t;
  regs->sp = (unsigned long)ustack_pool[current_task->pid % TASK_SIZE];
  //current_task->regs.pc = pc;
  //current_task->regs.pstate = PSR_MODE_EL0t;
  //current_task->regs.sp = (unsigned long)ustack_pool[current_task->pid % TASK_SIZE];
}

void fork(unsigned long pc){

}
