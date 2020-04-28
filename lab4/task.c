#include "task.h"
#include "uart.h"


//__attribute__((section(".userspace"))) char kstack_pool[64][4096];
char *kstack_pool;


task task_pool[64];

task_queue runqueue;

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
    task_pool[task_id].ksp = kstack_pool+(task_id*4096);
    task_pool[task_id].usp = kstack_pool+(10*4096) + (task_id*4096);
    task_pool[task_id].usage = 1;
    task_pool[task_id].rip = func;
    task_pool[task_id].start_coretime = _global_coretimer;
    task_pool[task_id].privilege = 1;
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
    extern void *_end;
	kstack_pool = (char*)&_end;

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

void task_schedule(unsigned long long rip, unsigned long long sp, unsigned long long *oreg)
{
    unsigned long long x19_x28[10], fp, lr;
    asm volatile("mov %0, x19":"=r"(x19_x28[0])::);
    asm volatile("mov %0, x20":"=r"(x19_x28[1])::);
    asm volatile("mov %0, x20":"=r"(x19_x28[2])::);
    asm volatile("mov %0, x22":"=r"(x19_x28[3])::);
    asm volatile("mov %0, x23":"=r"(x19_x28[4])::);
    asm volatile("mov %0, x24":"=r"(x19_x28[5])::);
    asm volatile("mov %0, x25":"=r"(x19_x28[6])::);
    asm volatile("mov %0, x26":"=r"(x19_x28[7])::);
    asm volatile("mov %0, x27":"=r"(x19_x28[8])::);
    asm volatile("mov %0, x28":"=r"(x19_x28[9])::);
    asm volatile("mov %0, fp":"=r"(fp)::);
    asm volatile("mov %0, lr":"=r"(lr)::);
    //uart_hex(oreg);
    //uart_puts("\r\n");
    //note : in here you should get origin sp from sp_el0 or some other way, I store here for test
    //uart_hex(sp);
    //note : switch_to do not store the rip, you need to manual set now task rip to now task struct
    task *current, *next;
    current = get_current_task();

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
        if(current->privilege == 1)
        {
            //uart_puts("current privilege 1\n");
            current->rip = rip;
            current->ksp = sp;
            for(int i=0;i<10;i++)
            {
                current->x19_x28[i] = x19_x28[i];
                current->fp_lr[0] = fp;
                current->fp_lr[1] = lr;
            }
        }
        else if(current->privilege == 0)
        {
            //uart_puts("current privilege 0\n");
            unsigned long long int elr_el1, spsr_el1, sp_el0;
            asm volatile("mrs %0,spsr_el1":"=r"(spsr_el1));
            asm volatile("mrs %0,elr_el1":"=r"(elr_el1));
            asm volatile("mrs %0,sp_el0":"=r"(sp_el0));
            current->rip = elr_el1;
            current->usp = sp_el0;
            current->spsr_el1 = spsr_el1;
            for(int i=0;i<31;i++)
            {
                //uart_hex(oreg[i]);
                //uart_puts("\r\n");
                if(i < 19 && i>=0)
                {   current->x0_x18[i] = oreg[i];}
                else if(i < 29 && i >= 19)
                {   current->x19_x28[i-19] = oreg[i];}
                else if(i >= 29 && i <= 30)
                {   current->x29_x30[i-29] = oreg[i];}
            }
        }
        else{uart_puts("schedule fail (in current)");}

        next->start_coretime = _global_coretimer;
        
        if(next->privilege == 1){
            //uart_puts("next privilege 1\n");
            asm volatile("mov x10, %0"::"r"(next):);
            asm volatile(
                "ldp x19, x20, [x10, 16 * 0]\n"
                "ldp x21, x22, [x10, 16 * 1]\n"
                "ldp x23, x24, [x10, 16 * 2]\n"
                "ldp x25, x26, [x10, 16 * 3]\n"
                "ldp x27, x28, [x10, 16 * 4]\n"
                "ldp fp, lr, [x10, 16 * 5]\n"
                "ldp x9, x30, [x10, 16 * 6]\n"
                "mov sp,  x9\n"
                "msr tpidr_el1, x10\n"
                "ret"
            );
        }
        else if(next->privilege == 0)
        {
            //uart_puts("next privilege 0\n");
            asm volatile("msr spsr_el1, %0"::"r"(next->spsr_el1):);
            asm volatile("msr elr_el1, %0"::"r"(next->rip):);
            asm volatile("msr sp_el0, %0"::"r"(next->usp):);
            asm volatile(
                "ldp x0, x1, [%0 ,16 * 0]\n"
                "ldp x2, x3, [%0 ,16 * 1]\n"
                "ldp x4, x5, [%0 ,16 * 2]\n"
                "ldp x6, x7, [%0 ,16 * 3]\n"
                "ldp x8, x9, [%0 ,16 * 4]\n"
                "ldp x10, x11, [%0 ,16 * 5]\n"
                "ldp x12, x13, [%0 ,16 * 6]\n"
                "ldp x14, x15, [%0 ,16 * 7]\n"
                "ldp x16, x17, [%0 ,16 * 8]\n"
                "ldr x18, [%0 ,16 * 9]\n"
                ::"r"(next->x0_x18):
            );
            asm volatile(
                "ldp x19, x20, [%0, 16 * 0]\n"
                "ldp x21, x22, [%0, 16 * 1]\n"
                "ldp x23, x24, [%0, 16 * 2]\n"
                "ldp x25, x26, [%0, 16 * 3]\n"
                "ldp x27, x28, [%0, 16 * 4]\n"
                ::"r"(next->x19_x28):
            );
            asm volatile(
                "ldp x29, x30, [%0, 16 * 0]"
                ::"r"(next->x29_x30):
            );
            asm volatile("eret");
        }
        else{uart_puts("schedule fail (in next)");}
        
        //asm volatile("mov %0, x30":"=r"(tmp)::);
        //uart_hex(tmp);
        //asm volatile("ret");
        //switch_to(current, next);
    }

}



task *get_current_task()
{
    task *current;
    asm volatile("mrs %0, tpidr_el1":"=r"(current)::);
    return current;
}

void toggle_privilege()
{
    task *current = get_current_task();
    current->privilege = !current->privilege;
}
