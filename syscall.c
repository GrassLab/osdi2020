#include "syscall.h"
#include "bootloader.h"
#include "timer.h"
#include "bottom_half.h"
#include "task.h"
#include "printf.h"
#include "debug.h"
#include "signal.h"
#include "system.h"

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
    DEBUG_LOG_SYSCALL(("\n\r"));
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
    DEBUG_LOG_SYSCALL(("\n\r"));
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
    DEBUG_LOG_SYSCALL(("\n\r"));
}

void syscall_uart_send(char x1)
{
    //enable_irq();
    /* wait until we can send */
    do
    {
        asm volatile("nop");
    } while (*UART0_FR & 0x20);
    /* write the character to the buffer */
    *UART0_DR = x1;
    //disable_irq();
}

void syscall_uart_recv(char *x1)
{
    do
    {
        asm volatile("nop");
    } while (*UART0_FR & 0x10);
    *x1 = (char)(*UART0_DR);
    //bottom_half_set(0x1);
}

void syscall_fork(int *x1)
{
    *x1 = do_fork();
    DEBUG_LOG_SYSCALL(("==%d==", *x1));
}

void syscall_exec(unsigned long x1)
{
    do_exec(x1);
}

void syscall_exit(int x1)
{
    exit_process(get_current());
}

void syscall_get_taskid(int *x1)
{
    *x1 = get_current();
}

void syscall_kill(int task_id, int signal)
{
    // signal_run(task_id, signal);
    DEBUG_LOG_SYSCALL(("kill: taskid[%d], signal[%d]\n\r"));
    signal_raise(task_id, signal);
}

/* Can't work */
void syscall_load_images()
{
    loadimg();
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
        enable_irq();
        syscall_gettime((double *)x1);
        break;
        /*
    case 0x3:
        syscall_load_images();
        break;
        */
    case 0x4:
        enable_irq();
        syscall_uart_send(x1);
        break;
    case 0x5:
        enable_irq();
        syscall_uart_recv((char *)x1);
        break;
    case 0x20:
        enable_irq();
        syscall_get_taskid((int *)x1);
        break;
    case 0x30:
        syscall_fork((int *)x1);
        break;
    case 0x31:
        syscall_exec((unsigned long)x1);
        break;
    case 0x32:
        syscall_exit((int)x1);
        break;
    case 0x40:
        enable_irq();
        syscall_kill((int)x1, (int)x2);
        break;
    // not this syscall
    default:
        printf("Can find this system call, Halt");
        reset(0);

        while (1)
        {
        };
    }
};

void uart_send(unsigned int x1)
{
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x04\n"
                 "svc #0x80\n" ::"r"(x1));
}

char uart_recv()
{
    char x1;
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x05\n"
                 "svc #0x80\n" ::"r"(&x1));
    return x1;
}

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

int get_taskid()
{
    int taskid;
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x20\n"
                 "svc #0x80\n" ::"r"(&taskid));
    return taskid;
}

int fork()
{
    int return_value = 0;
    /*
    asm volatile("mov x10, x0\n"
                 "mov x11, x1\n"
                 "mov x12, x2\n");
                 */
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x30\n"
                 "svc #0x80\n" ::"r"(&return_value));
    if (return_value == 0)
    {
        // thread_start();
        //asm volatile("mov x29, x10\n");
    }
    return return_value;
}

int exec(unsigned int func)
{
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x31\n"
                 "svc #0x80\n" ::"r"(func));
}

void exit(int value)
{
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x32\n"
                 "svc #0x80\n" ::"r"(value));
}

void kill(int task_id, int signal)
{
    asm volatile("mov x1, %0\n"
                 "mov x2, %1\n"
                 "mov x0, #0x40\n"
                 "svc #0x80\n" ::"r"(task_id),
                 "r"(signal));
}
