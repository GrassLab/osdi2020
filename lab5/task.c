#include "task.h"
#include "uart.h"
#include "page.h"

__attribute__((section(".userspace"))) char kstack_pool[64][4096];
__attribute__((section(".userspace"))) char user_pool[64][4096];
//char *kstack_pool;


task task_pool[64];

task_queue runqueue;

void switch_to(task *prev, task *next);

//char *ustack_pool;

unsigned long long _global_coretimer;

int privilege_task_create(void(*func)())
{
    int task_id=0;
    for(int i=1;i<64;i++) //start from 1 (idle), 0 is preserved
    {
        if(task_pool[i].usage == 0)
        {
            task_id = i;
            break;
        }
    }
    task_pool[task_id].id = task_id;
    task_pool[task_id].ksp = (unsigned long long)page_alloc();//kstack_pool+(task_id*4096);
    task_pool[task_id].sp_el0 = (unsigned long long)user_pool[task_id];//kstack_pool+(kernel_stack_size*4096) + (task_id*4096);
    task_pool[task_id].kbase = task_pool[task_id].ksp;
    task_pool[task_id].ubase = (unsigned long long)user_pool[task_id];
    task_pool[task_id].usage = 1;
    task_pool[task_id].fp_lr[1] = (unsigned long long)func;
    task_pool[task_id].start_coretime = _global_coretimer;
    task_pool[task_id].privilege = 1;
    runqueue_push(&task_pool[task_id]);
    //task_pool[task_id].alive = 1;
    return task_id;
}

task *get_current_task()
{
    task *current;
    asm volatile("mrs %0, tpidr_el1":"=r"(current)::);
    return current;
}

void context_switch(task* next) //old
{
    task* prev = get_current_task();
    //asm volatile("mrs %0, tpidr_el1":"=r"(prev)::);
    //asm volatile("mov %0, x30":"=r"(prev->rip)::);
    switch_to(prev, next);
}

void task_struct_init()
{
    //extern void *_end;
	//kstack_pool = (char*)&_end;

    runqueue.head = 0;
    runqueue.tail = 0;
    runqueue.now = 0;
    for(int i=0;i<64;i++){
        task_pool[i].usage =0;
        task_pool[i].alive = 0;
        task_pool[i].reschedule = 0;
        task_pool[i].start_coretime = 0;
    }
}

void runqueue_push(task *input)
{
    runqueue.taskq[runqueue.tail] = input;
    runqueue.tail += 1;
}

void runqueue_del(int id)
{
    for(int i=runqueue.head;i<runqueue.tail;i++)
    {
        if(runqueue.taskq[i]->id == id)
        {
            runqueue.tail--;
            for(int j=i;j<runqueue.tail;j++)
            {
                runqueue.taskq[j] = runqueue.taskq[j+1];
            }
        }
    }
}

void task_schedule()
{
    //uart_hex(oreg);
    //uart_puts("\r\n");
    //note : in here you should get origin sp from sp_el0 or some other way, I store here for test
    //uart_hex(sp);
    //note : switch_to do not store the rip, you need to manual set now task rip to now task struct
    task *next, *current = get_current_task();
    if( (runqueue.now + 1) < runqueue.tail )
    {
        runqueue.now++;
        next = runqueue.taskq[runqueue.now];
        //uart_puts("now+1: ");
        /*uart_hex(next);
        uart_send('\n');*/
    }
    else
    {
        runqueue.now = runqueue.head;
        next = runqueue.taskq[runqueue.head];
        //uart_puts("now else\n");
        /*uart_hex(next);
        uart_send('\n');*/
    }

    if(current != next)
    {
        
        /*if(current->privilege == 0)
        {
            //uart_puts("current privilege 0\n");
            unsigned long long int elr_el1, spsr_el1, sp_el0;
            asm volatile("mrs %0,spsr_el1":"=r"(spsr_el1));
            asm volatile("mrs %0,elr_el1":"=r"(elr_el1));
            asm volatile("mrs %0,sp_el0":"=r"(sp_el0));
            current->elr_el1 = elr_el1;
            current->sp_el0 = sp_el0;
            current->spsr_el1 = spsr_el1;
            uart_puts("current privilege 0: ");
            uart_hex(sp_el0);
            uart_puts("\r\n");
        }*/
        context_switch(next);
        //after context switch
        next->start_coretime = _global_coretimer;
        /*asm volatile("msr spsr_el1, %0"::"r"(next->spsr_el1):);
        asm volatile("msr elr_el1, %0"::"r"(next->elr_el1):);
        asm volatile("msr sp_el0, %0"::"r"(next->sp_el0):);*/
        
        //asm volatile("mov %0, x30":"=r"(tmp)::);
        //uart_hex(tmp);
        //asm volatile("ret");
        //switch_to(current, next);
    }

}


void do_exec(void(*func)(), char signal)
{
	task *current = get_current_task();
    //current->spsr_el1 = 0;
    //current->elr_el1 = func;
    //current->spsr_el1 = 0;
    if(signal == 0)
    {
        asm volatile("msr sp_el0, %0"::"r"(current->sp_el0):);
        asm volatile("msr spsr_el1, %0"::"r"(0):);
        asm volatile("msr elr_el1, %0"::"r"(func):);
        asm volatile("eret");
    }
    else
    {
        current->elr_el1 = (unsigned long long)func;
    }
}

void new_do_exec(unsigned long long begin, unsigned long long size, void(*func)())
{
    task *current = get_current_task();
    current->user_ttbr0 = user_paging();
}


void toggle_privilege()
{
    task *current = get_current_task();
    current->privilege = !current->privilege;
}
