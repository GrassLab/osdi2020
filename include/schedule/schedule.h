#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "schedule/task.h"

extern struct task* current;

int createPrivilegeTask(void(*func)(), int priority);
void contextSwitch(struct task* next);
void schedule();

#endif