#include "sys.h"

void uart_write(char *a) {
    asm volatile("mov x1, x0");
    asm volatile("mov x0, %0" : : "r"(SYS_WRITE_NUMBER));
    asm volatile("svc 0");
}

void uart_read(void) {
    asm volatile("mov x0, %0" : : "r"(SYS_READ_NUMBER));
    asm volatile("svc 0");
}

void exec(void (*func)()) {
    asm volatile("mov x1, x0");
    asm volatile("mov x0, %0" : : "r"(SYS_EXEC_NUMBER));
    asm volatile("svc 0");
}

int fork(void) {
    unsigned long x0;
    asm volatile("mov x0, %0" : : "r"(SYS_FORK_NUMBER));
    asm volatile("svc 0");
    asm volatile("mov %0, x0" : "=r"(x0) :);
    return x0;  
}

void exit(int status) {
    asm volatile("mov x1, x0");
    asm volatile("mov x0, %0" : : "r"(SYS_EXIT_NUMBER));
    asm volatile("svc 0");
}

