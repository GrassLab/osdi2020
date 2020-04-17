#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define THREAD_CPU_CONTEXT 0 	// offset of cpu_context in task_struct 

#ifndef __ASSEMBLER__
#define NR_TASKS 64 
#define THREAD_SIZE  4096

#define TASK_RUNNING	0

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
    unsigned long pc;
    unsigned long sp;
};

struct task_struct{
	struct cpu_context cpu_context;
	long state;
	long counter;
};

extern void schedule(void);
extern void context_switch(struct task_struct* next);
extern void switch_to(struct task_struct* prev, struct task_struct* next);
extern struct task* get_current();

#define INIT_TASK { {0,0,0,0,0,0,0,0,0,0,0,0,0}, 0,0} 

#endif
#endif /*_SCHEDULER_H */
