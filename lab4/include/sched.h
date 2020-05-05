#ifndef __SCHED_H__
#define __SCHED_H__

#include "task.h"
void schedule();
void check_resched();
void context_switch(Task *next);
void ret_from_fork(void);
#endif
