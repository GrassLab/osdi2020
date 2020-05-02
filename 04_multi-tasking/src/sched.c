#include "sched.h"

static struct task_struct init_task = INIT_TASK;

tid_t acquire_unused_task_id() {
    return ++ used_task_id;
}
