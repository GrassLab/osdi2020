#include "sched.h"
#include "uart.h"
#include "printf.h"
#include "mm.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct *task[NR_TASKS] = {&(init_task)};
unsigned long nr_tasks = 1;

void context_switch(struct task_struct *next) {
    if (current == next)
        return;
    struct task_struct *prev = current;
    current = next;
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
    p->counter = p->priority;
    p->preempt_count = 1; //disable preemtion until schedule_tail

    p->cpu_context.x19 = (unsigned long)func; /* hold the funtion pointer */
    p->cpu_context.x20 = (unsigned long)num;  /* hold the argument */
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