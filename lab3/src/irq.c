#include "io.h"
#include "irq.h"
#include "uart.h"
#include "timer.h"
#define nb8p(bytes, n, p) (((1 << n) - 1) & (bytes >> (p)))

void handle_uart_irq( void ){
    // sample handle one
    char c = uart_recv();
    print("mini UART interrupt received. Received: ");
	uart_send(c);
    puts("");
    // sample handle two
	// There may be more than one byte in the FIFO.
	//while((*(AUX_MU_IIR_REG) & IIR_REG_REC_NON_EMPTY) ==
	//      IIR_REG_REC_NON_EMPTY) {
	//	uart_send(uart_recv()); 
	//}
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
    if(aux_irq & miniUART_IRQ){
        puts("there");
        //handle_uart_irq();
        // muart interrupt work here or...
    }
    while (irq) {
        if (irq & SYSTEM_TIMER_IRQ_1) {
            sys_timer_handler();
            irq &= ~SYSTEM_TIMER_IRQ_1;
        } else if (irq & AUX_IRQ_MSK) {
            // ... or work here
            puts("here");
            handle_uart_irq();
            irq &= ~AUX_IRQ_MSK;
        } else {
            printf("Unknown pending irq: %x\r\n", irq);
            irq = 0;
        }
    } 
}
