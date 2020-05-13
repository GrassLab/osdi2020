#include "syscall.h"

int get_taskid()
{
    unsigned long x0;
    asm volatile("mov x8, %0" : :"r"(SYS_GET_TASKID));
    asm volatile("svc #0");
    asm volatile("mov %0, x0" : "=r"(x0) :);
    return x0;    
}

void get_input(){
    unsigned long x0;
    asm volatile("mov x8, %0" : :"r"(SYS_UART_READ));
    asm volatile("svc #0");
    asm volatile("mov %0, x0" : "=r"(x0) :);
    return x0;    
}

void printf_(char *c){
    unsigned long x0;
    asm volatile("mov x8, %0" : :"r"(SYS_UART_WRITE));
    asm volatile("mov x0, %0" : :"r"(c));
    asm volatile("svc #0");
}