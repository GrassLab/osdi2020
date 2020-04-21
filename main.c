#include "uart.h"
#include "pcsh.h"
#include "screen.h"
#include "string.h"

#include "syscall.h"
#include "irq.h"
#include "bottom_half.h"

#include "task.h"

#define INPUT_BUFFER_SIZE 1024

void system_start()
{
    uart_print("-------------------------\n");
    uart_print("Raspberry Pi 3B+ is start\n");
    uart_print("-------------------------\n");
    uart_print("Author  : Hsu, Po-Chun\n");
    uart_print("Version : 3.1.0\n");
    uart_print("-------------------------\n");
}

void make_exc()
{
    uart_puts("exception happen!!!!!\n");
    unsigned int r;
    r = *((volatile unsigned int *)0xFFFFFFFFFF000000);

    r++;
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

void task_1()
{
    // delay, depend on cpu frequency, so in rpi3 B+ and qemu is different
    for (int i = 0; i < 10000; i++)
    {
        uart_puts("1...\n");
        // very very very slow in real rpi3, but very very very fast in qemu
        asm volatile(
            "mov  x0, #0xffffff\n"
            "loop_task_1_0: subs  x0, x0, #1\n"
            "bne   loop_task_1_0\n");

        if (check_reschedule())
            schedule();
    }
}

void task_2()
{
    // delay, depend on cpu frequency, so in rpi3 B+ and qemu is different
    for (int i = 0; i < 10000; i++)
    {
        uart_puts("2...\n");
        // very very very slow in real rpi3, but very very very fast in qemu
        asm volatile(
            "mov  x0, #0xffffff\n"
            "loop_task_2_0: subs  x0, x0, #1\n"
            "bne   loop_task_2_0\n");
        if (check_reschedule())
            schedule();
    }
}

void user_task_3()
{
    // delay, depend on cpu frequency, so in rpi3 B+ and qemu is different
    for (int i = 0; i < 10000; i++)
    {
        uart_puts("3...\n");
        // very very very slow in real rpi3, but very very very fast in qemu
        asm volatile(
            "mov  x0, #0xffffff\n"
            "loop_task_3_0: subs  x0, x0, #1\n"
            "bne   loop_task_3_0\n");
    }
}

void task_3()
{
    move_to_user_mode(user_task_3);
    if (check_reschedule())
        schedule();
}

int main()
{
    //_irq_init();

    *ENABLE_IRQ2 = 1 << 25;

    // set uart
    uart_init();

    system_start();

    get_board_revision();
    get_vc_memory();

    get_frame_buffer();
    showpicture();

    // enroll system call to bottom_half
    bottom_half_t n = {
        0,
        bottom_half_0};
    bottom_half_enroll(n);

    unsigned long t;
    asm volatile("mrs %0, CNTFRQ_EL0"
                 : "=r"(t));
    uart_send_int(t);

    /* You can't know Current EL, if you in EL0 */
    /*
    uart_puts("Current EL: ");
    uart_send_int(get_current_el());
    uart_puts("\n");
    */

    // core timer enable, every 1ms
    syscall1(0, 1);

    task_init();
    privilege_task_create((unsigned long)&task_1, (unsigned long)"12345");
    privilege_task_create((unsigned long)&task_2, (unsigned long)"ccc");
    privilege_task_create((unsigned long)&task_3, (unsigned long)"aaa");

    //privilege_task_create((unsigned long)&pcsh, (unsigned long)"pcsh");

    schedule();

    /*
    while (1)
    {
        uart_puts("=Shell Start=\n");
        pcsh();
        uart_puts("=Shell End=\n");
    }
*/

    return 0;
}
