#include "sys.h"

uint64_t get_cntfrq() {
    asm volatile("mov x8, %0" : : "r"(SYS_GET_CNTFRQ));
    asm volatile("svc #0");
    register uint64_t ret_val;
    asm volatile("mov %0, x0" : "=r"(ret_val));
    return ret_val;
}

uint64_t get_cntpct() {
    asm volatile("mov x8, %0" : : "r"(SYS_GET_CNTPCT));
    asm volatile("svc #0");
    register uint64_t ret_val;
    asm volatile("mov %0, x0" : "=r"(ret_val));
    return ret_val;
}

uint32_t uart_read(char buf[], uint32_t size) {
    register char* buf_reg = buf;
    register uint32_t size_reg = size;
    asm volatile("mov x8, %0" : : "r"(SYS_UART_READ));
    asm volatile("mov x0, %0" : : "r"(buf_reg));
    asm volatile("mov x1, %0" : : "r"(size_reg));
    asm volatile("svc #0");
    register uint32_t ret_val;
    asm volatile("mov %0, x0" : "=r"(ret_val));
    return ret_val;
}

uint32_t uart_write(const char buf[], uint32_t size) {
    register char* buf_reg = (char*) buf;
    register uint32_t size_reg = size;
    asm volatile("mov x8, %0" : : "r"(SYS_UART_WRITE));
    asm volatile("mov x0, %0" : : "r"(buf_reg));
    asm volatile("mov x1, %0" : : "r"(size_reg));
    asm volatile("svc #0");
    register uint32_t ret_val;
    asm volatile("mov %0, x0" : "=r"(ret_val));
    return ret_val;
}

int exec(void(*func)()) {
    asm volatile("mov x8, %0" : : "r"(SYS_EXEC));
    asm volatile("mov x0, %0" : : "r"(func));
    asm volatile("svc #0");
    register int ret_val;
    asm volatile("mov %0, x0" : "=r"(ret_val));
    return ret_val;
}

int fork() {
    asm volatile("mov x8, %0" : : "r"(SYS_FORK));
    asm volatile("svc #0");
    register int ret_val;
    asm volatile("mov %0, x0" : "=r"(ret_val));
    return ret_val;
}

void exit(int status) {
    asm volatile("mov x8, %0" : : "r"(SYS_EXiT));
    asm volatile("mov x0, %0" : : "r"(status));
    asm volatile("svc #0");
}
