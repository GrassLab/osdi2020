#include "uart.h"
#include "timer.h"
#include "queue.h"
#include "sched.h"

#define CORE0_IRQ_SOURCE              ((volatile unsigned int*)0x40000060)
#define IRQ_BASIC_PENDING             ((volatile unsigned int*)(MMIO_BASE+0x0000b200))
#define S_FRAME_SIZE 272

void set_trap_ret(int ret_val){
    char *p;
    p = kstack_pool[current->taskid] - S_FRAME_SIZE - 16; //kstack_pool[current->taskid] is stack top!
    *p = ret_val;
}

void exception_handler(unsigned long esr, unsigned long elr)
{
    int svc_type = esr&0x1FFFFFF;
    if (svc_type == 1){
        uart_puts("Exception return address: ");
        uart_hex(elr); uart_puts("\n");
        uart_puts("Exception class(EC): ");
        uart_hex((esr>>26)&0x3F); uart_puts("\n");
        uart_puts("Instruction specific syndrome (ISS): ");
        uart_hex(esr&0x1FFFFFF); uart_puts("\n");
    }
    else if(svc_type == 2){
        core_timer_enable();
        local_timer_init();
    }
    else if(svc_type == 3){
        core_timer_disable();
        local_timer_disable();
    }
    else if(svc_type == 4){
        // uart_puts("get task id\n");
        set_trap_ret(current -> taskid);
    }
}

void irq_handler()
{
    static unsigned int core_cnt = 0;
    // uart_puts("irq\n");
    if (*CORE0_IRQ_SOURCE & (1 << 1)){
        ++core_cnt; 
        uart_puts("core timer interrupt ");
        uart_dec(core_cnt); uart_puts("\n");
        set_core_timer_period();
        --current->counter;
        schedule();
    }
    else if (*CORE0_IRQ_SOURCE & (1 << 11)){
        uart_puts("local timer interrupt\n");
        set_local_timer_control();
    }
    else if(*IRQ_BASIC_PENDING & (1 << 19)){
        char r;
        if (*UART0_MIS & (1 << 4)){
            // uart_puts("uart0 recieve interrupt\n");
            uart_puts("  ");
            while (*UART0_FR & 0x40){
                r = (char) (*UART0_DR);
                while (QUEUE_FULL(read_buf, UARTBUF_SIZE))
                    asm volatile ("nop");
                ENQUEUE(read_buf, UARTBUF_SIZE, r);
            }
            *UART0_ICR = *UART0_ICR | (1 << 4);
            *UART0_IMSC = *UART0_IMSC & 0x2f;
        }
        else if (*UART0_MIS & (1 << 5)){
            // uart_puts("uart0 send interrupt\n");
            while (!(QUEUE_EMPTY(write_buf))){
                DEQUEUE(write_buf, UARTBUF_SIZE, r);
                while (*UART0_FR & 0x20)
                    asm volatile ("nop");
                *UART0_DR = r;
            }
            *UART0_ICR = *UART0_ICR | (1 << 5);
            *UART0_IMSC = *UART0_IMSC & 0x1f;
        }
    }
}

void dummy_handler()
{
    uart_puts("You shouldn't jump into this!\n");
    while(1);
}

void debug_handler(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far)
{
    // print out interruption type
    switch(type) {
        case 0: uart_puts("Synchronous"); break;
        case 1: uart_puts("IRQ"); break;
        case 2: uart_puts("FIQ"); break;
        case 3: uart_puts("SError"); break;
    }
    uart_puts(": ");
    // decode exception type (some, not all. See ARM DDI0487B_b chapter D10.2.28)
    switch(esr>>26) {
        case 0b000000: uart_puts("Unknown"); break;
        case 0b000001: uart_puts("Trapped WFI/WFE"); break;
        case 0b001110: uart_puts("Illegal execution"); break;
        case 0b010101: uart_puts("System call"); break;
        case 0b100000: uart_puts("Instruction abort, lower EL"); break;
        case 0b100001: uart_puts("Instruction abort, same EL"); break;
        case 0b100010: uart_puts("Instruction alignment fault"); break;
        case 0b100100: uart_puts("Data abort, lower EL"); break;
        case 0b100101: uart_puts("Data abort, same EL"); break;
        case 0b100110: uart_puts("Stack alignment fault"); break;
        case 0b101100: uart_puts("Floating point"); break;
        default: uart_puts("Unknown"); break;
    }
    // decode data abort cause
    if(esr>>26==0b100100 || esr>>26==0b100101) {
        uart_puts(", ");
        switch((esr>>2)&0x3) {
            case 0: uart_puts("Address size fault"); break;
            case 1: uart_puts("Translation fault"); break;
            case 2: uart_puts("Access flag fault"); break;
            case 3: uart_puts("Permission fault"); break;
        }
        switch(esr&0x3) {
            case 0: uart_puts(" at level 0"); break;
            case 1: uart_puts(" at level 1"); break;
            case 2: uart_puts(" at level 2"); break;
            case 3: uart_puts(" at level 3"); break;
        }
    }
    // dump registers
    uart_puts(":\n  ESR_EL1 ");
    uart_hex(esr>>32);
    uart_hex(esr);
    uart_puts(" ELR_EL1 ");
    uart_hex(elr>>32);
    uart_hex(elr);
    uart_puts("\n SPSR_EL1 ");
    uart_hex(spsr>>32);
    uart_hex(spsr);
    uart_puts(" FAR_EL1 ");
    uart_hex(far>>32);
    uart_hex(far);
    uart_puts("\n");
    // no return from exception for now
    while(1);
}