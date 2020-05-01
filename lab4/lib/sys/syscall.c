#include "sys/syscall.h"
#include "MiniUart.h"
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
    default:
        sendStringUART("[ERROR] Unknown syscall number, shouldn't reach here");
        break;
    }
}
