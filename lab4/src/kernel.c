#include "uart.h"
#include "kernel.h"
#include "my_string.h"
#include "sys_call.h"

struct task_struct task[64] __attribute__((aligned(16u)));
char kstack_pool[64][4096] __attribute__((aligned(16u)));
char ustack_pool[64][4096] __attribute__((aligned(16u)));
struct run_queue runqueue = {.start = 0, .end = 0, .size = 0};

int reschedule = 0;

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
            return id;
        }
        available_id++;
    }

    if (i == 64){
        uart_puts("There are no available process IDs\n");
        return -2;
    }

    return -1;
}

int do_exec(void(*func)()){
    struct task_struct *now = get_current();
    now->ustack_top = ustack_pool[now->id+1];
    el1_to_el0(func, (unsigned long long)now->ustack_top);
}

int do_fork(){
    struct task_struct *now = get_current();
    int child_id = privilege_task_create(0);
    task[child_id].x19 = task[now->id].x19;
    task[child_id].x20 = task[now->id].x20;
    task[child_id].x21 = task[now->id].x21;
    task[child_id].x22 = task[now->id].x22;
    task[child_id].x23 = task[now->id].x23;
    task[child_id].x24 = task[now->id].x24;
    task[child_id].x25 = task[now->id].x25;
    task[child_id].x26 = task[now->id].x26;
    task[child_id].x27 = task[now->id].x27;
    task[child_id].x28 = task[now->id].x28;
    task[child_id].x29 = task[now->id].x29;
    task[child_id].x30 = task[now->id].x30;
    task[child_id].ustack_top = ustack_pool[child_id+1];
    for (int i=0; i<4096; i++){
        kstack_pool[child_id][i] = kstack_pool[now->id][i];
        ustack_pool[child_id][i] = ustack_pool[now->id][i];
    }

    return child_id;
}

void init(){
    unsigned long long ptr = (unsigned long long) &task[0];
    asm volatile("msr tpidr_el1, %0" : "=r"(ptr));

    privilege_task_create(test_user);
    privilege_task_create(test_user2);
    privilege_task_create(test_user3);


    enable_sys_timer();
    //asm ("msr DAIFClr, 0xf");
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
        //if (reschedule == 1){
            //uart_puts("ID = ");
            //uart_puts(res);
            //uart_puts(" is schedule out\n");
            //reschedule = 0;
            //schedule();
        //}
    }
}

void test_user(){
    do_exec(user);
}

void test_user2(){
    do_exec(user2);
}

void test_user3(){
    do_exec(user3);
}

void user4(){
    while(1){
        uart_write("user task 4444!!!\n");
    }
}

void user(){
    while(1){
        char c[5];
        c[1] = '\0';
        c[0] = uart_read();
        uart_write("user task 1!");
        uart_write("  My word = ");
        uart_write(c);
        uart_write("\n");
    }
    //exec(user4);
}

void user2(){
    /*int pid;
    pid = fork();
    while(1){
        if (pid == 0)
            uart_write("user task 4!!!\n");
        else
            uart_write("user task 2!!!\n");
    }*/
    while(1){
        //char c[5];
        //c[1] = '\0';
        //c[0] = uart_read();
        uart_write("User task 2!\n");
        for (int i=0; i<10000000; i++);
        //uart_write("  My word = ");
        //uart_write(c);
        //uart_write("\n");
    }
}

void user3(){
    while(1){
        char c[5];
        c[1] = '\0';
        c[0] = uart_read();
        uart_write("user task 3!");
        uart_write("  My word = ");
        uart_write(c);
        uart_write("\n");
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

void check_resched_flag(){
    if (reschedule == 1){
        reschedule = 0;
        schedule();
    }
}
