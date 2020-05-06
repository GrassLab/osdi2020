#include "sched.h"
#include "entry.h"
#include "mm.h"
#include "uart.h"
static struct task init_task = INIT_TASK;
struct task *current = &(init_task);
struct task *task_pool[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;
int exist = 1;

void preempt_disable(void)
{
    current->preempt_count++;
}

void preempt_enable(void)
{
    current->preempt_count--;
}



void context_switch(struct task* next){
    if (current == next) 
        return;
    struct task* prev = current;
    current = next;
    switch_to(prev, next);
}

void _schedule(){
    preempt_disable();
    int next,c;
    struct task * p;
    while (1) {
        c = -1;
        next = 0;
        for (int i = 0; i < NR_TASKS; i++){
            p = task_pool[i];
            if (p && p->state == TASK_RUNNING && p->counter > c) {
                c = p->counter;
                next = i;
            }
        }
        if (c) {
            break;
        }
        for (int i = 0; i < NR_TASKS; i++) {
            p = task_pool[i];
            if (p) {
                p->counter = (p->counter >> 1) + p->priority;
            }
        }
    }
    uart_puts("Context switch to ");
    uart_hex(next);
    uart_puts("\n");
    context_switch(task_pool[next]);
    preempt_enable();
}

void Schedule(){
    current->counter = 0;
    _schedule();
}

void timer_tick(){
    
    --current->counter;
    if (current->counter > 0 || current->preempt_count > 0) {
        return;
    }
    current->counter = 0;
    set_tsk_need_resched();
    uart_hex(current->id);
    uart_puts(" need_resched flag is set. ");
    enable_irq();
    _schedule();
    disable_irq();

}

/*
*keepping the kernel stack and task struct. 
*set its state to be zombie state and won't be scheduled again.
*/
void exit_process(){
    preempt_disable();
    for (int i = 0; i < NR_TASKS; i++){
        if (task_pool[i] == current) {
            task_pool[i]->state = TASK_ZOMBIE;
            exist -= 1;
            break;
        }
    }

    if (current->stack) {
        free_page(current->stack);
    }

    preempt_enable();
    Schedule();
}


void set_tsk_need_resched(){
    current->need_resched = 1;
}

void clear_tsk_need_resched(){
     current->need_resched = 0;
}

int need_resched(){
    if(current->need_resched){
        return 1;
    }else{
        return 0;
    }
}

