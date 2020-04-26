#include "schedule/schedule.h"
#include "schedule/context.h"
#include "schedule/task.h"

void schedule(void) {
    TaskStruct *next_task = popQueue(&running_queue);

    if (next_task == NULL) {
        // enter idle state
        task_pool[0].counter = 5u;
        switchContext(task_pool + 0u);
        return;
    }

    TaskStruct *cur_task = getCurrentTask();

    // we don't have to put idle task into running_queue
    if (cur_task != task_pool + 0u) {
        pushQueue(&running_queue, cur_task);
    }

    // allocate resource to next task
    next_task->counter = 10u;
    next_task->reschedule_flag = false;
    switchContext(next_task);
}
