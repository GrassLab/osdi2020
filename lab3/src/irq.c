#include "io.h"
#include "irq.h"
#include "uart.h"
#include "util.h"
#include "timer.h"
#include "shell.h"

#define nb8p(bytes, n, p) (((1 << n) - 1) & (bytes >> (p)))

void handle_uart_irq( void ){
	// There may be more than one byte in the FIFO.
	while((*(AUX_MU_IIR_REG) & IIR_REG_REC_NON_EMPTY) ==
	      IIR_REG_REC_NON_EMPTY) {
        shell_stuff_line(uart_recv());
	}
}

#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int*)(0x40000034))
#define CORE0_INT_SRC           ((volatile unsigned int*)(0x40000060))

#define miniUART_IRQ		(1 << 0)
void irq_handler(){
    unsigned int irq = *(IRQ_PENDING_1);
	unsigned int aux_irq = *(AUX_IRQ);
    
    if(nb8p(*LOCAL_TIMER_CONTROL_REG, 1, 31))
        local_timer_handler();
    if((*CORE0_INT_SRC) & 0x2)
        core_timer_handler();
    
    while (irq) {
        if (irq & SYSTEM_TIMER_IRQ_1) {
            sys_timer_handler();
            irq &= ~SYSTEM_TIMER_IRQ_1;
        }
        else if (irq & AUX_IRQ_MSK) {
        //else if(aux_irq & miniUART_IRQ)
            handle_uart_irq();
            irq &= ~AUX_IRQ_MSK;
        }
        else {
            printf("Unknown pending irq: %x\r\n", irq);
            irq = 0;
        }
    } 

    delay(50000000);
}

void init_irq(){

#if defined(RUN_ON_EL2)
    unsigned long hcr;
    __asm__ volatile ("mrs %0, hcr_el2" : "=r"(hcr));
    hcr |= 1 << 4; //IMO
    __asm__ volatile ("msr hcr_el2, %0" :: "r"(hcr));
#endif
    *ENABLE_IRQS_1 = (SYSTEM_TIMER_IRQ_1 | AUX_IRQ_MSK);
    __asm__ volatile ("msr  daifclr, #2");
}

void enable_irq(){
    *ENABLE_IRQS_1 = (SYSTEM_TIMER_IRQ_1 | AUX_IRQ_MSK);
    __asm__ volatile ("msr daifclr, #0xf");
}

void disable_irq(){
    __asm__ volatile ("msr daifset, #0xf");
}
