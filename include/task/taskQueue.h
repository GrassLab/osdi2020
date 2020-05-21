#ifndef _TASKQUEUE_H
#define _TASKQUEUE_H

#include "type.h"
#include "task/taskStruct.h"

void pushQueue(struct task *ready_task);
struct task *popQueue();

#endif