#include "uart.h"
#include "printf.h"
#include "pcsh.h"
#include "screen.h"
#include "string.h"

#include "syscall.h"
#include "irq.h"
#include "bottom_half.h"

#include "exception.h"
#include "task.h"

#define INPUT_BUFFER_SIZE 64

void system_start()
{
    uart_print("-------------------------\n");
    uart_print("Raspberry Pi 3B+ is start\n");
    uart_print("-------------------------\n");
    uart_print("Author  : Hsu, Po-Chun\n");
    uart_print("Version : 4.4.1\n");
    uart_print("-------------------------\n");
    get_board_revision();
    get_vc_memory();
    /*
    get_frame_buffer();
    showpicture();
    */
}

void delay_c(char c)
{
    for (int i = 0; i < 10000; i++)
    {
        uart_send(c);
        // very very very slow in real rpi3, but very very very fast in qemu
        asm volatile(
            "mov  x0, #0xfffff\n"
            "loop_delay_c_0: subs  x0, x0, #1\n"
            "bne   loop_delay_c_0\n");
    }
}

void delay(unsigned long num)
{
    for (unsigned long i = 0; i < num; i++)
    {
        asm volatile("nop");
    }
}

/* User task */
void user_task_3()
{
    // delay_c, depend on cpu frequency, so in rpi3 B+ and qemu is different
    delay_c('3');
}

void user_task_4()
{
    // delay_c, depend on cpu frequency, so in rpi3 B+ and qemu is different
    delay_c('4');
}

void user_syscall_test()
{
    int pid = fork();
    //int pid = 0;
    if (pid == 0)
    {
        printf("\nPID: %d\n", pid);
        exec((unsigned long)user_task_4);
    }
    else
    {
        printf("\nPID: %d\n", pid);
        exec((unsigned long)pcsh);
        exit(0);
    }
}

/* Kernel task */
void task_1()
{
    delay_c('1');
}

void task_2()
{
    do_exec((unsigned long)user_syscall_test);
    if (check_reschedule())
        schedule();
}

void task_3()
{
    do_exec((unsigned long)user_task_3);
    //do_exec((unsigned long)&pcsh);
    if (check_reschedule())
        schedule();
}

void task_4()
{
    while (1)
    {
        bottom_half_router();
        if (check_reschedule())
            schedule();
    }
}

void foo()
{
    int tmp = 5;
    printf("Task %d after exec, tmp address 0x%x, tmp value %d\n", get_taskid(), &tmp, tmp);
    exit(0);
}

void test()
{
    int cnt = 1;
    if (fork() == 0)
    {
        fork();
        delay(10000);
        //fork();
        while (cnt < 10)
        {
            printf("Task id: %d, cnt: %d address: %x\n\r", get_taskid(), cnt, &cnt);
            delay(1000000);
            ++cnt;
        }
        exit(0);
        printf("Should not be printed\n\r");
    }
    else
    {
        printf("Task %d before exec, cnt address 0x%x, cnt value %d\n\r", get_taskid(), &cnt, cnt);
        exec(foo);
    }
}
void user_test()
{
    do_exec((unsigned long)test);
}

int main()
{
    // set uart
    uart_init();
    init_printf(0, putc);

    system_start();

    // enable_irq();

    /* You can't know Current EL, if you in EL0 */
    /*
    uart_puts("Current EL: ");
    uart_send_int(get_current_el());
    uart_puts("\n");
    */

    task_init();
    // copy_process(PF_KTHREAD, (unsigned long)task_1, 0, 0);
    copy_process(PF_KTHREAD, (unsigned long)task_2, 0, 0); // fork, shell
    copy_process(PF_KTHREAD, (unsigned long)task_3, 0, 0);
    //copy_process(PF_KTHREAD, (unsigned long)task_4, 0, 0);
    copy_process(PF_KTHREAD, (unsigned long)user_test, 0, 0);

    // core timer enable, every 1ms
    core_timer(1);
    //_core_timer_enable();

    while (1)
    {
        schedule();
    }

    return 0;
}
