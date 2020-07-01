#ifndef _SCHED_INCLUDE_H_
#define _SCHED_INCLUDE_H_

#include "task.h"

void sched_init();
void sched_next();

extern volatile uint32_t current_task;
extern int8_t resched;
#endif