#include "syscall.h"

void get_taskid() {
    asm volatile("");
    asm volatile("svc #0");
}