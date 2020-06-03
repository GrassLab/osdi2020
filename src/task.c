#include "task.h"

#define ENABLE_IRQS_1 ((volatile unsigned int *)(MMIO_BASE + 0x0000B210))
#define AUX_IRQ (1 << 29)
//static struct task init_task = INIT_TASK;
struct task *current;// = &(init_task);
struct task *task_list[TASKS_POOL];// = {&(init_task), };
char kstack_pool[64][4096];
char ustack_pool[64][4096];
int task_num = 0;
struct queue_element runqueue[RUNQUEUE_SIZE];
int runqueue_now;
int runqueue_last;
extern void user_context(unsigned long sp, unsigned long func);
unsigned long uaddr;

struct task* get_current() {
    unsigned long res;
    asm volatile("mrs %0, tpidr_el1" : "=r"(res));
    struct task* res_task = (struct task_t*)res;
    return res_task;
}

void user0() {
    uart_puts("user 0\n");
    while (1);
}
void user1() {
    uart_puts("user 1\n");
    char buf[64];
    //uart_read(buf, 3);
    //uart_write(buf, 3);
    //uart_send('\n');
    //do_fork();
    exec(user0);
    while (1);
}

void test() {
  uart_puts("new task id: ");
  uart_int(task_num);
  uart_send('\n');
  fork();
  uart_puts("Hello\n");
    delay(10000000);
    while (1);
  /*
  while(cnt < 10) {
    //printf("Task id: %d, cnt: %d\n", get_taskid(), cnt);
    delay(100000);
    ++cnt;
  }
  //exit(0);
 // printf("Should not be printed\n");
  //} else {
    //printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", get_taskid(), &cnt, cnt);
    exec(foo);
  }
  */
}

void foo(){
    uart_puts("Task id: ");
    uart_int(current -> task_id);
    uart_send('\n');
    do_exec(user1);
}

void idle(){
  while(1){
    schedule();
  }
}

int privilege_task_create(void(*func)()) {
  struct task *new_task = (struct task_struct *) get_free_page();;
  new_task->task_id = task_num;
  new_task->counter = 1;
  new_task->cpu_context.x19 = (unsigned long)func;
  new_task->cpu_context.lr = (unsigned long)ret_from_fork;
  new_task->cpu_context.sp = (unsigned long)kstack_pool[task_num];
  unsigned long spsr_el1;
  asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
  new_task->spsr = spsr_el1;
  new_task->status = ACTIVE;
  task_list[task_num] = new_task;

  uart_puts("Create new Task: ");
  uart_int(task_list[task_num]->task_id);
  uart_send('\n');
  task_num++;
  runqueue_push(new_task);
  return new_task->task_id;
}

void context_switch(struct task* next) {
  current = get_current();
  if (current == next)
	return;
  struct task* prev = current;
  current = next;
  uart_puts("current task: ");
  uart_int(next->task_id);
  uart_puts("\n");
  switch_to(prev, next, next->spsr);
}

void runqueue_push(struct task* task) {
    runqueue[runqueue_last].task = task;
    runqueue[runqueue_last].is_active = 1;
    runqueue_last = (runqueue_last + 1) % RUNQUEUE_SIZE;
}

struct task* runqueue_pop() {
    struct task* task;
    int now;
    for (int i = 0; i < RUNQUEUE_SIZE; i++) {
        now = (runqueue_now + i) % RUNQUEUE_SIZE;
        if (runqueue[now].is_active) {
            task = runqueue[now].task;
            runqueue_now = (now + 1) % RUNQUEUE_SIZE;
            return task;
        }
    }
    return &task_list[0];
}

void schedule() {
    struct task* task = runqueue_pop();
    context_switch(task);
}

void counter_check() {
  current = get_current();
  current->counter--;
  if (current->counter == 0) {
    //current->counter = 3;
    enable_irq();
    schedule();
    disable_irq();
  }
}

void task_init() {
    runqueue_now = 0;
    runqueue_last = 0;
}

struct utask* get_current_utask() {
    unsigned long res = uaddr;
    //asm volatile("mrs %0, tpidr_el0" : "=r"(res));
    struct utask* res_task = (struct utask*)res;
    return res_task;
}

void set_aux() { *(ENABLE_IRQS_1) = AUX_IRQ; }

void do_exec(void (*func)()) {
    struct task* task = get_current();
    struct page* user_page = page_alloc();
    //for (int i = 0; i < 4 << 13; i++) {
     //   *((unsigned long*)user_page->virtual_addr + i) = *(func + i);
    //}
    page_mapping(task, user_page);

    task->user_task.cpu_context.x19 = *func;
    task->user_task.cpu_context.sp = (unsigned long)ustack_pool[task->task_id];
    unsigned long utask_addr = (unsigned long)&task->user_task;
    uaddr = utask_addr;
    unsigned long spsr_el1;
    asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
    task->spsr = spsr_el1;
    asm volatile("mov     x7, %0" : "=r"(utask_addr));
    asm volatile("msr     tpidr_el0, x7");
    switch_to_user();
}

void switch_to_user() {
    struct task* task = get_current();
    uart_puts("switch to user task\n");
    asm volatile("msr     spsr_el1, %0" :: "r"(0));
    asm volatile("msr     sp_el0, %0" :: "r"(task->user_task.cpu_context.sp));
    asm volatile("msr     elr_el1, %0" :: "r"(task->user_task.cpu_context.x19));
}

void switch_to_new_user() {
    struct task* task = get_current();
    uart_puts("switch to user task\n");
    asm volatile("msr     spsr_el1, %0" :: "r"(0));
    asm volatile("msr     sp_el0, %0" :: "r"(task->user_task.cpu_context.sp));
    asm volatile("msr     elr_el1, %0" :: "r"(task->user_task.cpu_context.x19));
    asm volatile("eret");
}

void do_fork() {
  struct task* parent_task = get_current();
  int child_id = privilege_task_create(switch_to_user);
  struct task* child_task = &task_list[child_id];
  unsigned long sp_el0, func;
  for (int i = 0; i < STACK_SIZE; i++) {
    *(kstack_pool[child_id] + i) = *(kstack_pool[parent_task->task_id] + i);
    *(ustack_pool[child_id] + i) = *(ustack_pool[parent_task->task_id] + i);
  }
  uart_puts("fork id: ");
  uart_int(child_id);
  uart_puts("\n");
  asm volatile("gg:");
  asm volatile("mrs %0, sp_el0" : "=r"(sp_el0));
  uart_puts("sp_el0: ");
  uart_hex(sp_el0);
  uart_puts("\n");
  asm volatile("mrs %0, elr_el1" : "=r"(func));
  uart_puts("func: ");
  uart_hex(func);
  uart_puts("\n");
  child_task->user_task.cpu_context.x19 = parent_task->user_task.cpu_context.x19;
  child_task->user_task.cpu_context.sp = &ustack_pool[child_id] - (&ustack_pool[parent_task->task_id] - sp_el0);
  child_task->cpu_context.sp = &kstack_pool[child_id] - (&kstack_pool[parent_task->task_id] - parent_task->cpu_context.sp);
  child_task->cpu_context.lr = func;
  uart_puts("lr: ");
  uart_hex(child_task->cpu_context.lr);
  uart_puts("\n");

  //child_task->cpu_context.sp = child_task->user_task.cpu_context.sp;
}

int N = 3;
void create_tasks() {
  task_init();
  //for (int num = 0; num < N; num++)
    //privilege_task_create(foo);
  idle();
}
