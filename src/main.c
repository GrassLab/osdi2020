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

// void main()
// {
//     uart_irq_enable();
//     uart_init();
//     for (int i=0; i<16; i++)
//         uart_send('a');
//     uart_send('\r');
//     uart_send('\n');
//     while(1);
// }


void main()
{
    // set up serial console and linear frame buffer
    // uart_irq_enable();
    uart_init();
    uart_puts(WELCOME);

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
    
    // shell();

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

    // while(1) {
    //     uart_send(uart_getc());
    // }
    uart_send('>'); // to interactive with raspbootcom 
    char buf[0x100];
    uart_readline(buf);
    // char tmp;
    // tmp = uart_getc();
    uart_puts("ddddddddd\n");
    uart_puts(buf);

    /*
     ** final test
     */
    task_manager_init(final_idle);
    enable_irq();
    core_timer_enable();
    privilege_task_create(final_user_test, 0);
    final_idle();
     
    
    
}

