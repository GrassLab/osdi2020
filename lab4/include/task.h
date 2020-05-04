#ifndef __TASK_H__
#define __TASK_H__

#define TASK_SIZE 64
#define STACK_SIZE 4096

struct cpu_ctx {
  unsigned long x19;
  unsigned long x20;
  unsigned long x21;
  unsigned long x22;
  unsigned long x23;
  unsigned long x24;
  unsigned long x25;
  unsigned long x26;
  unsigned long x27;
  unsigned long x28;
  unsigned long fp; // x29
  unsigned long lr; // x30
  unsigned long sp;
  // unsigned long pc;
};

struct pt_regs {
	unsigned long regs[31];
	unsigned long sp;
	unsigned long pc;
	unsigned long pstate;
};

#define RESCHED 0x1
typedef struct task_tag {
  struct cpu_ctx cpu_ctx;
  struct pt_regs regs;
  unsigned long pid;
  unsigned long flag;
  unsigned long counter;
  unsigned long priority;
  unsigned long preempt_count;
  enum {
    none,
    pending,
    idle,
    running,
  } status;
} Task;

void task_1();
void task_2();
void task_3();
void task_4();

void preempt_enable();
void preempt_disable();

void init_task_pool();
void kernel_process();
Task *privilege_task_create(void (*func)());

#endif