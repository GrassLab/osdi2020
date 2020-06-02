#include "peripherals/uart.h"
#include "peripherals/irq.h"
#include "entry.h"
#include "utils.h"
#include "timer.h"
#include "fork.h"
#include "sys.h"
#include "mm.h"

const unsigned int delay_interval = 100000000;

void foo() {
    while(1) {
        uart_puts("Task ID: ");
        uart_send_ulong(current -> task_id);
        uart_puts("; task_struct ptr: ");
        uart_send_hex((unsigned long)current);
        uart_puts("; sp: ");
        uart_send_hex(current -> cpu_context.sp);
        uart_send('\n');
        delay(delay_interval);
        schedule();
    }
}

void test() {
    int cnt = 1;
    if (fork() == 0) {
        fork();
        delay(delay_interval);
        fork();
        while (cnt < 10) {
            uart_puts("Task id: ");
            uart_send_ulong(current->task_id);
            uart_puts(", cnt: ");
            uart_send_ulong(cnt);
            uart_send('\n');
            delay(delay_interval);
            ++ cnt;
        }
        exit(0);
    } else {
        while (1) {
            delay(delay_interval);
        }
    }
}

void create_user_process() {
    int ret = do_exec((unsigned long)&test);
    if (ret) {
        uart_send_ulong(ret);
        uart_puts("Create user process failed.\n");
    }
}

void idle() {
    while(1){
        schedule();
        delay(delay_interval);
    }
}

void zombie_reaper(){
    while(1){
        delay(10000);
        struct task_struct *t;
        for (int i = 0; i < NR_TASKS; ++i) {
            t = tasks[i];
            if(t && t->state == TASK_ZOMBIE){
                free_task_struct((unsigned long)t);
                tasks[i] = 0;
            }
        }
    }
}

void demo_memory() {
    show_buddy_system();
    void* addr = get_free_pages(PAGE_AVAILABLE, 6);
    show_buddy_system();
    put_free_pages(addr, 6);
    show_buddy_system();
}

void start_kernel() {
    uart_init();
    el1_vector_init();
    enable_core_timer();
    enable_irq();
    init_page_map();

    uart_puts("Welcome to MiniKernel 0.0.4\n");
    int ret;

    demo_memory();

    for (int i = 0; i < 2; ++ i) {
        ret = __clone((unsigned long)&foo, PF_KTHREAD, 0);
        if (ret == -1) {
            uart_puts("Error when creating privilege task ");
            uart_send_ulong(i);
            uart_send('\n');
        }
    }

    ret = __clone((unsigned long)&zombie_reaper, PF_KTHREAD, 0);
    ret = __clone((unsigned long)&create_user_process, PF_KTHREAD, 0);
    
    idle();
}
