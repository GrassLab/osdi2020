#include "syscall.h"
#include "bootloader.h"
#include "timer.h"
#include "bottom_half.h"
#include "task.h"
#include "printf.h"
#include "debug.h"

void syscall_core_timer(int enable)
{
    DEBUG_LOG_SYSCALL(("core timer: "));
    if (enable == 0)
    {
        DEBUG_LOG_SYSCALL(("core timer disable"));
        _core_timer_disable();
    }
    else
    {
        DEBUG_LOG_SYSCALL(("core timer enable"));
        _core_timer_enable();
    }
    DEBUG_LOG_SYSCALL(("\n"));
}

void syscall_local_timer(int enable)
{
    DEBUG_LOG_SYSCALL(("local timer: "));
    if (enable == 0)
    {
        DEBUG_LOG_SYSCALL(("local timer disable"));
        local_timer_disable();
    }
    else
    {
        DEBUG_LOG_SYSCALL(("local timer enable"));
        local_timer_enable();
    }
    DEBUG_LOG_SYSCALL(("\n"));
}

void syscall_gettime(double *t)
{
    DEBUG_LOG_SYSCALL(("get time: "));
    register unsigned long freq;
    register unsigned long ct;

    asm volatile("mrs %0, CNTFRQ_EL0\n"
                 "mrs %1, CNTPCT_EL0\n"
                 : "=r"(freq), "=r"(ct));
    *t = (double)ct / (double)freq;
    DEBUG_LOG_SYSCALL(("\n"));
}

void syscall_uart_send(char x1)
{
    /* wait until we can send */
    do
    {
        asm volatile("nop");
    } while (*UART0_FR & 0x20);
    /* write the character to the buffer */
    *UART0_DR = x1;
}

void syscall_uart_recv(char *x1)
{
    /* wait until something is in the buffer */
    do
    {
        asm volatile("nop");
    } while (*UART0_FR & 0x10);
    /* read it and return */
    *x1 = (char)(*UART0_DR);
}

void syscall_fork(int *x1)
{
    *x1 = do_fork();
    printf("==%d==", *x1);
}

void syscall_exec(unsigned long x1)
{
    do_exec(x1);
}

void syscall_exit(int *x1)
{
    exit_process();
}

/* Can't work */
void syscall_load_images()
{
    loadimg();
}

void syscall_delay()
{
    DEBUG_LOG_SYSCALL(("syscall_delay"));
    DEBUG_LOG_SYSCALL(("\n"));
    bottom_half_set(0x0);
}

void syscall_delay_without_bottom_half()
{
    DEBUG_LOG_SYSCALL(("syscall_delay_without_bottom_half"));
    for (int i = 0; i < 5; i++)
    {
        DEBUG_LOG_SYSCALL(("."));
        // very very very slow in real rpi3, but very very very fast in qemu
        asm volatile(
            "mov  x0, #0xfffff\n"
            "bottom_half_0_l: subs  x0, x0, #1\n"
            "bne   bottom_half_0_l\n");
    }
    uart_send('n');
    DEBUG_LOG_SYSCALL(("\n"));
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
    case 0x4:
        syscall_uart_send(x1);
        break;
    case 0x5:
        syscall_uart_recv((char *)x1);
        break;
    case 0x30:
        syscall_fork((int *)x1);
        break;
    case 0x31:
        syscall_exec((unsigned long)x1);
        break;
    case 0x32:
        syscall_exit((int *)x1);
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
        DEBUG_LOG_SYSCALL(("Can find this system call"));

        while (1)
        {
        };
    }
};

/*
void uart_send(unsigned int x1)
{
    syscall_1(0x4, x1);
}

char uart_recv()
{
    char x1;
    asm volatile("mov x1, %0\\n"
                 "mov x0, #0x05\\n"
                 "svc #0x80\\n" ::"r"(&x1));
    return x1;
}
*/

void core_timer(int enable)
{
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x0\n"
                 "svc #0x80\n" ::"r"(enable));
}

double gettime()
{
    double t;
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x02\n"
                 "svc #0x80\n" ::"r"(&t));

    return t;
}

int fork()
{
    int return_value;
    asm volatile("mov x10, x0\n"
                 "mov x11, x1\n"
                 "mov x12, x2\n");
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x30\n"
                 "svc #0x80\n" ::"r"(&return_value));
    /*
    if (return_value == 0)
    {
        thread_start();
    }
    */
    return return_value;
}

int exec(unsigned int func)
{
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x31\n"
                 "svc #0x80\n" ::"r"(func));
}

int exit()
{
}