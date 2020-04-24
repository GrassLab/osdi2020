#include "../include/gpio.h"

#include "../include/uart.h"
#include "../include/peripheral.h"

#include "../include/task.h"


extern unsigned int CORE_TIMER_COUNT;
extern unsigned int LOCAL_TIMER_COUNT;

void debug(){
    uart_puts("debugggggg!!\n");
}

void disable_irq() 
{
    asm volatile("msr daifset, 0xf");
}

void enable_irq() 
{
    asm volatile("msr daifclr, 0xf");
}

void set_HCR_EL2_IMO()
{
    //asm volatile("mov x0, #(1 << 4)");
    //asm volatile("msr hcr_el2, x0");
    asm volatile("mrs x0, hcr_el2");
    asm volatile("orr x0, x0, #16");
    asm volatile("msr hcr_el2, x0");
}

void core_timer_counter()
{
    uart_puts("Core Timer interrupt received ");
    uart_hex(CORE_TIMER_COUNT++);
    uart_puts("\n");
    timer_tick();  // set reschedule_flag
}

#define CORE0_TIMER_IRQ_CTRL (unsigned int* )0x40000040
#define EXPIRE_PERIOD 0xffffff

void core_timer_enable()
{
    unsigned int val = EXPIRE_PERIOD;
    asm volatile("msr cntp_tval_el0, %0" :: "r" (val));
    
    asm volatile("mov x0, 1");
	asm volatile("msr cntp_ctl_el0, x0");
    *CORE0_TIMER_IRQ_CTRL = 0x2;

}

void core_timer_enable_user()
{
    asm volatile("mov x0, #0\n" "svc #0\n");
}

void core_timer_handler()
{
    unsigned int val = EXPIRE_PERIOD;
    asm volatile("msr cntp_tval_el0, %0" :: "r" (val));
    core_timer_counter();
}


#define LOCAL_TIMER_CONTROL_REG (unsigned int* )0x40000034

void local_timer_init()
{
    unsigned int flag = 0x30000000; // enable timer and interrupt.
    unsigned int reload = 25000000;
    *LOCAL_TIMER_CONTROL_REG = (flag | reload);
}

void local_timer_counter()
{
    uart_puts("Local Timer interrupt received ");
    uart_hex(LOCAL_TIMER_COUNT++);
    uart_puts("\n");
}

#define LOCAL_TIMER_IRQ_CLR (unsigned int* )0x40000038
#define LOCAL_TIMER_RELOAD 0xc0000000//0xc0000000
void local_timer_handler()
{
    *LOCAL_TIMER_IRQ_CLR = LOCAL_TIMER_RELOAD; // clear interrupt and reload.
    local_timer_counter();
}

#define CORE0_INTERRUPT_SRC (unsigned int* )0x40000060
void interrupt_handler()
{
    unsigned int interrupt_src = *CORE0_INTERRUPT_SRC;
    char r;

    //if (arm & 0x80000) {
    if (interrupt_src & (1<<8)) {
        // uart interrupt
        if (*UART0_RIS & (1<<4)) {	// UARTRXINTR - uart_getc()
            // while (RX_FIFO_FULL) {
            //     // receive
            //     r = (char) (*UART0_DR);
            //     if (!QUEUE_FULL (read_buf)) {
            //         QUEUE_SET (read_buf, r);
            //         QUEUE_PUSH (read_buf);
            //     }
            // }
	        // *UART0_ICR = 1 << 4;
            while (RX_FIFO_FULL) { RX_BUF[0] = (char)(*UART0_DR); }
            *UART0_ICR = 1<<4; // Clears the UARTTXINTR interrupt.

	    } else if (*UART0_RIS & (1<<5))	{// UARTTXINTR - uart_send()
            
            // while (!QUEUE_EMPTY (write_buf)) {
            //     r = QUEUE_GET (write_buf);
            //     QUEUE_POP (write_buf);
            //     while (TX_FIFO_FULL) asm volatile ("nop");
            //     *UART0_DR = r;
            // }
            // *UART0_ICR = 1<<5;


	        while (!(TX_BUF[0] == 0)) {
                r = TX_BUF[0];
                TX_BUF[0] = 0;
                while (TX_FIFO_FULL) asm volatile ("nop");
                *UART0_DR = r;
            }
            *UART0_ICR = 1<<5; // Clears the UARTRTINTR interrupt.


            // while (TX_FIFO_FULL) asm volatile ("nop");
            // *UART0_ICR = 1<<5; // Clears the UARTRTINTR interrupt.
	    }
    }
    // local timer interrupt
    else if (interrupt_src & (1<<11)) {
        local_timer_handler();
    }
    // core timer interrupt
    else if (interrupt_src & (1<<1)) { // Physical Non Secure Timer Interrupt
        core_timer_handler();
    }
    else {
        uart_puts("interrupt_handler error.\n");
    }


    struct task* current = get_current();
    if (current->reschedule_flag == 1) {
        uart_puts("IRQ reschedule...\n");
        uart_hex(current->task_id);
        current->reschedule_flag = 0;
        schedule();
    }
}

#define UART_ENABLE_IRQ (unsigned int *)(MMIO_BASE + 0xb214)
void uart_irq_enable()
{
    *UART_ENABLE_IRQ = (1 << 25);
}

void timer_tick()
{
    struct task* current = get_current();
    --current->counter;
	if (current->counter == 0) {
	    current->reschedule_flag = 1;
    }
}
