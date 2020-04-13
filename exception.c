#include "uart.h"
#include "irq.h"
#include "timer.h"
#include "exception.h"
#include "bootloader.h"

/**
 * common exception handler
 */
void exc_handler(unsigned long type)
{
    unsigned long esr;
    unsigned long elr;
    unsigned long spsr;
    unsigned long far;

    asm volatile(
        "mrs %0, elr_el2;"
        "mrs %1, esr_el2;"
        "mrs %2, spsr_el2;"
        "mrs %3, far_el2;"
        : "=r"(elr), "=r"(esr), "=r"(spsr), "=r"(far));

    // print out interruption type
    switch (type)
    {
    case 0:
        uart_puts("Synchronous");
        break;
    case 1:
        uart_puts("IRQ");
        break;
    case 2:
        uart_puts("FIQ");
        break;
    case 3:
        uart_puts("SError");
        break;
    }
    uart_puts(": ");
    // decode exception type (some, not all. See ARM DDI0487B_b chapter D10.2.28)
    switch (esr >> 26)
    {
    case 0b000000:
        uart_puts("Unknown");
        break;
    case 0b000001:
        uart_puts("Trapped WFI/WFE");
        break;
    case 0b001110:
        uart_puts("Illegal execution");
        break;
    case 0b010101:
        uart_puts("System call");
        break;
    case 0b100000:
        uart_puts("Instruction abort, lower EL");
        break;
    case 0b100001:
        uart_puts("Instruction abort, same EL");
        break;
    case 0b100010:
        uart_puts("Instruction alignment fault");
        break;
    case 0b100100:
        uart_puts("Data abort, lower EL");
        break;
    case 0b100101:
        uart_puts("Data abort, same EL");
        break;
    case 0b100110:
        uart_puts("Stack alignment fault");
        break;
    case 0b101100:
        uart_puts("Floating point");
        break;
    default:
        uart_puts("Unknown");
        break;
    }
    // decode data abort cause
    if (esr >> 26 == 0b100100 || esr >> 26 == 0b100101)
    {
        uart_puts(", ");
        switch ((esr >> 2) & 0x3)
        {
        case 0:
            uart_puts("Address size fault");
            break;
        case 1:
            uart_puts("Translation fault");
            break;
        case 2:
            uart_puts("Access flag fault");
            break;
        case 3:
            uart_puts("Permission fault");
            break;
        }
        switch (esr & 0x3)
        {
        case 0:
            uart_puts(" at level 0");
            break;
        case 1:
            uart_puts(" at level 1");
            break;
        case 2:
            uart_puts(" at level 2");
            break;
        case 3:
            uart_puts(" at level 3");
            break;
        }
    }
    // dump registers
    uart_puts(":\n  ESR_EL1 ");
    uart_send_hex(esr >> 32);
    uart_send_hex(esr);
    uart_puts(" ELR_EL1 ");
    uart_send_hex(elr >> 32);
    uart_send_hex(elr);
    uart_puts("\n SPSR_EL1 ");
    uart_send_hex(spsr >> 32);
    uart_send_hex(spsr);
    uart_puts(" FAR_EL1 ");
    uart_send_hex(far >> 32);
    uart_send_hex(far);
    uart_puts("\n");

    // no return from exception for now
}

void syscall_core_timer(int enable)
{
    uart_puts("core timer: ");
    if (enable == 0)
    {
        uart_puts("core timer disable");
        _core_timer_disable();
    }
    else
    {
        uart_puts("core timer enable");
        _core_timer_enable();
    }
}

void syscall_local_timer(int enable)
{
    uart_puts("local timer: ");
    if (enable == 0)
    {
        uart_puts("local timer disable");
        local_timer_disable();
    }
    else
    {
        uart_puts("local timer enable");
        local_timer_enable();
    }
}

void syscall_gettime(double *t)
{
    uart_puts("get time: ");
    register unsigned long freq;
    register unsigned long ct;

    asm volatile("mrs %0, CNTFRQ_EL0\n"
                 "mrs %1, CNTPCT_EL0\n"
                 : "=r"(freq), "=r"(ct));
    *t = (double)ct / (double)freq;
}

