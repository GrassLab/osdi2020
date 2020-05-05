#include "tool.h"

struct cpu_context {
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
	unsigned long fp;
	unsigned long sp;
	unsigned long pc;
};

struct task {
  struct cpu_context cpu_context;
  long state;
  long counter;
  long priority;
  long preempt_count;
  int task_id;
};

#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	0,0,1, 0, 0 \
}
#define TASKS_POOL			64

void foo();
void idle();
void privilege_task_create(void(*func)());
void context_switch(struct task* next);
void schedule();
void create_tasks();
extern void switch_to(struct task* prev, struct task* next);
