#include "syscall.h"
#include "bootloader.h"
#include "timer.h"
#include "bottom_half.h"

void svc(int num)
{
    //asm volatile("svc %0" :"=r"(num));
    asm volatile("svc #1");
}

void brk(int num)
{
    //asm volatile("svc %0" :"=r"(num));
    asm volatile("brk #1");
}

void syscall_core_timer(int enable)
{
    uart_puts("core timer: ");
    if (enable == 0)
    {
        uart_puts("core timer disable");
        _core_timer_disable();
    }
    else
    {
        uart_puts("core timer enable");
        _core_timer_enable();
    }
}

void syscall_local_timer(int enable)
{
    uart_puts("local timer: ");
    if (enable == 0)
    {
        uart_puts("local timer disable");
        local_timer_disable();
    }
    else
    {
        uart_puts("local timer enable");
        local_timer_enable();
    }
}

void syscall_gettime(double *t)
{
    uart_puts("get time: ");
    register unsigned long freq;
    register unsigned long ct;

    asm volatile("mrs %0, CNTFRQ_EL0\n"
                 "mrs %1, CNTPCT_EL0\n"
                 : "=r"(freq), "=r"(ct));
    *t = (double)ct / (double)freq;
}

/* Can't work */
void syscall_load_images()
{
    loadimg();
}

void syscall_delay()
{
    uart_puts("syscall_delay");
    bottom_half_set(0x0);
}

void syscall_delay_without_bottom_half()
{
    uart_puts("syscall_delay_without_bottom_half");
    for (int i = 0; i < 5; i++)
    {
        uart_puts(".");
        // very very very slow in real rpi3, but very very very fast in qemu
        asm volatile(
            "mov  x0, #0xfffff\n"
            "bottom_half_0_l: subs  x0, x0, #1\n"
            "bne   bottom_half_0_l\n");
    }
    uart_send('\n');
}

void syscall_router(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3)
{
    switch (x0)
    {
    // arm core timer
    case 0x0:
        syscall_core_timer(x1);
        break;
    // arm local timer
    case 0x1:
        syscall_local_timer(x1);
        break;
    // get time
    case 0x2:
        syscall_gettime((double *)x1);
        break;
    case 0x3:
        syscall_load_images();
        break;
    // delay than print ...  (for test bottom half)
    case 0x100:
        syscall_delay();
        break;
    // delay than print ...  (for test bottom half)
    case 0x101:
        syscall_delay_without_bottom_half();
        break;
    // not this syscall
    default:
        uart_puts("Can find this system call");

        while (1)
            ;
    }
};