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

static void handleExec(uint64_t *trapframe) {
    // first parameter of svc is syscall number, access second parameter for
    // real argument
    doExec((void (*)())trapframe[1]);
}

static void handleFork(uint64_t *trapframe) {
    doFork(trapframe);

}

void handleSVC(uint64_t *trapframe) {
    TaskStruct *cur_task = getCurrentTask();

    uint64_t x0 = trapframe[0];
    switch (x0) {
    case 0:
        handleRecvUart(trapframe);
        break;
    case 1:
        handleSendUart(trapframe);
        break;
    case 2:
        handleExec(trapframe);
        break;
    case 3:
        handleFork(trapframe);
        break;
    default:
        sendStringUART("[ERROR] Unknown syscall number, shouldn't reach here\n");
        break;
    }
}
