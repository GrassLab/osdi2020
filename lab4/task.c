#include "task.h"
#include "uart.h"


//__attribute__((section(".userspace"))) char kstack_pool[64][4096];
char *kstack_pool;
task task_pool[64];
task_queue runqueue;
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
    task_pool[task_id].sp = kstack_pool+(task_id*8192);
    task_pool[task_id].usage = 1;
    task_pool[task_id].rip = func;
    task_pool[task_id].start_coretime = _global_coretimer;
    runqueue_push(&task_pool[task_id]);
    //task_pool[task_id].alive = 1;
    return task_id;
}

void context_switch(task* next) //old
{
    task* prev;
    asm volatile("mrs %0, tpidr_el1":"=r"(prev)::);
    asm volatile("mov %0, x30":"=r"(prev->rip)::);
    switch_to(prev, next);
}

void task_struct_init()
{
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

void task_schedule(unsigned long long rip)
{
    //note : in here you should get origin sp from sp_el0 or some other way, I store here for test
    unsigned long long int sp;
    asm volatile("mov %0, sp":"=r"(sp)::);
    //uart_hex(sp);
    //note : switch_to do not store the rip, you need to manual set now task rip to now task struct
    task *current, *next;

    asm volatile("mrs %0, tpidr_el1":"=r"(current)::);
    

    if(rip == 0)
    {
        
        asm volatile("mov %0, x30":"=r"(rip)::);

        //uart_puts("rip 0: ");
        /*uart_hex(rip);
        uart_send('\n');*/
    }
    if(current == 0)
    {
        
        //uart_puts("now task 0\n");
        current = &task_pool[1];
        next = runqueue.taskq[1];
    }
    else if( (runqueue.now + 1) < runqueue.tail )
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
        //uart_puts("current!=next\n");
        /*uart_puts("note: ");
        uart_hex(current->rip);
        uart_puts("\r\n");
        uart_hex(next->rip);
        uart_puts("\r\n");*/

        next->start_coretime = _global_coretimer;
        current->rip = rip;
        asm volatile("mov sp, %0"::"r"(sp-16):);

        asm volatile(
            "stp x19, x20, [%0, 16 * 0]\n"
            "stp x21, x22, [%0, 16 * 1]\n"
            "stp x23, x24, [%0, 16 * 2]\n"
            "stp x25, x26, [%0, 16 * 3]\n"
            "stp x27, x28, [%0, 16 * 4]\n"
            "stp fp, lr, [%0, 16 * 5]\n"
            "mov x9, sp\n"
            "str x9, [%0, 16 * 6]\n"
            ::"r"(current):
        );

        asm volatile(
            "ldp x19, x20, [%0, 16 * 0]\n"
            "ldp x21, x22, [%0, 16 * 1]\n"
            "ldp x23, x24, [%0, 16 * 2]\n"
            "ldp x25, x26, [%0, 16 * 3]\n"
            "ldp x27, x28, [%0, 16 * 4]\n"
            "ldp fp, lr, [%0, 16 * 5]\n"
            "ldp x9, x30, [%0, 16 * 6]\n"
            "mov sp,  x9\n"
            "msr tpidr_el1, %0\n"
            "ret"
            ::"r"(next):
        );
        //asm volatile("mov %0, x30":"=r"(tmp)::);
        //uart_hex(tmp);
        //asm volatile("ret");
        //switch_to(current, next);
    }
}

