#include "sched.h"
#include "uart.h"
#include "printf.h"
#include "mm.h"
#include "irq.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct *task[NR_TASKS] = {&(init_task)};
unsigned long nr_tasks = 1;

void context_switch(struct task_struct *next) {
    if (current == next)
        return;
    struct task_struct *prev = current;
    current = next;
    printf("context switch to %d\n",next->pid);
    cpu_switch_to(prev, next);
}

void preempt_disable(void) {
	current->preempt_count++;
}

void preempt_enable(void) {
	current->preempt_count--;
}

struct task_struct *privilege_task_create(void (*func)(), unsigned long num) {
    preempt_disable();

    struct task_struct *p;
    p = (struct task_struct *) get_free_page();
    if (!p)
        return p;  // or 1
    
    p->priority = current->priority;
    p->state = TASK_RUNNING;
    p->counter = p->priority; //p->priority
    p->preempt_count = 1; //disable preemtion until schedule_tail
    p->need_reched = 0;

    p->cpu_context.x19 = (unsigned long)func; // hold the funtion pointer
    p->cpu_context.x20 = (unsigned long)num;  // hold the argument
    p->cpu_context.pc = (unsigned long)ret_from_fork;
    p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;

    p->pid = nr_tasks++; // create a unique pid
    task[p->pid] = p;

    printf("Create a privilage task %x\n", p);
    printf("pid:      %d\n", p->pid);
    printf("priority: %d\n", p->priority);
    printf("counter:  %d\n", p->counter);

    preempt_enable();
    return p; // or 0
}

void schedule_tail(void) {
    preempt_enable();
}

void _schedule() {
    preempt_disable();

    int next = 0;
    int c = 0;

    struct task_struct *p;

    while (1) {
        // find out the first task which counter!=0
        for (int i = 0; i < NR_TASKS; ++i) {
            p = task[i];
            if (p && p->state == TASK_RUNNING && p->counter > c) {
                c = p->counter;
                next = i;
            }
        }
        if (c) {
            break;
        }
        // re-assign a counter for each task in task[]
        for (int i = 0; i < NR_TASKS; ++i) {
            p = task[i];
            if (p) {
                p->counter = (p->counter >> 1) + p->priority;
            }
        }
    }

    context_switch(task[next]);
    preempt_enable();
}

void schedule() {
    struct task_struct *p = current;
    if (p->pid) {
        if (p->need_reched && p->preempt_count <= 0) {
            p->need_reched = 0;
            p->counter = 0;

            _schedule();
        }
    } else {
        _schedule();
    }
}

void timer_tick() {
    struct task_struct *p = current;
    --current->counter;
    if (current->counter > 0 || current->preempt_count > 0) {
        return;
    }
    current->need_reched = 1;
    current->counter = 0;
}
