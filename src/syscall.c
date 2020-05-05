#include "task.h"
void uart_write(char ch) {
    asm volatile("mov x0, %0" : "=r"(ch));
    asm volatile("mov x8, #0");
    asm volatile("svc 0");
}

char uart_read() {
    char ch;
    asm volatile("mov x8, #1");
    asm volatile("svc 0");
    asm volatile("mov %0, x0" : "=r"(ch));
    return ch;
}

void exec(void (*func)()) {
    asm volatile("mov x0, %0" : "=r"(func));
    asm volatile("mov x8, #2");
    asm volatile("svc 0");
}

int fork() {
    asm volatile("sys:");
    asm volatile("mov x8, #3");
    asm volatile("svc 0");
    asm volatile("sys2:");
    struct task_t* task = get_current();
    return task->utask.fork_id;
}
