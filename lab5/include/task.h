#ifndef __TASK_H__
#define __TASK_H__

#define TASK_SIZE 64
#define STACK_SIZE 4096
#define PSR_MODE_EL0t	0x00000000
#define PSR_MODE_EL1t	0x00000004
#define PSR_MODE_EL1h	0x00000005
#define PSR_MODE_EL2t	0x00000008
#define PSR_MODE_EL2h	0x00000009
#define PSR_MODE_EL3t	0x0000000c
#define PSR_MODE_EL3h	0x0000000d


#ifndef __ASSEMBLER__

#include "mutex.h"

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

#define TASK_BUFFER_SIZE 1024
#define RESCHED 0x1
#define SIGKILL 0x1

#define MAX_PROCESS_PAGES			16

struct user_page {
	unsigned long phys_addr;
	unsigned long virt_addr;
};

struct mm_struct {
	unsigned long pgd;
	int user_pages_count;
	struct user_page user_pages[MAX_PROCESS_PAGES];
	int kernel_pages_count;
	unsigned long kernel_pages[MAX_PROCESS_PAGES];
};

typedef struct task_tag {
  struct cpu_ctx cpu_ctx;
  char buffer[TASK_BUFFER_SIZE];
  unsigned long pid;
  unsigned long flag;
  unsigned long signals;
  unsigned long counter;
  unsigned long priority;
  unsigned long preempt_count;
  Mutex* mtx;
  enum {
    none,
    idle,
    zombie,
    block,
    sleep,
    running,
  } status;
	struct mm_struct mm;
} Task;

void task_1();
void task_2();
void task_3();
void task_4();

void preempt_enable();
void preempt_disable();
int preempt_reschedable();

void init_task_pool();
void kernel_process();
Task *privilege_task_create(void (*func)(), unsigned long arg, unsigned long p);
void do_exec(unsigned long);
int move_to_user_mode(unsigned long start, unsigned long size, unsigned long pc);
#endif
#endif
