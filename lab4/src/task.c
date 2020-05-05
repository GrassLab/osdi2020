#include "task.h"
#include "util.h"
#include "sched.h"
#include "shell.h"
#include "io.h"
#include "info.h"
#include "string.h"
#include "sys.h"
#include "mm.h"

#define task_pt_regs(tsk) (void*)(kstack_pool[tsk->pid % TASK_SIZE] + STACK_SIZE)
//#define current get_current();
//extern Task *get_current();

char kstack_pool[TASK_SIZE][STACK_SIZE];
char ustack_pool[TASK_SIZE][STACK_SIZE];

//static Task init_task = INIT_TASK;
Task task_pool[TASK_SIZE] = {
  [0 ... TASK_SIZE - 1] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {[0 ... TASK_BUFFER_SIZE - 1] = 0}, 0, 0, 0, 0, 0, 0, none
  }
};

Task init_task = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  {[0 ... TASK_BUFFER_SIZE - 1] = 0}, 0, 0, 0, 0, 0, 0, none
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

int talloc(){
  for (int i = 0; i < TASK_SIZE; i++)
    if (task_pool[i].status == none) // || task_pool[i].status == zombie
      return i;
  return -1;
}

Task *privilege_task_create(void (*func)(), unsigned long arg, unsigned long priority) {


  preempt_disable();
  int free_idx = talloc();
  if(free_idx < 0){
    puts("create new task failed");
    preempt_enable();
    return 0;
  }
  Task *p = task_pool + free_idx;

  //if(!func){ memzero(p, p + sizeof(Task)); }

  p->pid = unique_pid();
  p->flag = 0;
  p->signals = 0;
  p->counter = 1;
  p->priority = priority;
  p->preempt_count = 1; // disable preemtion until schedule_tail
  p->status = idle;


  if(func){

    p->cpu_ctx.sp = (unsigned long)kstack_pool[p->pid % TASK_SIZE] + STACK_SIZE;
    p->cpu_ctx.fp = (unsigned long)kstack_pool[p->pid % TASK_SIZE] + STACK_SIZE;
    p->cpu_ctx.x19 = (unsigned long)func; /* hold the funtion pointer */
    p->cpu_ctx.x20 = (unsigned long)arg;
    p->cpu_ctx.lr = (unsigned long)ret_from_fork;

  }
  else{ // null fptr means fork

    unsigned long ksp_off = current_task->cpu_ctx.sp
      - (unsigned long)kstack_pool[current_task->pid % TASK_SIZE];

    p->cpu_ctx.sp = ksp_off + (unsigned long)kstack_pool[p->pid % TASK_SIZE];

    unsigned long kfp_off = current_task->cpu_ctx.fp
      - (unsigned long)kstack_pool[current_task->pid % TASK_SIZE];
    p->cpu_ctx.fp = kfp_off  + (unsigned long)kstack_pool[p->pid % TASK_SIZE];

    /*
       p->cpu_ctx.x19 = current_task->cpu_ctx.x19;
       p->cpu_ctx.x20 = current_task->cpu_ctx.x20;
       */

    memcpy(kstack_pool[p->pid % TASK_SIZE],
        kstack_pool[current_task->pid % TASK_SIZE],
        STACK_SIZE);
    memcpy(ustack_pool[p->pid % TASK_SIZE],
        ustack_pool[current_task->pid % TASK_SIZE],
        STACK_SIZE);

    struct pt_regs *childregs = task_pt_regs(p);
    struct pt_regs *cur_regs = task_pt_regs(current_task);
    *childregs = *cur_regs;     /* new pt_regs <- old pt_regs */
    childregs->regs[0] = 0;      /* aissgn the return value */

    unsigned long usp_off = cur_regs->sp
      - (unsigned long)ustack_pool[current_task->pid % TASK_SIZE];

    childregs->sp = usp_off + (unsigned long)ustack_pool[p->pid % TASK_SIZE];

    p->cpu_ctx.lr = (unsigned long)ret_from_fork;
  }

  printf("new task [%d] sp = 0x%x pc = 0x%x" NEWLINE,
      p->pid, p->cpu_ctx.sp, p->cpu_ctx.x19);

  append_task(p);
  preempt_enable();
  return p->pid == current_task->pid ? 0 : task_pool + free_idx;
}


int *show_sp(){
  int *sp;
  __asm__ volatile("mov %0, sp":"=r"(sp));
  printf("sp = 0x%x" NEWLINE, sp);
  return sp;
}

char* getline(char *buffer, char c){
  char *p = buffer;
  while((*p++ = call_sys_read()) != '\r')
    if(c) print(c);
  //printf("-------------------- %c == %c<", *(p - 1), current_task->buffer[0]);
  *--p = 0;
  return buffer;
}

