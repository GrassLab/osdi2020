#include "schedule/context.h"
#include "schedule/task.h"

void switchContext(struct __KernelContext *next_task) {
    switchCpuContext((struct __KernelContext *)getCurrentTask(), next_task);
}

void switchUserToKernel(void) {
    TaskStruct *cur_task = getCurrentTask();

    switchToEl1(&cur_task->user_task->user_context, &cur_task->kernel_context);
}

void switchKernelToUser(void) {
    TaskStruct *cur_task = getCurrentTask();

    switchToEl0(&cur_task->kernel_context, &cur_task->user_task->user_context);
}
