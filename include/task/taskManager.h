#ifndef _TASKMANAGER_H
#define _TASKMANAGER_H

#include "type.h"
#include "task/taskStruct.h"

extern struct task *current;
extern struct task_queue tq;
extern struct task task_pool[64];
extern char kstack_pool[64][4096];
extern uint32_t task_count;

void _sysGetTaskId();
void _sysFork();
void _sysexec();
void _sysexit();
void doExec(uint64_t start, uint64_t size, uint64_t pc);
uint32_t createPrivilegeTask(void (*func)(), uint32_t priority);
void zombieReaper();

#endif