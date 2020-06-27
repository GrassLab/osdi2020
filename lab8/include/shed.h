#ifndef _TASK_STRUCT
#define _TASK_STRUCT

#define NR_TASKS				64 
#define THREAD_SIZE				4096
#define THREAD_CPU_CONTEXT		0 		// offset of cpu_context in task_struct 
#define TASK_RUNNING			0
#define TASK_ZOMBIE				1



#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	0,0,1, 0, 0 \
}
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
	int storeid;
    int taskid;
	int parentid;
	int pfn;

	// page table
    unsigned long pgd;
    unsigned long pud;
    unsigned long pmd;
    unsigned long pte;
};

#endif

void foo();
void foo_sys();
void idle();
void schedule();
void create_foo();
void init_init_task();
void update_task_counter();
void exit();
int get_taskid();
void enable_preempt();
int fork();
void exec(void (*func)());

void set_pgd(unsigned long pgd);

extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);

