#include "uart.h"
#include "kernel.h"

struct task_struct task[64];
char kstack_pool[64][4096];
struct run_queue runqueue = {.start = 0, .end = 0, .size = 0};


int kernel_init(){
    uart_init();

    uart_puts("Init Kernel\n");

    task[0].valid = 1;
    task[0].id = 0;
    task[0].kstack_top = kstack_pool[1];
    task[0].x29 = kstack_pool[1];
    task[0].x30 = (unsigned long long)&init;
    //privilege_task_create(init);

    context_switch(&task[0]);

    return 0;
}

int privilege_task_create(void(*func)()){
    static int available_id = 0;
    int i;
    for (i=0; i<64; i++){
        if (!task[available_id%64].valid){
            int id = available_id % 64;     
            task[id].valid = 1;
            task[id].id = id;
            task[id].kstack_top = kstack_pool[id+1];
            task[id].x29 = kstack_pool[id+1];
            task[id].x30 = (unsigned long long)func;

            available_id++;
            /*uart_puts("Task Created, ID = ");
            char res[4];
            itoa(id, res);
            uart_puts(res);
            uart_puts("\n");*/
            if (runqueue.size == QUEUE_SIZE){
                uart_puts("Runqueue is FULL!\n");
                break;
            }
            runqueue.queue[runqueue.end % QUEUE_SIZE] = &task[id];
            runqueue.end++;
            runqueue.size++;
            break;
        }
        available_id++;
    }

    if (i == 64){
        uart_puts("There are no available process IDs\n");
        return 1;
    }

    return 0;
}

void init(){
    unsigned long long ptr = (unsigned long long) &task[0];
    asm volatile("msr tpidr_el1, %0" : "=r"(ptr));

    privilege_task_create(test);
    privilege_task_create(test);
    privilege_task_create(test);

    schedule();
}

void context_switch(struct task_struct *next){
    struct task_struct *prev = get_current();
    switch_to(prev, next);
}

void test(){
    struct task_struct *now = get_current();
    char res[4];
    while(1){
        itoa(now->id, res);
        uart_puts("ID = ");
        uart_puts(res);
        uart_puts("\n");
        for (int i=0; i<100000000; i++) asm volatile("nop");
        schedule();
    }
}

int schedule(){
    if (!runqueue.size){
        uart_puts("Runqueue is EMPTY!\n");
        return 1;
    }
    struct task_struct *next = runqueue.queue[runqueue.start % QUEUE_SIZE];
    runqueue.size--;
    runqueue.queue[runqueue.start % QUEUE_SIZE] = 0;
    runqueue.start++;

    // test
    runqueue.queue[runqueue.end % QUEUE_SIZE] = next;
    runqueue.end++;
    runqueue.size++;

    context_switch(next);

    return 0;
}
