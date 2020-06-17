#ifndef _SCHED_H
#define _SCHED_H

#define CPU_CONTEXT_OFFSET 0
#ifndef __ASSEMBLER__

#define NR_TASKS 64

// identified it is kernel thread
#define PF_KTHREAD 0x00000002

typedef long tid_t;
enum state_t {TASK_RUNNING, TASK_ZOMBIE};

extern struct task_struct *current;
extern struct task_struct *tasks[NR_TASKS];
extern unsigned int nr_tasks;

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
    unsigned long fp;  // x29 
    unsigned long sp; 
    unsigned long pc;  // x30
};

struct task_struct {
    struct cpu_context cpu_context;
    tid_t task_id;
    enum state_t state;
    long counter;
    long priority;
    int preempt_count;
    unsigned long stack;
    unsigned long flags;
};

#define INIT_TASK \
{ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
   0, TASK_RUNNING, 5, 5, 0, PF_KTHREAD, 0}

tid_t acquire_unused_task_id();
void preempt_enable();
void preempt_disable();
void schedule();
void cpu_switch(struct task_struct*, struct task_struct*);
void exit_process();

#endif
#endif
