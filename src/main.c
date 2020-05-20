#include "../include/uart.h"
#include "../include/power.h"
#include "../include/time.h"
#include "../include/info.h"
#include "../include/lfb.h"
#include "../include/loadimg.h"
#include "../include/cmdline.h"
#include "../include/interrupt.h"
#include "../include/shell.h"
#include "../include/task.h"
#include "../include/mm.h"
#include "../include/page.h"
#include "../include/user.h"

#define WELCOME \
    "                                                 \n" \
    "   ____   _____ _____ _____ ___   ___ ___   ___  \n" \
    "  / __ \\ / ____|  __ \\_   _|__ \\ / _ \\__ \\ / _ \\ \n" \
    " | |  | | (___ | |  | || |    ) | | | | ) | | | |\n" \
    " | |  | |\\___ \\| |  | || |   / /| | | |/ /| | | |\n" \
    " | |__| |____) | |__| || |_ / /_| |_| / /_| |_| |\n" \
    "  \\____/|_____/|_____/_____|____|\\___/____|\\___/ \n" \
    "                                                 \n"

#define SPLASH_ON 0
#define HW_INFO_ON 0

void user_process() {
    call_sys_uart_write("it is user_process\n");
    while(1);
}

void kernel_process(){
    // unsigned long begin = (unsigned long)&user_begin;
    unsigned long begin = (unsigned long)&_binary_user_img_start;
    // unsigned long end = (unsigned long)&user_end;
    unsigned long end = (unsigned long)&_binary_user_img_end;
    // unsigned long process = (unsigned long)&user_process;
    // int err = do_exec(begin, end - begin, process - begin);
    // if (err < 0){
    //     printf("Error while moving process to user mode\n\r");
    // }
    // printf("before do_exec\n");
    // do_exec(begin, end - begin, user_process-begin);
    do_exec(begin, end - begin, 0x1000);
    printf("after do_exec\n"); // should not be here
    while(1);
}
void main()
{
    // set up serial console and linear frame buffer
    // uart_irq_enable();
    uart_init();
    printf(WELCOME);
    // while(1);

    // display a pixmap
    # if SPLASH_ON
        lfb_init();
        lfb_showpicture();
    # endif

    // get hardware info
    # if HW_INFO_ON
        show_serial();
        show_board_revision();
        show_vccore_addr();
    # endif

    unsigned long ttbr1_el1;
    unsigned long ttbr0_el1;
    asm volatile ("mrs %0, ttbr1_el1" : "=r"(ttbr1_el1));
    asm volatile ("mrs %0, ttbr0_el1" : "=r"(ttbr0_el1));
    printf("ttbr0_el1: %x\n", ttbr0_el1);
    printf("ttbr1_el1: %x\n", ttbr1_el1);
    
    init_page();
    task_manager_init(idle);
    privilege_task_create(kernel_process, 0);
    // privilege_task_create(user_test, 0);
    // privilege_task_create(user_test, 0);
    idle();

    /*
     ** el1 task
     */
    // task_manager_init(idle);
    // core_timer_enable();
    // for(int i = 0; i < N; ++i) { // N should > 2
    //     privilege_task_create(kernel_test, 0);
    // }
    // idle();

    /*
     ** el0 task
     */
    // task_manager_init(idle);
    // enable_irq();
    // core_timer_enable();
    // for(int i = 0; i < N; ++i) { // N should > 2
    //     privilege_task_create(user_test, 0);
    // }
    // idle();


    /*
     ** final test
     */
    // task_manager_init(final_idle);
    // enable_irq();
    // core_timer_enable();
    // privilege_task_create(final_user_test, 0);
    // final_idle();
    
    /*tp
     ** test uart_readline 
     */
    // uart_send('>'); // to interactive with raspbootcom 
    // char buf[0x100];
    // uart_readline(buf);
    // uart_puts("ddddddddd\n");
    // uart_puts(buf);
    
    while(1);
}

