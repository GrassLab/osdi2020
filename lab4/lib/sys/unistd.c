#include "unistd.h"
#include "sys/syscall.h"

void exec(void (*func)()) {
    asm volatile("mov x1, x0");
    asm volatile("mov x0, %0" : : "r"(SYS_EXEC));
    asm volatile("svc 0");
}

int fork(void) {
    asm volatile("mov x0, %0" : : "r"(SYS_FORK));
    asm volatile("svc 0");
}

void exit(int status) {
    asm volatile("mov x1, x0");
    asm volatile("mov x0, %0" : : "r"(SYS_EXIT));
    asm volatile("svc 0");
}
