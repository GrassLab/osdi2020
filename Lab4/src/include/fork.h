#ifndef _FORK_H
#define _FORK_H

#include "scheduler.h"

int get_availible_pid();
void free_pid(int i);

int privilege_task_create(void(* func));
int user_task_create();
int do_exec(void(* func));
struct trapframe* get_task_trapframe(struct task_struct *task);

struct trapframe{
	unsigned long regs[31];
	unsigned long sp;
	unsigned long elr_el1;
	unsigned long spsr_el1;
};
#endif
