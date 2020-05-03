#include "fork.h"
#include "sched.h"
#include "entry.h"
#include "mm.h"

int privilege_task_create(unsigned long func_ptr, unsigned long arg) {
    preempt_disable(); 
    struct task_struct *t;

    /* task struct place at top of the allocate memory */
    t = (struct task_struct *) allocate_a_page(); 
    if (!t) 
        return 1;

    t->task_id = num_tasks;
    t->priority = 5;
    t->state = TASK_RUNNING;
    t->counter = 5;
    t->preempt_count = 1; /* TODO: when to enable */
    
    t->cpu_context.x19 = func_ptr;
    t->cpu_context.x20 = arg;
    t->cpu_context.pc = (unsigned long)ret_from_fork;
    t->cpu_context.sp = (unsigned long)t + PAGE_SIZE;

    /* TODO: A simple implementation, 
    which will cause error if create more than 64 task */
    int pid = num_tasks ++;
    tasks[pid] = t;
    preempt_enable();
    return 0;
}
