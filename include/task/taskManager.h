#ifndef _TASKMANAGER_H
#define _TASKMANAGER_H

#include "type.h"
#include "task/taskStruct.h"

extern struct task* current;
extern struct task task_pool[64];
extern int task_num;

void _sysFork();
void doExec(void(*func)());
int createPrivilegeTask(void(*func)(), int priority);

#endif