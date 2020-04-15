#include "int.h"
#include "uart.h"
#include "timer.h"
#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int*)(0x40000034))
#define LOCAL_TIMER_CONTROL_IRQ_CLR_REG ((volatile unsigned int*)(0x40000038))
#define LOCAL_TIMER_ENABLE (0x30000000)
#define LOCAL_TIMER_RELOAD (25000000)

#define CORE0_TIMER_IRQ_CTRL ((volatile unsigned int*)(0x40000040))
#define EXPIRE_PERIOD 0xfffffff


void require_el1() {
        uart_puts("Current at EL1.");
        unsigned long esr;
        unsigned long elr;
        unsigned long spsr;
        unsigned long far;
        asm volatile("mrs %0, esr_el1" : "=r"(esr));
        asm volatile("mrs %0, elr_el1" : "=r"(elr));
        asm volatile("mrs %0, spsr_el1" : "=r"(spsr));
        asm volatile("mrs %0, far_el1" : "=r"(far));
    if((esr<<(17 + 32))>>(17 + 32) == 1) {

        uart_puts("\n\rException return address: 0x");
        uart_hex(elr);
        uart_puts("\n\rException class(EC): 0x");
        uart_hex(esr>>26);
        uart_puts("\n\rInstruction specific syndrome(ISS): ");
        uart_hex((esr<<(17 + 32))>>(17 + 32));
    } else if((esr<<(17 + 32))>>(17 + 32) == 2) {
        uart_puts("\n\rset up timer in EL1.");
        //asm volatile("hvc #2");
        local_timer_init();
        local_timer_handler();
        core_timer_init();
        core_timer_handler();
        //asm volatile("svc #2");
    }
}

void require_el2() {
        uart_puts("Current at EL2.");
        unsigned long esr;
        unsigned long elr;
        unsigned long spsr;
        unsigned long far;
        asm volatile("mrs %0, esr_el2" : "=r"(esr));
        asm volatile("mrs %0, elr_el2" : "=r"(elr));
        asm volatile("mrs %0, spsr_el2" : "=r"(spsr));
        asm volatile("mrs %0, far_el2" : "=r"(far));
    if((esr<<(17 + 32))>>(17 + 32) == 1) {

        uart_puts("\n\rException return address: 0x");
        uart_hex(elr);
        uart_puts("\n\rException class(EC): 0x");
        uart_hex(esr>>26);
        uart_puts("\n\rInstruction specific syndrome(ISS): ");
        uart_hex((esr<<(17 + 32))>>(17 + 32));
    } else if((esr<<(17 + 32))>>(17 + 32) == 2) {
        uart_puts("\n\rset up timer in EL2.");
        local_timer_init();
        local_timer_handler();
        core_timer_handler();
        core_timer_init();
    }
}

void require_el3() {
        uart_puts("Current at EL3.");
        unsigned long esr;
        unsigned long elr;
        unsigned long spsr;
        unsigned long far;
        asm volatile("mrs %0, esr_el3" : "=r"(esr));
        asm volatile("mrs %0, elr_el3" : "=r"(elr));
        asm volatile("mrs %0, spsr_el3" : "=r"(spsr));
        asm volatile("mrs %0, far_el3" : "=r"(far));
    if((esr<<(17 + 32))>>(17 + 32) == 1) {

        uart_puts("\n\rException return address: 0x");
        uart_hex(elr);
        uart_puts("\n\rException class(EC): 0x");
        uart_hex(esr>>26);
        uart_puts("\n\rInstruction specific syndrome(ISS): ");
        uart_hex((esr<<(17 + 32))>>(17 + 32));
    } else if((esr<<(17 + 32))>>(17 + 32) == 2) {

    }
}

void timer_irq() {
    if((unsigned int)((*LOCAL_TIMER_CONTROL_REG) & 0x80000000)) {
        uart_puts("Local timer interrupt\n\r");

    } else {
        uart_puts("Core timer interrupt\n\r");
        register unsigned long r asm("x0");
        r = EXPIRE_PERIOD;
        asm volatile("msr cntp_tval_el0, x0");
    }
    *(LOCAL_TIMER_CONTROL_IRQ_CLR_REG) = 0xc0000000;
}


/*
void exc_handler_syn() {
    unsigned long esr;
    unsigned long elr;
    unsigned long spsr;
    unsigned long far;
    asm volatile("mrs %0, esr_el2" : "=r"(esr));
    asm volatile("mrs %0, elr_el2" : "=r"(elr));
    asm volatile("mrs %0, spsr_el2" : "=r"(spsr));
    asm volatile("mrs %0, far_el2" : "=r"(far));
    uart_puts("\n\rException return address: 0x");
    uart_hex(elr);
    uart_puts("\n\rException class(EC): 0x");
    uart_hex(esr>>26);
    uart_puts("\n\rInstruction specific syndrome(ISS): ");
    uart_hex((esr<<17)>>17);
    while(1){};
}

void exc_handler() {
    unsigned long esr;
    unsigned long elr;
    unsigned long spsr;
    unsigned long far;
    asm volatile("mrs %0, esr_el2" : "=r"(esr));
    asm volatile("mrs %0, elr_el2" : "=r"(elr));
    asm volatile("mrs %0, spsr_el2" : "=r"(spsr));
    asm volatile("mrs %0, far_el2" : "=r"(far));
    uart_puts("\n\rException return address: 0x");
    uart_hex(elr);
    uart_puts("\n\rException class(EC): 0x");
    uart_hex(esr>>26);
    uart_puts("\n\rInstruction specific syndrome(ISS): 0x");
    uart_hex((esr<<(17+32))>>(17+32));
    while(1){};
}
*/