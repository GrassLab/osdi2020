#include "uart.h"
#include "irq.h"

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

void not_implemented()
{
    uart_puts("kernel panic - function not implemented!");
    while (1)
        ;
}

void synchronous_handler()
{

    unsigned long esr;
    unsigned long elr;
    unsigned long spsr;
    unsigned long far;

    asm volatile(
        "mrs %0, elr_el2;"
        "mrs %1, esr_el2;"
        : "=r"(elr), "=r"(esr));

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

    uart_puts("Exception return address: ");
    uart_send_hex(elr);
    // ESR [31:26]
    uart_puts("\nException class (EC): ");
    uart_send_hex(esr >> 26);
    // ESR [24:0]
    uart_puts("\nInstruction specific syndrome (ISS): ");
    uart_send_hex(esr & 0x00FFFFFF);

    uart_puts("\n");

    while (1)
        ;
}

void irq_handler()
{
    uart_puts("*Interrput*: <IRQ>\n");

    unsigned int arm, arm_local;
    char r;
    arm = *IRQ_BASIC_PENDING;
    arm_local = *CORE0_INTR_SRC;

    if (arm_local & 0x2)
    {
        // core timer interrupt
        uart_puts("core timer\n");
        _core_timer_handler();
    }
}