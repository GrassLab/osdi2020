#ifndef __SCHED_H__
#define __SCHED_H__

#include "task.h"
void schedule();
void context_switch(Task *next);

#endif
