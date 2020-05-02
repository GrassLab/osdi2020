#include "schedule/task.h"
#include "MiniUart.h"
#include "exception/exception.h"
#include "schedule/context.h"
#include "schedule/schedule.h"
#include "sys/unistd.h"
#include "utils.h"

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

void idle(void) {
    while (1) {
        sendStringUART("Enter idle state ...\n");
        // TODO: zombie reaper
        //   There is no need for now since each task has their own fixed resources.

        if (isQueueEmpty(&running_queue)) {
            break;
        }
        schedule();
    }
    sendStringUART("\n\nTest finished\n\n");
    while (1)
        ;
}

void initIdleTaskState() {
    ktask_pool[0].id = 0u;
    ktask_pool[0].counter = 0u;
    ktask_pool[0].reschedule_flag = true;
    ktask_pool[0].status = kInUse;
    asm volatile("msr tpidr_el1, %0"
                 : /* output operands */
                 : "r"(&ktask_pool[0]) /* input operands */
                 : /* clobbered register */);
}

int64_t createPrivilegeTask(void (*func)()) {
    for (uint32_t i = 1; i < MAX_TASK_NUM; ++i) {
        if (ktask_pool[i].status != kInUse) {
            ktask_pool[i].status = kInUse;
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
            return i;
        }
    }

    sendStringUART("[ERROR] No more tasks can be created!\n");
    return -1;
}

void checkRescheduleFlag(void) {
    TaskStruct *cur_task = getCurrentTask();

    if (cur_task->reschedule_flag) {
        schedule();

        // regain the resource
        cur_task->user_task->regain_resource_flag = true;
    }
}

void doExec(void (*func)()) {
    TaskStruct *cur_task = getCurrentTask();
    UserTaskStruct *user_task = cur_task->user_task;

    user_task->user_context.lr = (uint64_t)func;

    // +1 since stack grows toward lower address
    user_task->user_context.fp = (uint64_t)ustack_pool[user_task->id + 1];
    user_task->user_context.sp = (uint64_t)ustack_pool[user_task->id + 1];

    initUserTaskandSwitch(&cur_task->kernel_context,
                          &cur_task->user_task->user_context);
}

static void copyUserContext(int64_t id) {
    TaskStruct *cur_task = getCurrentTask();

    UserContext *src_ctx = &cur_task->user_task->user_context;
    UserContext *dst_ctx = &utask_pool[id].user_context;

    // fp, lr should be populated by _kernel_exit
    // sp should be populated by copyStacks()
    dst_ctx->elr_el1 = src_ctx->elr_el1;
    dst_ctx->spsr_el1 = src_ctx->spsr_el1;
}

void copyStacks(int64_t id) {
    TaskStruct *cur_task = getCurrentTask();

    // kernel
    uint8_t *src_sp = kstack_pool[cur_task->id];
    uint8_t *dst_sp = kstack_pool[id];

    memcpy(dst_sp, src_sp, 4096);

    // bottom - current
    uint64_t src_size =
        (uint64_t)kstack_pool[cur_task->id + 1] - cur_task->kernel_context.sp;
    ktask_pool[id].kernel_context.sp = (uint64_t)kstack_pool[id + 1] - src_size;

    // user
    src_sp = ustack_pool[cur_task->id];
    dst_sp = ustack_pool[id];

    memcpy(dst_sp, src_sp, 4096);

    src_size = (uint64_t)ustack_pool[cur_task->id + 1] -
               cur_task->user_task->user_context.sp;
    utask_pool[id].user_context.sp = (uint64_t)ustack_pool[id + 1] - src_size;
}

static void updateTrapFrame(int64_t id) {
    TaskStruct *child_task = ktask_pool + id;

    uint64_t *trapframe = (uint64_t *)child_task->kernel_context.sp;

    // retval
    trapframe[0] = 0;

    // fp
    trapframe[29] = (uint64_t)ustack_pool[id + 1] -
                    // gap b/t parent's current fp and it's stack orignal base
                    ((uint64_t)ustack_pool[getCurrentTask()->id + 1] - trapframe[29]);
}

void doFork(uint64_t *trapframe) {
    TaskStruct *cur_task = getCurrentTask();

    int64_t new_task_id = createPrivilegeTask(_child_return_from_fork);
    if (new_task_id == -1) {
        // TODO:
        sendStringUART("[ERROR] fail to create privilege task\n");
        return;
    }

    copyUserContext(new_task_id);

    cur_task->kernel_context.sp = (uint64_t)trapframe;
    copyStacks(new_task_id);

    // child's fp, retval should be different from parent's ones
    updateTrapFrame(new_task_id);
}

void doExit(int status) {
    TaskStruct *cur_task = getCurrentTask();

    // TODO: handle status code, where to use???

    cur_task->status = kZombie;
    cur_task->reschedule_flag = true;
}

static void barTask(void);
static void forkTask(void);

// kernel task
void fooTask(void) {

    // kernel routine
    TaskStruct *cur_task = getCurrentTask();

    sendStringUART("\nHi, I'm ");
    sendHexUART(cur_task->id);
    sendStringUART(" in kernel mode...\n");
    sendStringUART("Doing kernel routine for awhile...\n");

    doExec(forkTask);
}

// ------ For User Mode ----------------
static void forkedTask(void) {
    int tmp = 5;
    UserTaskStruct *cur_task = getUserCurrentTask();

    writeStringUART("Task ");
    writeHexUART(cur_task->id);
    writeStringUART(" after exec, tmp address ");
    writeHexUART((uint64_t)&tmp);
    writeStringUART(", tmp value ");
    writeHexUART(tmp);
    writeUART('\n');

    exit(0);
}

static void forkTask(void) {
    int cnt = 1;
    if (fork() == 0) {
        fork();
        delay(100000);
        fork();

        UserTaskStruct *cur_task = getUserCurrentTask();

        while (cnt < 10) {
            writeStringUART("Task id ");
            writeHexUART(cur_task->id);
            writeStringUART(", cnt: ");
            writeHexUART(cnt);
            writeUART('\n');
            delay(100000);
            ++cnt;
        }
        exit(0);

        writeStringUART("Shouldn't reach here\n");
    } else {
        UserTaskStruct *cur_task = getUserCurrentTask();

        writeStringUART("Task ");
        writeHexUART(cur_task->id);
        writeStringUART(" before exec, cnt address ");
        writeHexUART((uint64_t)&cnt);
        writeStringUART(", cnt value ");
        writeHexUART(cnt);
        writeUART('\n');
        exec(forkedTask);
    }
}

static void bazTask(void) {
    UserTaskStruct *cur_task = getUserCurrentTask();

    writeStringUART("\nHi, I'm ");
    writeHexUART(cur_task->id);
    writeStringUART(" in user mode...\n");

    writeStringUART("Doing bazTask() for awhile...\n");

    while (cur_task->regain_resource_flag == false)
        ;

    // reset
    cur_task->regain_resource_flag = false;
}

// main user task
static void barTask(void) {
    UserTaskStruct *cur_task = getUserCurrentTask();

    writeStringUART("\nHi, I'm ");
    writeHexUART(cur_task->id);
    writeStringUART(" in user mode...\n");

    writeStringUART("Doing barTask() for awhile...\n");

    while (cur_task->regain_resource_flag == false)
        ;

    // reset
    cur_task->regain_resource_flag = false;

    exec(bazTask);
}
