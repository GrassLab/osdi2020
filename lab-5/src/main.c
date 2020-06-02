#include "uart.h"
#include "string.h"
#include "utility.h"
#include "mailbox.h"
#include "time.h"
#include "command.h"
#include "task.h"
#include "printf.h"

void kernel_process(){
    // unsigned long begin = (unsigned long)&_binary_user_img_start;
    // unsigned long end = (unsigned long)&_binary_user_img_end;
    // do_exec(begin, end - begin, 0x1000);
    printf("after do_exec\n"); // should not be here
    while(1);
}

int main()
{
    uart_init();
    init_printf(0, uart_puts);
    paging_init();
    uart_puts("Hello\n");
    // printPowerOnMessage();
    // task_manager_init(idle);

    // privilege_task_create(kernel_process);

    // for(int i = 0; i < 4; ++i) {
    //     privilege_task_create(req12_test);
    // }

    // privilege_task_create(req34_test);
    // __enable_irq();
    // __core_timer_init();

    // idle12();
    idle();
    while(1);
}
