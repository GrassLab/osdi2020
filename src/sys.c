#include "sys.h"

uint64_t sys_get_cntfrq() {
    asm volatile("mov x8, %0" : : "r"(SYS_GET_CNTFRQ));
    asm volatile("svc #0");
    register uint64_t ret_val;
    asm volatile("mov %0, x0" : "=r"(ret_val));
    return ret_val;
}

uint64_t sys_get_cntpct() {
    asm volatile("mov x8, %0" : : "r"(SYS_GET_CNTPCT));
    asm volatile("svc #0");
    register uint64_t ret_val;
    asm volatile("mov %0, x0" : "=r"(ret_val));
    return ret_val;
}

uint32_t sys_uart_read(char buf[], uint32_t size) {
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

uint32_t sys_uart_write(const char buf[], uint32_t size) {
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
