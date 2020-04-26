#include "uart.h"
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
    uart_print("Version : 4.0.2\n");
    uart_print("-------------------------\n");
    get_board_revision();
    get_vc_memory();
    /*
    get_frame_buffer();
    showpicture();
    */
}

void bottom_half_0()
{
    // delay, depend on cpu frequency, so in rpi3 B+ and qemu is different
    for (int i = 0; i < 5; i++)
    {
        uart_puts(".");
        // very very very slow in real rpi3, but very very very fast in qemu
        asm volatile(
            "mov  x0, #0xfffff\n"
            "loop_bottom_half_0: subs  x0, x0, #1\n"
            "bne   loop_bottom_half_0\n");
    }
    uart_send('\n');
}

void delay(char c)
{
    for (int i = 0; i < 10000; i++)
    {
        uart_send(c);
        // very very very slow in real rpi3, but very very very fast in qemu
        asm volatile(
            "mov  x0, #0xfffff\n"
            "loop_delay_0: subs  x0, x0, #1\n"
            "bne   loop_delay_0\n");
    }
}

void task_1()
{
    delay('1');
}

void task_2()
{
    /*
    uart_puts("task_2 Current EL: ");
    uart_send_int(get_current_el());
    uart_puts("\n");
    */

    // delay, depend on cpu frequency, so in rpi3 B+ and qemu is different
    delay('2');
}

void user_task_3()
{
    // delay, depend on cpu frequency, so in rpi3 B+ and qemu is different
    delay('3');
}

void task_3()
{
    do_exec((unsigned long)&user_task_3);
    //do_exec((unsigned long)&pcsh);
    if (check_reschedule())
        schedule();
}

int main()
{
    // set uart
    uart_init();

    system_start();

    enable_irq();

    // enroll system call to bottom_half
    bottom_half_t n = {
        0,
        bottom_half_0};
    bottom_half_enroll(n);

    /* You can't know Current EL, if you in EL0 */
    /*
    uart_puts("Current EL: ");
    uart_send_int(get_current_el());
    uart_puts("\n");
    */

    // core timer enable, every 1ms
    //syscall_1(0, 1);

    task_init();
    //privilege_task_create((unsigned long)task_1, (unsigned long)"12345");
    copy_process(PF_KTHREAD, (unsigned long)task_2, (unsigned long)"ccc", 0);
    //privilege_task_create((unsigned long)task_3, (unsigned long)"aaa");
    copy_process(PF_KTHREAD, (unsigned long)task_3, 0, 0);

    //privilege_task_create((unsigned long)&pcsh, (unsigned long)"pcsh");

    core_timer(1);
    //_core_timer_enable();

    schedule();

    return 0;
}
