#include "uart.h"
#include "mystd.h"
#include "mmu.h"
// #include "exc.h"
// #include "irq.h"
#include "task.h"
// #include "timer.h"

void kernel_main()
{
    uart_init();

    mmu_info_init();


    uart_puts("hello mmu\n");

    create_user_virt_test();

    uart_puts("hello mmu\n");

    while(1);

}
