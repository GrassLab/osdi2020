#ifndef _FORK_H
#define _FORK_H
#include "sched.h"
#define PSR_MODE_EL0t	0x00000000
int privilege_task_create(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long stack);
int do_exec(unsigned long pc);
struct pt_regs {
    unsigned long regs[31];
    unsigned long sp;
    unsigned long pc;
    unsigned long pstate;
};
struct pt_regs* task_pt_regs(struct task_struct *tsk);

#endif