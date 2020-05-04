#ifndef _SCHED_H
#define _SCHED_H

#define THREAD_CPU_CONTEXT			0 		// offset of cpu_context in task_struct 
#ifndef __ASSEMBLER__

#define THREAD_SIZE				4096
#define TASK_RUNNING			0
#define TASK_ZOMBIE				1
#define TASK_WAITING            2


#define NR_TASKS				64 
#define PF_KTHREAD		        0x00000002	

extern struct task_struct *current;
extern struct task_struct *task[NR_TASKS];
extern int nr_tasks;

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
	long task_id;
    long state;
    long counter;
    long priority;
    long preempt_count;
	unsigned long stack;
	unsigned long flags;
	unsigned long kill_flag;
};

void preempt_disable(void);
void preempt_enable(void);
void schedule();
void switch_to(struct task_struct * next);
void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
void schedule_uart();
#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	0,0,0,1, 0, 0, PF_KTHREAD, 0 , 0 \
}

#endif

#endif