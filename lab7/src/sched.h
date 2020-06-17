#ifndef _SCHED_H
#define _SCHED_H

#define THREAD_CPU_CONTEXT          0       // offset of cpu_context in task

#ifndef __ASSEMBLER__

#define THREAD_SIZE             4096

#define NR_TASKS                64 


#define FIRST_TASK task_pool[0]
#define LAST_TASK task_pool[NR_TASKS-1]

#define TASK_RUNNING                0
#define TASK_ZOMBIE                 1

#define PF_KTHREAD                      0x00000002

#define INIT_TASK \
/*cpu_context*/ { {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */ 0,0,1,0,0,PF_KTHREAD,0,0\
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
    unsigned long fp; //x29
    unsigned long sp; 
    unsigned long pc; //x30
};

struct task {
    struct cpu_context cpu_context;
    long state;
    long counter;
    long priority;
    long preempt_count;
    unsigned long stack;
    unsigned long flags;
    int id;
    int need_resched;
};

extern struct task *current;
extern struct task *task_pool[NR_TASKS];
extern int nr_tasks; //the number of currently running tasks
extern int exist;

extern void Schedule(void);
extern void timer_tick(void);
extern void exit_process(void);
extern void preempt_disable(void);
extern void preempt_enable(void);
extern void context_switch(struct task *next);
extern void switch_to(struct task *prev, struct task *next);

extern void set_tsk_need_resched();
extern void clear_tsk_need_resched();
extern int need_resched();

char kstack_pool[64][4096];

#endif
#endif
