#include "schedule/task.h"
#include "MiniUart.h"
#include "schedule/context.h"
#include "schedule/schedule.h"

// since circular queue will waste a space for circulation
TaskStruct *queue_buffer[MAX_TASK_NUM + 1];
Queue running_queue = {.buffer = (void **)queue_buffer,
                       .capacity = MAX_TASK_NUM,
                       .front = 0u,
                       .back = 0u};

TaskStruct ktask_pool[MAX_TASK_NUM] __attribute__((aligned(16u)));
uint8_t kstack_pool[MAX_TASK_NUM][4096] __attribute__((aligned(16u)));

UserTaskStruct utask_pool[MAX_TASK_NUM] __attribute__((aligned(16u)));
uint8_t ustack_pool[MAX_TASK_NUM][4096] __attribute__((aligned(16u)));

void initIdleTaskState() {
    ktask_pool[0].id = 0u;
    ktask_pool[0].counter = 0u;
    ktask_pool[0].reschedule_flag = true;
    ktask_pool[0].in_use = true;
    asm volatile("msr tpidr_el1, %0"
                 : /* output operands */
                 : "r"(&ktask_pool[0]) /* input operands */
                 : /* clobbered register */);
}

int createPrivilegeTask(void (*func)()) {
    for (uint32_t i = 1; i < MAX_TASK_NUM; ++i) {
        if (ktask_pool[i].in_use == false) {
            ktask_pool[i].in_use = true;
            ktask_pool[i].id = i;
            ktask_pool[i].counter = 10u;
            ktask_pool[i].reschedule_flag = false;

            ktask_pool[i].kernel_context.lr = (uint64_t)func;

            // +1 since stack grows toward lower address
            ktask_pool[i].kernel_context.fp = (uint64_t)kstack_pool[i + 1];
            ktask_pool[i].kernel_context.sp = (uint64_t)kstack_pool[i + 1];

            ktask_pool[i].user_task = utask_pool + i;
            ktask_pool[i].user_task->id = i;
            ktask_pool[i].user_task->regain_resource_flag = false;

            pushQueue(&running_queue, ktask_pool + i);
            return 0;
        }
    }

    sendStringUART("[ERROR] No more tasks can be created!\n");
    return 1;
}

void checkRescheduleFlag(void) {
    TaskStruct *cur_task = getCurrentTask();

    if (cur_task->reschedule_flag) {
        schedule();

        // regain the resource
        cur_task->user_task->regain_resource_flag = true;
    }
}

static void doExec(void (*func)()) {
    TaskStruct *cur_task = getCurrentTask();
    UserTaskStruct *user_task = cur_task->user_task;

    user_task->user_context.lr = (uint64_t)func;

    // +1 since stack grows toward lower address
    user_task->user_context.fp = (uint64_t)ustack_pool[user_task->id + 1];
    user_task->user_context.sp = (uint64_t)ustack_pool[user_task->id + 1];

    initUserTaskandSwitch(&cur_task->kernel_context,
                          &cur_task->user_task->user_context);
}

// user task
static void barTask(void) {
    UserTaskStruct *cur_task = getUserCurrentTask();

    sendStringUART("\nHi, I'm ");
    sendHexUART(cur_task->id);
    sendStringUART(" in user mode...\n");

    sendStringUART("Doing user routine for awhile...\n");

    while (cur_task->regain_resource_flag == false)
        ;

    // reset
    cur_task->regain_resource_flag = false;
}

// kernel task
void fooTask(void) {

    // kernel routine
    TaskStruct *cur_task = getCurrentTask();

    sendStringUART("\nHi, I'm ");
    sendHexUART(cur_task->id);
    sendStringUART(" in kernel mode...\n");
    sendStringUART("Doing kernel routine for awhile...\n");

    doExec(barTask);
}
