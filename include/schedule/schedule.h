#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "type.h"
#include "schedule/task.h"

extern struct task* current;

void do_exec(void(*func)(), int priority);
int createPrivilegeTask(void(*func)(), int priority);
void contextSwitch(struct task* next, bool returnEL0);
void schedule();
void userScheduel();
void checkRSFlag();

#endif