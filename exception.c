#include "uart.h"
#include "irq.h"
#include "timer.h"
#include "exception.h"
#include "syscall.h"
#include "debug.h"
#include "system.h"

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
    reset(0);
    while (1)
    {
    }
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

    DEBUG_LOG_EXCEPTION(("Current EL: %d\r\n", current_el));

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

    DEBUG_LOG_EXCEPTION(("*Interrput*: <Synchronous>\r\n"));
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
        DEBUG_LOG_EXCEPTION(("System call"));
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
        uart_send_hex(elr);
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
        reset(0);
        while (1)
        {
        }
        break;
    }
    DEBUG_LOG_EXCEPTION(("\n"));

    unsigned long iss = esr & 0x00FFFFFF;
    DEBUG_LOG_EXCEPTION(("Exception return address: 0x%x\n", elr));
    // ESR [31:26]
    DEBUG_LOG_EXCEPTION(("Exception class (EC): 0x%x\n", esr >> 26));
    // ESR [24:0]
    DEBUG_LOG_EXCEPTION(("Instruction specific syndrome (ISS): 0x%x\n", iss));

    if (iss == 0x80)
    {
        DEBUG_LOG_EXCEPTION(("System Call[%d]: ", x0));
        syscall_router(x0, x1, x2, x3);

        DEBUG_LOG_EXCEPTION(("\n===\n"));
    }

    if (esr >> 26 != 0b010101)
    {
        reset(0);
        while (1)
        {
        }
    }
}

void irq_handler()
{
    // DEBUG_LOG_EXCEPTION(("*Interrput*: <IRQ>\n"));

    irq_router();
}
