#include "lib.h"

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

/*
void exit(int value)
{
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x32\n"
                 "svc #0x80\n" ::"r"(value));
}
*/

void kill(int task_id, int signal)
{
    asm volatile("mov x1, %0\n"
                 "mov x2, %1\n"
                 "mov x0, #0x40\n"
                 "svc #0x80\n" ::"r"(task_id),
                 "r"(signal));
}
