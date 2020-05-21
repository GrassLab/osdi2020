#include "tool.h"
#include "gpio.h"

#define STACK_SIZE				4096
#define RUNQUEUE_SIZE           64

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
	unsigned long lr;
	unsigned long sp;
    unsigned long elr_el1;
	unsigned long sp_el0; //7
};

struct utask {
  struct cpu_context cpu_context;
  unsigned long spsr;
  unsigned long elr;
  unsigned long fork_id;
};

struct task {
  struct cpu_context cpu_context;
  struct utask user_task;
  long spsr;
  long counter;
  long reschedule;
  long preempt_count;
  long status;
  int task_id;
};

#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	0,0,2, 0, 0 \
}
#define TASKS_POOL			64

struct queue_element {
    struct task* task;
    int is_active;
};

//struct queue_element runqueue[RUNQUEUE_SIZE];
//int runqueue_now;
//int runqueue_last;

enum task_state {
    ACTIVE,
    INACTIVE,
    ZOMBIE,
};

void foo();
void idle();
int privilege_task_create(void(*func)());
void context_switch(struct task* next);
void schedule();
void counter_check();
void create_tasks();
void switch_to_user();
void switch_to_new_user();
extern void switch_to(struct task* prev, struct task* next, unsigned long spsr);
