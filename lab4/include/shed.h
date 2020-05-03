#ifndef _SHED_H
#define _SHED_H

#define THREAD_CPU_CONTEXT  0

#ifndef __ASSEMBLER__

#define THREAD_SIZE 4096

#define NR_TASKS    64

#define FIRST_TASK  task[0]
#define LAST_TASK   task[NR_TASKS - 1]

#define TASK_RUNNING    0
#define TASK_ZOMBIE     1

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
    int task_id;
    int parent_id;
};

extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
void enable_preempt();
void disable_preempt();
void privilege_task_create(void (*func)());

#define INIT_TASK \
{   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
    0, 0, 1, 0, 0 \
}

#endif
#endif


