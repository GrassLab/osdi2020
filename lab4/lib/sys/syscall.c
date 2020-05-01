#include "sys/syscall.h"
#include "MiniUart.h"
#include "exception/exception.h"
#include "schedule/context.h"
#include "schedule/task.h"

#include <assert.h>

static void handleRecvUart(uint64_t *trapframe) {
    char retval = recvUART();

    // write to x0 of user context
    trapframe[0] = (uint64_t)retval;
}

static void handleSendUart(uint64_t *trapframe) {
    // first parameter of svc is syscall number, access second parameter for
    // real argument
    sendUART(trapframe[1]);
}

static void handleExec(uint64_t *trapframe) {
    // first parameter of svc is syscall number, access second parameter for
    // real argument
    doExec((void (*)())trapframe[1]);
}

static void handleFork(uint64_t *trapframe) {
    TaskStruct *cur_task = getCurrentTask();

    int64_t new_task_id = createPrivilegeTask(_child_return_from_fork);
    if (new_task_id == -1) {
        // TODO:
        sendStringUART("[ERROR] fail to create privilege task\n");
        return;
    }

    copyContexts(new_task_id);
    copyStacks(new_task_id);

    // child's fp, retval should be different from parent's ones
    updateTrapFrame(new_task_id);

    trapframe[0] = new_task_id;
}

void handleSVC(void) {
    TaskStruct *cur_task = getCurrentTask();

    uint64_t x0 = *(uint64_t *)cur_task->kernel_context.sp;
    switch (x0) {
    case 0:
        handleRecvUart((uint64_t *)cur_task->kernel_context.sp);
        break;
    case 1:
        handleSendUart((uint64_t *)cur_task->kernel_context.sp);
        break;
    case 2:
        handleExec((uint64_t *)cur_task->kernel_context.sp);
        break;
    case 3:
        handleFork((uint64_t *)cur_task->kernel_context.sp);
        break;
    default:
        sendStringUART("[ERROR] Unknown syscall number, shouldn't reach here");
        break;
    }
}
