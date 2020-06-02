#include "userlib.h"

int get_taskid()
{
    unsigned long x0;
    asm volatile("mov x8, %0" : :"r"(SYSCALL_GET_TASK_ID));
    asm volatile("svc #0");
    asm volatile("mov %0, x0" : "=r"(x0) :);
    return x0;    
}

int __get_taskid()
{
    Task *task = get_current();
    return task->id;
}

void get_timestamp()
{
    asm volatile("mov x8, %0" : :"r"(SYSCALL_GET_TIMESTAMP));
    asm volatile("svc #0");
}

void __get_timestamp()
{
    long long int cntfrq;
    long long int cntptc;
    asm volatile ("mrs %0, cntfrq_el0" : "=r" (cntfrq));
    asm volatile ("mrs %0, cntpct_el0" : "=r" (cntptc));
    cntptc *= 100000;
    int intPart = (cntptc / cntfrq) / 100000;
    int floatPart = (cntptc / cntfrq) % 100000;
    uart_puts("[");
    uart_print_int(intPart);
    uart_puts(".");
    uart_print_int(floatPart);
    uart_puts("]");
    uart_puts("\n");
}

int exec(void(*func)())
{
    unsigned long x0;
    asm volatile("mov x8, %0" : :"r"(SYSCALL_EXEC));
    asm volatile("mov x0, %0" : :"r"(func));
    asm volatile("svc #0");
    asm volatile("mov %0, x0" : "=r"(x0) :);
}

void exit(int status)
{
    asm volatile("mov x8, %0" : :"r"(SYSCALL_EXIT));
    asm volatile("mov x0, %0" : :"r"(status));
    asm volatile("svc #0");
}

int fork()
{
    unsigned long x0;
    asm volatile("mov x8, %0" : :"r"(SYSCALL_FORK));
    asm volatile("svc #0");
    asm volatile("mov %0, x0" : "=r"(x0) :);
    return x0;  
}

char uart_read() 
{
    unsigned long x0;
    asm volatile("mov x8, %0" : :"r"(SYSCALL_UART_READ));
    asm volatile("svc #0");
    asm volatile("mov %0, x0" : "=r"(x0) :);
    return x0;  
}

void uart_write(char *c)
{
    unsigned long x0;
    asm volatile("mov x8, %0" : :"r"(SYSCALL_UART_WRITE));
    asm volatile("mov x0, %0" : :"r"(c));
    asm volatile("svc #0");
}