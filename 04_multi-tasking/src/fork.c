#include "fork.h"

tid_t privilege_task_create(unsigned long func) {
    tid_t tid = acquire_unused_task_id();
    struct task_struct *p = task_pool + tid;
    p -> task_id = tid;
    p -> state = TASK_RUNNING;
    p -> counter = 0;
    p -> priority = 1;

    void (*foo)();
    foo = func;
    foo();
    return tid;
}
