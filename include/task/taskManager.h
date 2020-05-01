#ifndef _TASKMANAGER_H
#define _TASKMANAGER_H

#include "type.h"
#include "task/taskStruct.h"

extern struct task *current;
extern struct task_queue tq;
extern struct task task_pool[64];
extern uint32_t task_count;

void _sysFork();
void _sysexec();
void _sysexit();
void doExec(void (*func)());
uint32_t createPrivilegeTask(void (*func)(), uint32_t priority);
void zombieReaper();

#endif