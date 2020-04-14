#include "io.h"
#include "irq.h"
#include "timer.h"
#define nb8p(bytes, n, p) (((1 << n) - 1) & (bytes >> (p)))

void handle_uart_irq( void ){
	// There may be more than one byte in the FIFO.
	//while((*(AUX_MU_IIR_REG) & IIR_REG_REC_NON_EMPTY) ==
	//      IIR_REG_REC_NON_EMPTY) {
	//	uart_send(uart_recv()); 
	//}
}

#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int*)(0x40000034))
#define CORE0_INT_SRC           ((volatile unsigned int*)(0x40000060))

void irq_handler(){
    unsigned int irq = *(IRQ_PENDING_1);
    
    if(nb8p(*LOCAL_TIMER_CONTROL_REG, 1, 31))
        local_timer_handler();
    if((*CORE0_INT_SRC) & 0x2)
        core_timer_handler();
    // loop in case multiple interrupts have been raised
    while (irq) {
        // each bitflag is only 1 bit so we do not 
        // need to test equality against themself
        if (irq & SYSTEM_TIMER_IRQ_1) {
            sys_timer_handler();
            irq &= ~SYSTEM_TIMER_IRQ_1;
        } else if (irq & AUX_IRQ) {
            handle_uart_irq();
            irq &= ~AUX_IRQ;
        } else {
            printf("Unknown pending irq: %x\r\n", irq);
            irq = 0;
        }
    } 
}
