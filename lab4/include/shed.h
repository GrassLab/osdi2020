#ifndef _TASK_STRUCT
#define _TASK_STRUCT
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

struct task_struct {
	struct cpu_context cpu_context;
	long state;	
	long counter;
	long priority;
	long preempt_count;
    int taskid;
};
#endif

#define NR_TASKS				64 
#define THREAD_SIZE				4096
#define THREAD_CPU_CONTEXT		0 		// offset of cpu_context in task_struct 



#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	0,0,1, 0, 0 \
}
void foo();
void idle();
void schedule();
void create_foo();
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);

