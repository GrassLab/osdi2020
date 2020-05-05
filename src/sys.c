#include "sys.h"

uint64_t sys_get_cntfrq() {
    asm volatile("mov x8, %0" : : "r"(SYS_GET_CNTFRQ));
    asm volatile("svc #0");
    uint64_t ret_val;
    asm volatile("mov %0, x0" : "=r"(ret_val));
    return ret_val;
}

uint64_t sys_get_cntpct() {
    asm volatile("mov x8, %0" : : "r"(SYS_GET_CNTPCT));
    asm volatile("svc #0");
    uint64_t ret_val;
    asm volatile("mov %0, x0" : "=r"(ret_val));
    return ret_val;
}
