#include "lib/type.h"
#include "kernel/peripherals/uart.h"

#include "exception.h"

void exec_controller ( uint64_t elr, uint64_t esr )
{
    int ec = (((uint32_t)esr)>>26);
    // int il = (((uint32_t)esr)>>25) & 0x1 ;
    int iss = (((uint32_t)esr)) & 0x00FFFFFF;

    uart_printf("Exception return address: %x\n", elr);
    uart_printf("Exception clss (EC): %x\n", ec);
    uart_printf("Instruction specific syndrome (ISS): %x\n", iss);

    // asm volatile ( "eret;" );
}

int get_current_el ()
{
    int el;
    asm volatile (
        "mrs x0, CurrentEL;"
        "mov %0, x0;" 
        : "=r"(el)
    );

    el >>= 2;
    el &= 2;

    return el;
}