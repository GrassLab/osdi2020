#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#define THREAD_SIZE                    4096
#define NR_TASKS                       64 

#define TASK_RUNNING                   0
#define TASK_ZOMBIE                    1

extern struct task_struct *current;
extern struct task_struct *task_pool[NR_TASKS];
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
    unsigned long lr;
    unsigned long sp;
};

struct task_struct {
    struct cpu_context cpu_context;
    long schedule_flag;
    long task_id;
    long state;
    long counter;
};

long privilege_task_create(void(*func)());
void do_exec(void(*func)());
void sched_init(void);
void schedule(void);
void timer_tick(void);
void context_switch(struct task_struct* next);
void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
void task_preemption();
void wait(long);
long do_get_taskid();
void do_exec(void(*func)());
void do_fork();
void do_exit();


#endif//_SCHEDULE_H