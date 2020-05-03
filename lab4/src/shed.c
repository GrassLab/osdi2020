#include "shed.h"
#include "uart.h"
#include "mm.h"
#include "entry.h"
#include "irq.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct *task[NR_TASKS] = {&(init_task), };

int n_tasks = 1;
int n_task_id = 1;

void enable_preempt() {
    current->preempt_count = 1;
}

void disable_preempt() {
    current->preempt_count = 0;
}

void current_task_info() {
    while(1) {
        uart_puts("Task id: ");
        uart_print_int(current->task_id);
        uart_puts("\r\n");
        delay(100000);
    }
}

void privilege_task_create(void (*func)()) {
    disable_preempt();
    struct task_struct *new_task = (struct task_struct *) get_free_page();
    if(!new_task) {
        uart_puts("Fail to create a new task...\r\n");
        return;
    }

    new_task->task_id = n_task_id;
    new_task->counter = 5;
    new_task->state = TASK_RUNNING;
    new_task->preempt_count = 1;
    new_task->cpu_context.x19 = (unsigned long) func;
    new_task->cpu_context.pc = (unsigned long) ret_from_fork;
    new_task->cpu_context.sp = (unsigned long) new_task + THREAD_SIZE;
    new_task->parent_id = current->task_id;
    n_task_id++;

    task[n_tasks] = new_task;
    n_tasks %= NR_TASKS;
    n_tasks++;

    // print created message
    uart_puts("Create new task: ");
    uart_print_int(new_task->task_id);
    uart_puts("\r\n");
    enable_preempt();
    return;
}

void switch_to(struct task_struct * next)  {
	if (current == next) 
		return;
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next);
}

void _schedule() {
    disable_preempt();
    int next, c;
    struct task_struct *p;

    c = 0;
    next = 0;

    for(int i=1; i<NR_TASKS; i++) {
        p = task[i];
        if(p && p->state == TASK_RUNNING && p->counter > c) {
            c = p->counter;
            next = i;
        }
    }
    // if(c) {
    //     break;
    // }
    // for(int i=0; i<NR_TASKS; i++) {
    //     p = task[i];
    //     if(p) {
    //         p->counter = 3;
    //     }
    // }
    switch_to(task[next]);
    enable_preempt();
}

void schedule() {
    // current->counter = 0;
    // _schedule();
    // timer_tick();
}

void schedule_tail() {
    enable_preempt();
}

struct task_struct *get_current_task() {
    return current;
}

void timer_tick() {
    // uart_print_int(current->counter);
    // uart_puts("\r\n");
	current->counter--;
	if (current->counter > 0 || current->preempt_count  == 0) {
		return;
	}
	current->counter=0;
	enable_irq();
    uart_puts("Rescheduling...\r\n");
	_schedule();
	disable_irq();
}
