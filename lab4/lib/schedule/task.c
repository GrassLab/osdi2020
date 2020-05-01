#include "schedule/task.h"
#include "MiniUart.h"
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

int64_t createPrivilegeTask(void (*func)()) {
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

void copyContexts(int64_t id) {
    TaskStruct *cur_task = getCurrentTask();

    uint8_t *src_ctx = (uint8_t *)&cur_task->kernel_context;
    uint8_t *dst_ctx = (uint8_t *)&ktask_pool[id].kernel_context;

    // only x19-x28
    memcpy(dst_ctx, src_ctx, sizeof(uint64_t) * 10);

    UserContext *src = &cur_task->user_task->user_context;
    UserContext *dst = &utask_pool[id].user_context;

    // fp, lr should be populated by _kernel_exit
    // sp should be populated by copyStacks()
    dst->elr_el1 = src->elr_el1;
    dst->spsr_el1 = src->spsr_el1;
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

void updateTrapFrame(int64_t id) {
    TaskStruct *child_task = ktask_pool + id;

    uint64_t *trapframe = (uint64_t *)child_task->kernel_context.sp;

    // retval
    trapframe[0] = 0;
    // fp
    trapframe[28] = (uint64_t)ustack_pool[id + 1];
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

    while (cur_task->regain_resource_flag == false)
        ;

    // reset
    cur_task->regain_resource_flag = false;
}

static void forkTask(void) {
    int cnt = 1;
    if (fork() == 0) {
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
