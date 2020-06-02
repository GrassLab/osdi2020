#include "peripherals/uart.h"
#include "sys.h"
#include "fork.h"
#include "sched.h"

/*
void handle_sync(unsigned long esr, unsigned long address) {
    uart_send_hex(address);
    uart_send_hex(esr >> 26);
    uart_send_hex(esr & 0x1ffffff);
}
*/
#define SYS_ALLOCATOR_REGISTER      5
#define SYS_ALLOCATOR_ALLOC         6
#define SYS_ALLOCATOR_FREE          7
#define SYS_ALLOCATOR_UNREGISTER    8

int handle_el0_sync(unsigned long arg0, unsigned long arg1) {
    int syscall;
    asm("mov %0, x8" : "=r"(syscall));
    if (syscall == SYS_UART_WRITE) {
        for (int i = 0; i < arg1; ++ i) {
            uart_send(((char*)arg0)[i]);
        }
        return arg1;
    } else if (syscall == SYS_UART_READ) {
        for (int i = 0; i < arg1; ++ i) {
            ((char*)arg0)[i] = uart_getc();
        }
        return arg1;
    } else if (syscall == SYS_EXEC) {
        return do_exec(arg0);
    } else if (syscall == SYS_FORK) {
        return __clone(0, 0, 0);
    } else if (syscall == SYS_EXIT) {
        exit_process();
        return 0;
    } else if (syscall == SYS_ALLOCATOR_REGISTER) {
        uart_send_ulong(arg0);
        return sys_allocator_register(arg0);
    } else if (syscall == SYS_ALLOCATOR_ALLOC) {
        return sys_allocator_alloc(arg0);
    } else if (syscall == SYS_ALLOCATOR_FREE) {
        sys_allocator_free(arg0, arg1);
        return 0;
    } else if (syscall == SYS_ALLOCATOR_UNREGISTER) {
        sys_allocator_unregister(arg0);
        return 0;
    }
    uart_puts("unknown syscall\n");
    return -1;
}

int handle_el1_sync(unsigned long arg0, unsigned long arg1) {
    uart_puts("el1 sync\n");
    return -1;
}