void user_login(){
  char buffer[128];
  printf(NEWLINE "============      [%d] login daemon      ============"  NEWLINE, current_task->pid);
  while(1){
    char *input = getline(buffer, '*'), pw[10] = "root";
    int d = strcmp(input, pw);
    if(!d) break;
    printf(NEWLINE "input password:");
  }
  puts(NEWLINE "============     login successfully!     ============");
  call_sys_exit();
}

void user_shell(){
  printf(NEWLINE "============      [%d] shell daemon      ============"  NEWLINE, current_task->pid);
  irq_shell_loop(0);
  call_sys_exit();
}

void user_read() {
  call_sys_write("user_read test" NEWLINE);
  while(1){
    printf("input:");
    printf("read %d" NEWLINE, call_sys_read());
    //delay(100000000);
  }
}

void user_write() {
  printf(NEWLINE "============      [%d] WRITE testbeg     ============"  NEWLINE, current_task->pid);
  while(1){
    call_sys_write(NEWLINE "============         WRITE   test        ============" NEWLINE);
    delay(100000000);
    if(current_task->signals & SIGKILL){
      printf(NEWLINE "============      [%d] WRITE sigkill     ============"  NEWLINE, current_task->pid);
      call_sys_exit();
      while(1);
    }
  }
}

void task_do_exec(unsigned long pc){
  do_exec(pc);
}

void user_idle() {
  while(1){
    call_sys_write("user_process idle..." NEWLINE);
  }
}

void user_hang() {
  printf(NEWLINE "============      [%d]  user  hang       ============"  NEWLINE, current_task->pid);
  while(1){
    delay(100000000);
    printf(NEWLINE "============     [%d] user is hanging    ============"  NEWLINE, current_task->pid);
  }
}

void user_exec() {
  while(1){
    call_sys_write("user_process_exec test" NEWLINE);
    call_sys_exec(user_idle);
  }
}

void user_exit() {
  while(1){
    call_sys_write("user_process_exit test" NEWLINE);
    call_sys_exit();
  }
}

void user_fork() {
  int child = 0, k = 15;
  call_sys_write("user_process_fork test" NEWLINE);
  child = call_sys_fork();

  while(1){
    printf("stack reg = %x - %x" NEWLINE, ustack_pool[current_task->pid % TASK_SIZE],
        ustack_pool[current_task->pid % TASK_SIZE] + STACK_SIZE);
    show_sp();
    printf("[%d] fork return %d" NEWLINE, current_task->pid, child);
    printf("k = [0x%x] = %d" NEWLINE, &k, k);
    call_sys_write(NEWLINE);
    delay(100000000);
    if(!child) k++;
  }
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
    delay(500000000);
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
    delay(500000000); \
    check_resched(); \
  } \
}

TASK_(1); TASK_(2); TASK_(3); TASK_(4);

void kernel_process(){
  puts("kernel process begin...");
  //printf("kstack: %x, ustack %x" NEWLINE, kstack_pool, ustack_pool);
  //privilege_task_create(user_process_login, 0, 3);
  privilege_task_create(task_do_exec, (UL)user_shell, 2);
  privilege_task_create(task_1, 0, current_task->priority);
  //privilege_task_create(task_2, 0, current_task->priority);
  //privilege_task_create(user_process_write, 0, current_task->priority);
  //privilege_task_create(task_do_exec, (UL)user_write, current_task->priority);
  privilege_task_create(task_do_exec, (UL)user_hang, current_task->priority);
  privilege_task_create(zombie_reaper, 0, current_task->priority);
  //privilege_task_create(task_2, 0);
  //privilege_task_create(task_3, 0);
  //privilege_task_create(task_4, 0);
  //irq_shell_loop();
  puts("kernel stack:");
  show_sp();
  do_exec((unsigned long)user_exit);
  //exit();
  //do_exec((unsigned long)user_write);
  //exit();
  //while(1){
  // puts("kernel process scheduling");
  // schedule();
  //}
}

void do_exec(unsigned long pc){
  printf("[%d] do exec" NEWLINE, current_task->pid);
  struct pt_regs *regs = task_pt_regs(current_task);
  //printf("!! %x %x %x %x" NEWLINE,
  //    current_task->cpu_ctx.sp,
  //    kstack_pool[current_task->pid % TASK_SIZE],
  //    kstack_pool[current_task->pid % TASK_SIZE] + STACK_SIZE,
  //    task_pt_regs(current_task));

  memzero((unsigned long)regs, (unsigned long)regs + sizeof(struct pt_regs));
  regs->pc = pc;
  regs->pstate = PSR_MODE_EL0t;
  regs->sp = (unsigned long)ustack_pool[current_task->pid % TASK_SIZE] + STACK_SIZE;
}
