#ifndef _TASK_H
#define _TASK_H

#include "type.h"

struct cpu_context 
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
	unsigned long pc;
	unsigned long sp;
};

typedef enum
{
    ready,
	running,
	pending
} state;

struct task
{
    struct cpu_context cpu_context;
	int task_id;
	int priority;
	state task_state;
	bool re_schedule;
};

#endif