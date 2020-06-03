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

static void handleFork(uint64_t *trapframe) { doFork(trapframe); }

static void handleExit(uint64_t *trapframe) { doExit(trapframe[1]); }

static void handleGetTaskId(uint64_t *trapframe) {
    TaskStruct *cur_task = getCurrentTask();

    trapframe[0] = cur_task->id;
}

void handleSVC(uint64_t *trapframe) {
    TaskStruct *cur_task = getCurrentTask();

    uint64_t x0 = trapframe[0];
    switch (x0) {
    case SYS_RECV_UART:
        handleRecvUart(trapframe);
        break;
    case SYS_SEND_UART:
        handleSendUart(trapframe);
        break;
    case SYS_EXEC:
        handleExec(trapframe);
        break;
    case SYS_FORK:
        handleFork(trapframe);
        break;
    case SYS_EXIT:
        handleExit(trapframe);
        break;
    case SYS_GET_TASK_ID:
        handleGetTaskId(trapframe);
        break;
    default:
        sendStringUART(
            "[ERROR] Unknown syscall number, shouldn't reach here\n");
        break;
    }
}
