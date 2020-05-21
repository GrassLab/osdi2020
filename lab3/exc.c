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
#include "irq.h"

#define CORE0_IRQ_SOURCE ((volatile unsigned int*)0x40000060)
#define ARM_LOCAL_TIMER_IRQ 0b100000000000
#define ARM_CORE_TIMER_IRQ 0b10

#define IRQ_PENDIGN_1 ((volatile unsigned int*)0x3F00B204)

int core_timer_irq_queue = 0;
int local_timer_irq_queue = 0;




int get_currentEL(){
    int el;

    asm volatile(
        "mrs     %[el], CurrentEL;"
        "and     %[el], %[el], #12 ;"
        : [el] "=r" (el)
        ::
    );

    return el>>2;
}

void show_currentEL(){
    int el = get_currentEL();

    uart_puts("CurrentEL: ");
    uart_hex(el);
    uart_puts("\n");
}

void show_esr_elr(unsigned long esr, unsigned long elr){
    uart_puts("sync_exc1_handler\n");
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
}


/* ========================= system call ========================= */

void sysCall_print_esr_elr(){
    supervisor_call_1();
}

void sysCall_set_timer(){
    supervisor_call_2();
}

void sysCall_unset_timer(){
    supervisor_call_3();
}

void sysCall_miniUART_irq(){
    supervisor_call_4();
}

void dequeue_core_timer_bottom_half(){
    supervisor_call_5();
}

void dequeue_local_timer_bottom_half(){
    supervisor_call_6();
}

void sysCall_handler_el0(int num){
    unsigned long esr, elr;

    switch(num){
        case 1:
            asm volatile(
                "mrs %[esr], esr_el1;"
                "mrs %[elr], elr_el1;"

                : [esr] "=r" (esr), [elr] "=r" (elr)
                ::
            );
            show_esr_elr(esr, elr);
            break;
        case 2:
            core_timer_enable();
            arm_local_timer_init();
            break;
        case 3:
            arm_local_timer_cancel();
            core_timer_cancel();
            break;
        case 4:
            enable_miniUART_interrupt();
            break;
        case 5:
            core_timer_irq_queue = 0;
            break;
        case 6:
            local_timer_irq_queue = 0;
            break;

        default:
            uart_puts("system call ");
            uart_hex(num);
            uart_puts(" not exist \n");
            break;
    }
}


void sync_exc0_handler(){
    unsigned long esr, elr;
    asm volatile(
        "mrs %[esr], esr_el1;"
        "mrs %[elr], elr_el1;"

        : [esr] "=r" (esr), [elr] "=r" (elr)
        ::
    );

    if(esr>>26 == 0x15){
        int supervisorCallNum = esr & 0x1FFFFFF;
        sysCall_handler_el0( supervisorCallNum );
    }else{
        show_esr_elr(esr, elr);
    }
}

void sync_exc1_handler(){
    show_currentEL();
    unsigned long esr, elr;

    asm volatile(
        "mrs %[esr], esr_el1;"
        "mrs %[elr], elr_el1;"

        : [esr] "=r" (esr), [elr] "=r" (elr)
        ::
    );    
    
    show_esr_elr(esr, elr);
}

void sync_exc2_handler(){
    show_currentEL();
    unsigned long esr, elr;

    asm volatile(
        "mrs %[esr], esr_el2;"
        "mrs %[elr], elr_el2;"

        : [esr] "=r" (esr), [elr] "=r" (elr)
        ::
    );
    
   show_esr_elr(esr, elr);
}

void core_timer_irq_bottom_half(){
    uart_puts("start arm core bottom half\n");
    simulate_bottom_half();
    uart_puts("finish arm core bottom half\n");
}

void local_timer_irq_bottom_half(){
    uart_puts("start arm local bottom half\n");
    simulate_bottom_half();
    uart_puts("finish arm local bottom half\n");
}

void irq_exc_handler(){
    unsigned int irq_status = *CORE0_IRQ_SOURCE;

    if( irq_status & ARM_CORE_TIMER_IRQ){
        uart_puts("ARM_CORE_TIMER_IRQ\n");
        core_timer_handler();
        core_timer_irq_queue = 1;
        

        // core_timer_irq_bottom_half();
        return;
    }

    if( irq_status & ARM_LOCAL_TIMER_IRQ) {
        uart_puts("ARM_LOCAL_TIMER_IRQ\n");
        arm_local_timer_handler();
        local_timer_irq_queue = 1;

        // local_timer_irq_bottom_half();
        return;
    }

    unsigned int IRQ_pending = *IRQ_PENDIGN_1;
    if(IRQ_pending & 1<<29){
        show_interrupt_status();
        return;
    }
        
    uart_puts("irq_exc_handler\n");
}

void SError_handler(){   
    uart_puts("SError\n");
}

void bottom_half_check(){
    if(core_timer_irq_queue) {
        core_timer_irq_bottom_half();
        dequeue_core_timer_bottom_half();
    }
    if(local_timer_irq_queue) {
        local_timer_irq_bottom_half();
        dequeue_local_timer_bottom_half();
    }
}






