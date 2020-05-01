#ifndef __SCHEDULE_TASK_H
#define __SCHEDULE_TASK_H

#include "queue/queue.h"

#include <stdbool.h>
#include <stdint.h>

#define MAX_TASK_NUM 64

typedef struct __KernelContext {
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t fp;
    uint64_t lr;

    uint64_t sp;
} KernelContext;

typedef struct __UserContext {
    uint64_t fp;
    uint64_t lr;

    uint64_t sp;
    uint64_t elr_el1;
    uint64_t spsr_el1;
} UserContext;

typedef struct __UserTaskStruct {
    UserContext user_context;
    uint32_t id;
    bool regain_resource_flag;
} UserTaskStruct;

typedef struct __TaskStruct {
    KernelContext kernel_context;
    UserTaskStruct *user_task;
    uint32_t id;
    uint32_t counter;
    bool in_use;
    bool reschedule_flag;
} TaskStruct;

extern TaskStruct ktask_pool[MAX_TASK_NUM] __attribute__((aligned(16u)));
extern uint8_t kstack_pool[MAX_TASK_NUM][4096] __attribute__((aligned(16u)));

extern UserTaskStruct utask_pool[MAX_TASK_NUM] __attribute__((aligned(16u)));
extern uint8_t ustack_pool[MAX_TASK_NUM][4096] __attribute__((aligned(16u)));

extern Queue running_queue;

void initIdleTaskState(void);
int64_t createPrivilegeTask(void (*func)());
void doExec(void (*func)());
void checkRescheduleFlag(void);

void copyContexts(int64_t id);
void copyStacks(int64_t id);
void updateTrapFrame(int64_t id);

// for testing scheduler
void fooTask(void);

#endif
