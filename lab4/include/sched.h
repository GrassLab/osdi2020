#ifndef __SCHED_H__
#define __SCHED_H__
#include "thread.h"

void _schedule(void);
void switch_to(task_t * next);

#endif