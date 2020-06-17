#include "uart.h"
#include "printf.h"
#include "timer.h"

void exc_not_implement(){
    uart_puts("Exception not implement\n");
    while(1);
}


void exc ()
{
    // get el level
    unsigned int el;
    asm volatile (
        "mrs %0, CurrentEL\n"
        :"=r" (el)
    );
    el = el >> 2;

    unsigned long elr_elx = 0;
    unsigned long esr_elx = 0;
    unsigned int exception_imm = 0;
    switch(el){
        case 1:
            uart_puts("Exception Level 1\n");
            asm volatile (
                "mrs %0, elr_el1\n"
                "mrs %1, esr_el1\n"
                :"=r" (elr_elx), "=r" (esr_elx)
            );

            exception_imm = esr_elx & 0xffffff;
            switch(exception_imm){
                case 1:
                    printf ("Exception return address 0x%x\r\n", (void *) elr_elx);
                    printf ("Exception class (EC) 0x%x\r\n", esr_elx >> 26);
                    printf ("Instruction specific syndrome (ISS) 0x%x\r\n", esr_elx & 0xffffff);
                    break;
                case 2:
                    uart_puts("case 2\n");
                    core_timer_enable();
                    local_timer_init();
                    break;
                default:
                    uart_puts("Unhandled svc imm value\n");
                    break;
            }
            break;
        case 2:
            uart_puts("Exception Level 2\n");
            asm volatile (
                "mrs %0, elr_el2\n"
                "mrs %1, esr_el2\n"
                :"=r" (elr_elx), "=r" (esr_elx)
            );
            break;
        default:
            uart_puts("Unknown Exception Level\n");
            return;
    }    
}