void syscall_load_images()
{
    loadimg();
}

unsigned long get_current_el()
{
    unsigned long current_el;

    asm volatile(
        "mrs %0, CurrentEL;"
        : "=r"(current_el));
    current_el = (current_el >> 2) & 3;

    return current_el;
}

/*
https://developer.arm.com/docs/ddi0595/e/aarch64-system-registers/currentel

EL [63:4]: Reserved, RES0.
EL [3:2]:
EL  	Meaning
0b00	EL0
0b01	EL1
0b10	EL2
0b11	EL3

EL [1:0]: Reserved, RES0.
*/
void synchronous_handler(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3)
{
    unsigned long esr;
    unsigned long elr;
    unsigned long spsr;
    unsigned long far;

    unsigned long current_el;
    current_el = get_current_el();

    uart_puts("Current EL: ");
    uart_send_int(current_el);
    uart_puts("\n");

    switch (current_el)
    {
    case 0:
        break;
    case 1:
        asm volatile(
            "mrs %0, elr_el1;"
            "mrs %1, esr_el1;"
            : "=r"(elr), "=r"(esr));
        break;
    case 2:
        asm volatile(
            "mrs %0, elr_el2;"
            "mrs %1, esr_el2;"
            : "=r"(elr), "=r"(esr));
        break;
    case 3:
        uart_puts("Something wrong. Halt");
        while (1)
            ;
        break;
    }

    /* used in el2 */
    /*
    asm volatile(
        "mrs %0, elr_el2;"
        "mrs %1, esr_el2;"
        : "=r"(elr), "=r"(esr));
        */

    uart_puts("*Interrput*: <Synchronous>\n");
    // decode exception type (some, not all. See ARM DDI0487B_b chapter D10.2.28)
    switch (esr >> 26)
    {
    case 0b000000:
        uart_puts("Unknown");
        break;
    case 0b000001:
        uart_puts("Trapped WFI/WFE");
        break;
    case 0b001110:
        uart_puts("Illegal execution");
        break;
    case 0b010101:
        uart_puts("System call");
        break;
    case 0b100000:
        uart_puts("Instruction abort, lower EL");
        break;
    case 0b100001:
        uart_puts("Instruction abort, same EL");
        break;
    case 0b100010:
        uart_puts("Instruction alignment fault");
        break;
    case 0b100100:
        uart_puts("Data abort, lower EL");
        break;
    case 0b100101:
        uart_puts("Data abort, same EL");
        break;
    case 0b100110:
        uart_puts("Stack alignment fault");
        break;
    case 0b101100:
        uart_puts("Floating point");
        break;
    default:
        uart_puts("Unknown");
        break;
    }
    uart_puts("\n");

    unsigned long iss = esr & 0x00FFFFFF;
    uart_puts("Exception return address: ");
    uart_send_hex(elr);
    // ESR [31:26]
    uart_puts("\nException class (EC): ");
    uart_send_hex(esr >> 26);
    // ESR [24:0]
    uart_puts("\nInstruction specific syndrome (ISS): ");
    uart_send_hex(iss);

    uart_puts("\n");

    if (iss == 0x80)
    {
        uart_puts("System Call[");
        uart_send_int(x0);
        uart_puts("]: ");
        switch (x0)
        {
        // arm core timer
        case 0x0:
            syscall_core_timer(x1);
            break;
        // arm local timer
        case 0x1:
            syscall_local_timer(x1);
            break;
        // get time
        case 0x2:
            syscall_gettime((double *)x1);
            break;
        case 0x3:
            syscall_load_images();
            break;
        // not this syscall
        default:
            uart_puts("Can find this system call");

            while (1)
                ;
        }
        uart_puts("\n===\n");
    }
}

void irq_handler()
{
    uart_puts("*Interrput*: <IRQ>\n");

    irq();
}