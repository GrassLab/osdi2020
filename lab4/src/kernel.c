#include "uart.h"
#include "kernel.h"

struct task_struct task[64] __attribute__((aligned(16u)));
char kstack_pool[64][4096] __attribute__((aligned(16u)));
struct run_queue runqueue = {.start = 0, .end = 0, .size = 0};

extern int reschedule;

int kernel_init(){
    uart_init();

    uart_puts("Init Kernel...\n");

    task[0].valid = 1;
    task[0].id = 0;
    task[0].kstack_top = kstack_pool[1];
    task[0].x29 = kstack_pool[1];
    task[0].x30 = (unsigned long long)&init;
    //unsigned long long ptr = (unsigned long long) &task[0];
    //asm volatile("msr tpidr_el1, %0" : "=r"(ptr));
    //privilege_task_create(init);

    //context_switch(&task[0]);
    init();

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


    enable_sys_timer();
    asm ("msr DAIFClr, 0xf");
    schedule();
}

void context_switch(struct task_struct *next){
    struct task_struct *prev = get_current();
    switch_to(prev, next);
}

void test(){
    struct task_struct *now = get_current();
    char res[4];
    itoa(now->id, res);
    while(1){
        //for (int i=0; i<100000000; i++) asm volatile("nop");
        if (reschedule == 1){
            uart_puts("ID = ");
            uart_puts(res);
            uart_puts(" is schedule out\n");
            reschedule = 0;
            schedule();
        }
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

void enable_sys_timer(){
    uart_puts("\n");
    uart_puts("Enable system timer.\n");

    asm(
        "mov x0, 1;"
        "msr cntp_ctl_el0, x0;"
        "mrs x0, cntfrq_el0;"
        "msr cntp_tval_el0, x0;"
        "mov x0, 2;"
        "ldr x1, =0x40000040;"
        "str x0, [x1];"
    );
    *CORE0_TIMER_IRQ_CTRL = 2;
}
