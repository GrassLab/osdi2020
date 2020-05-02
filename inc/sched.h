#ifndef _SCHED_H
#define _SCHED_H

#define THREAD_SIZE                    4096
#define NR_TASKS                       64 
#define TASK_RUNNING                   0

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
    unsigned long pc;//ra
};

struct task_struct {
    struct cpu_context cpu_context;
    long task_id;
    long state;	
    long counter;
    // long priority;
    // long preempt_count;
};


int privilege_task_create(void(*func)());
void sched_init(void);
void schedule(void);
void context_switch(struct task_struct* next);
void cpu_switch_to(struct task_struct* prev, struct task_struct* next);

#endif//_SCHED_H