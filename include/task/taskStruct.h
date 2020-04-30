#ifndef _TASKSTRUCT_H
#define _TASKSTRUCT_H

#include "type.h"

struct cpu_context
{
	unsigned long x19; //0
	unsigned long x20;
	unsigned long x21; //1
	unsigned long x22;
	unsigned long x23; //2
	unsigned long x24;
	unsigned long x25; //3
	unsigned long x26;
	unsigned long x27; //4
	unsigned long x28;
	unsigned long fp; //5
	unsigned long pc;
	unsigned long sp; //6
	unsigned long elr_el1;
	unsigned long sp_el0; //7
};

typedef enum
{
	ready,
	running,
	pending
} state;

struct task
{
	struct cpu_context kernel_context;
	int task_id;
	int priority;
	state task_state;
	bool re_schedule;
};

#endif