#ifndef __SCHEDULE_TASK_H
#define __SCHEDULE_TASK_H

#include "mmu/vma.h"
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

    // PGD's frame for user space
    uint64_t physical_pgd;
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
    uint64_t id;
    uint64_t regain_resource_flag;
} UserTaskStruct;

extern const uint64_t kUnUse;
extern const uint64_t kInUse;
extern const uint64_t kZombie;

typedef struct __TaskStruct {
    KernelContext kernel_context;
    UserTaskStruct *user_task;
    uint64_t id;
    uint64_t counter;
    uint64_t status;
    uint64_t reschedule_flag;

    Page *pgd;

    // for concatenating pages so that we can free pages easily
    Page *tail_page;
} TaskStruct;

extern TaskStruct ktask_pool[MAX_TASK_NUM] __attribute__((aligned(16u)));
extern uint8_t kstack_pool[MAX_TASK_NUM][4096] __attribute__((aligned(16u)));

extern UserTaskStruct utask_pool[MAX_TASK_NUM] __attribute__((aligned(16u)));
extern uint8_t ustack_pool[MAX_TASK_NUM][4096] __attribute__((aligned(16u)));

extern const uint64_t kDefaultStackVirtualAddr;

extern Queue running_queue;

void idle(void);
void initIdleTaskState(void);
int64_t createPrivilegeTask(void (*func)());
void doExec(void (*func)());
void checkRescheduleFlag(void);

void doFork(uint64_t *trapframe);
void doExit(int status);

// for testing scheduler
void fooTask(void);

#endif
