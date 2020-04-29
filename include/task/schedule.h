#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "type.h"
#include "task/taskStruct.h"

void contextSwitch(struct task* next, bool returnEL0);
void schedule();
void userScheduel();
void checkRSFlag();

#endif