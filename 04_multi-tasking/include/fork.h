#ifndef _FORK_H
#define _FORK_H

#include "sched.h"

#define PSR_MODE_EL0t   0x00000000

enum privilege_t {KERNEL_MODE, USER_MODE};

int _task_create(enum privilege_t, unsigned long);
int do_exec(unsigned long);
int _fork();

struct pt_regs* get_pt_regs(struct task_struct *p);

struct pt_regs {
    unsigned long regs[31];
    unsigned long sp;
    unsigned long pc;
    unsigned long pstate;
};

#endif
