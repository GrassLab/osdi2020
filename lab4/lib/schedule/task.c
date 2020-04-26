#include "schedule/task.h"
#include "MiniUart.h"
#include "schedule/context.h"
#include "schedule/schedule.h"

TaskStruct task_pool[MAX_TASK_NUM] __attribute__((aligned(16u)));
uint8_t kstack_pool[MAX_TASK_NUM][4096] __attribute__((aligned(16u)));
TaskStruct *queue_buffer[MAX_TASK_NUM];

Queue running_queue = {.buffer = (void **)queue_buffer,
                       .capacity = MAX_TASK_NUM,
                       .front = 0u,
                       .back = 0u};

void initIdleTaskState() {
    task_pool[0].in_use = true;
    task_pool[0].id = 0u;
    asm volatile("msr tpidr_el1, %0"
                 : /* output operands */
                 : "r"(&task_pool[0]) /* input operands */
                 : /* clobbered register */);
}

int createPrivilegeTask(void (*func)()) {
    for (uint32_t i = 1; i < MAX_TASK_NUM; ++i) {
        if (task_pool[i].in_use == false) {
            task_pool[i].in_use = true;
            task_pool[i].id = i;

            task_pool[i].cpu_context.lr = (uint64_t)func;

            // +1 since stack grows toward lower address
            task_pool[i].cpu_context.fp = (uint64_t)kstack_pool[i + 1];
            task_pool[i].cpu_context.sp = (uint64_t)kstack_pool[i + 1];

            pushQueue(&running_queue, task_pool + i);
            return 0;
        }
    }

    sendStringUART("[ERROR] No more tasks can be created!\n");
    return 1;
}

void fooTask(void) {
    sendStringUART("Hi, I'm ");
    sendHexUART(getCurrentTask()->id);
    sendStringUART("...\n");

    delay(0x3000000u);

    schedule();
}
