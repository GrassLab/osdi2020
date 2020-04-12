/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "uart.h"
#include "timer.h"

#define CORE0_IRQ_SOURCE ((volatile unsigned int*)0x40000060)
#define ARM_LOCAL_TIMER_IRQ 0b100000000000
#define ARM_CORE_TIMER_IRQ 0b10

inline void kernel_entry(){
    asm volatile(
        "sub    sp, sp, #8 * 32;"
        "stp x0, x1, [sp, #16 * 0];"
        "stp x2, x3, [sp, #16 * 1];"
        "stp x4, x5, [sp, #16 * 2];"
        "stp x6, x7, [sp, #16 * 3];"
        "stp x8, x9, [sp, #16 * 4];"
        "stp x10, x11, [sp, #16 * 5];"
        "stp x12, x13, [sp, #16 * 6];"
        "stp x14, x15, [sp, #16 * 7];"
        "stp x16, x17, [sp, #16 * 8];"
        "stp x18, x19, [sp, #16 * 9];"
        "stp x20, x21, [sp, #16 * 10];"
        "stp x22, x23, [sp, #16 * 11];"
        "stp x24, x25, [sp, #16 * 12];"
        "stp x26, x27, [sp, #16 * 13];"
        "stp x28, x29, [sp, #16 * 14];"
        "str x30, [sp, #16 * 15];"

        :::
    );
}

inline void kernel_exit(){
    asm volatile(
        "ldp x0, x1, [sp, #16 * 0];"
        "ldp x2, x3, [sp, #16 * 1];"
        "ldp x4, x5, [sp, #16 * 2];"
        "ldp x6, x7, [sp, #16 * 3];"
        "ldp x8, x9, [sp, #16 * 4];"
        "ldp x10, x11, [sp, #16 * 5];"
        "ldp x12, x13, [sp, #16 * 6];"
        "ldp x14, x15, [sp, #16 * 7];"
        "ldp x16, x17, [sp, #16 * 8];"
        "ldp x18, x19, [sp, #16 * 9];"
        "ldp x20, x21, [sp, #16 * 10];"
        "ldp x22, x23, [sp, #16 * 11];"
        "ldp x24, x25, [sp, #16 * 12];"
        "ldp x26, x27, [sp, #16 * 13];"
        "ldp x28, x29, [sp, #16 * 14];"
        "ldr x30, [sp, #16 * 15];"
        "add sp, sp, #8 * 32;"
        "eret;"

        :::

    );
}

void enable_interrupt(){
    asm volatile( 
        "mrs     x0, hcr_el2;"
        "orr     x0, x0, 0b10000;"   // IMO physical IRQ Routing
        "msr     hcr_el2, x0;"

        "msr     DAIF, xzr;"

        ::: 
    );
}

void disable_interrupt(){
    asm volatile( 
        "mov    x0, 0x3C0;"
        "msr    DAIF, x0;" 

        ::: 
    );
}

void show_currentEL(){
    int el;

    asm volatile(
        "mrs     %[el], CurrentEL;"
        "and     %[el], %[el], #12 ;"
        : [el] "=r" (el)
        ::
    );

    uart_puts("CurrentEL: ");
    uart_hex(el>>2);
    uart_puts("\n");
}

void supervisor_call(){
    asm volatile( "svc #1;" :::);
}

void brk_instr(){
    asm volatile( "brk #1;" :::);
}



void sync_exc_handler(){
    kernel_entry();

    show_currentEL();
    unsigned long esr, elr;

    asm volatile(
        "mrs %[esr], esr_el2;"
        "mrs %[elr], elr_el2;"

        : [esr] "=r" (esr), [elr] "=r" (elr)
        ::
    );


    uart_puts("***Exception type: Synchronous***\n");

    uart_puts("Exception return adress 0x");
    uart_hex(elr);
    uart_puts("\n");

    uart_puts("Exception class (EC) 0x");
    uart_hex(esr>>26);
    uart_puts("\n");

    // decode Instruction Specific Syndrome (ISS)
    uart_puts("Instruction Specific Syndrome (ISS) 0x");
    uart_hex(esr&0x1FFFFFF);
    uart_puts("\n");

    kernel_exit();
}

void irq_exc_handler(){
    kernel_entry();

    uart_puts("irq_exc_handler\n");
    unsigned int irq_status = *CORE0_IRQ_SOURCE;

    if( (irq_status & ARM_CORE_TIMER_IRQ) > 0){
        uart_puts("ARM_CORE_TIMER_IRQ\n");
        core_timer_handler();
    }

    if( (irq_status & ARM_LOCAL_TIMER_IRQ) > 0) {
        uart_puts("ARM_LOCAL_TIMER_IRQ\n");
        arm_local_timer_handler();
    }

    

    kernel_exit();
}

void SError_handler(){
    uart_puts("SError\n");
}