#ifndef _FORK_H
#define _FORK_H

#include "scheduler.h"

int privilege_task_create(void(* func),unsigned long arg);
int fork(unsigned long stack);
int do_exec(unsigned long pc);
struct pt_regs * task_pt_regs(struct task_struct *tsk);

struct pt_regs{
	unsigned long regs[31];
	unsigned long sp;
	unsigned long elr_el1;
	unsigned long spsr_el1;
};
#endif
