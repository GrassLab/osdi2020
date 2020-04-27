#ifndef _TASK_H_
#define _TASK_H_

#include "irq.h"

#define PSR_MODE_EL0t 0x00000000
#define PSR_MODE_EL1t 0x00000004
#define PSR_MODE_EL1h 0x00000005
#define PSR_MODE_EL2t 0x00000008
#define PSR_MODE_EL2h 0x00000009
#define PSR_MODE_EL3t 0x0000000c
#define PSR_MODE_EL3h 0x0000000d

#define PF_KTHREAD 0x00000002

#define TASK_NUM 16
enum
{
    TASK_IDLE,
    TASK_RUNNING,
    TASK_ZOMBIE
};

typedef struct cpu_context_t
{
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
} cpu_context_t;

/*
SP_EL0: The address of user mode’s stack pointer.

ELR_EL1: The program counter of user mode’s procedure.

SPSR_EL1: The CPU state of user mode.
*/
typedef struct user_context_t
{
    unsigned long regs[31];
    unsigned long sp;
    unsigned long pc;
    unsigned long pstate;
} user_context_t;

typedef struct task_t
{
    cpu_context_t cpu_context;
    long state;
    long counter;
    long priority;
    long preempt_count;
    unsigned long stack;
    unsigned long flags;
} task_t;

extern int get_current();
extern void set_current(int);
extern unsigned long ret_from_fork();
extern void switch_to(task_t *, task_t *);

void schedule();

int privilege_task_create(unsigned long func, unsigned long arg);
user_context_t *task_user_context(task_t *task);

int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long stack);

int do_exec(unsigned long pc);

int do_fork();

void task_init();

void context_switch(int task_id);

void schedule();

int check_reschedule();

void timer_tick();

void schedule_cnt_add(int n);

void exit_process();

#endif