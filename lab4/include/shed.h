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

#define PF_KTHREAD		            	0x00000002

#define PSR_MODE_EL0t	0x00000000
#define PSR_MODE_EL1t	0x00000004
#define PSR_MODE_EL1h	0x00000005
#define PSR_MODE_EL2t	0x00000008
#define PSR_MODE_EL2h	0x00000009
#define PSR_MODE_EL3t	0x0000000c
#define PSR_MODE_EL3h	0x0000000d

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
    unsigned long stack;
    unsigned long flag;
    int task_id;
    int parent_id;
};

struct pt_regs {
	unsigned long regs[31];
	unsigned long sp;
	unsigned long pc;
	unsigned long pstate;
};

extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
void enable_preempt();
void disable_preempt();
void privilege_task_create(void (*func)());
void timer_tick();
int move_to_user_mode();
struct pt_regs * task_pt_regs(struct task_struct *tsk);
void _do_exec(void(*func)());
void _do_exit();
int _do_fork();
int get_taskid();
int num_runnable_tasks();
struct task_struct *get_current_task();

#define INIT_TASK \
{   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
    0, 5, 0, 1, 0, PF_KTHREAD, 0, 0 \
}

#endif
#endif


