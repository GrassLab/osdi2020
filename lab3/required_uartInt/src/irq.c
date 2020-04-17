#include "timer.h"
#include "uart.h"
#include "irq.h"
#include "printf.h"

unsigned int core_jf = 1;
unsigned int local_jf = 1;

void irq(){
    unsigned int arm = *IRQ_BASIC_PENDING;
    unsigned int arm_local = *CORE0_INTERRUPT_SOURCE;

    char r;
    if(arm & 0x80000){
        // uart interrupt
        //uart_puts("uart interrupt\n");

        if(*UART0_RIS & 0x10){
            while(*UART0_FR & 0x40){
                r =  (char)(*UART0_DR);
                if(!QUEUE_FULL(read_buf)){
                    QUEUE_SET(read_buf, r);
                    QUEUE_PUSH(read_buf);
                }
            }
            *UART0_ICR = 1<<4;
        }else if(*UART0_RIS & 0x20){
            while(!QUEUE_EMPTY(write_buf)){
                r = QUEUE_GET(write_buf);
                QUEUE_POP(write_buf);
                while(*UART0_FR & 0x20){
                    asm volatile ("nop");
                }
                *UART0_DR = r;
            }
            *UART0_ICR = 2<<4;
        }
        
    }else if(arm_local & 0x800){
        // local timer interrupt
        handle_local_timer_irq();
    }else if(arm_local & 0x2){
        // core timer interrupt
        handle_core_timer_irq();
    }else{
        uart_puts("Exist a bug ><");
    }
    return;
}

int is_local_timer(){
    return *LOCAL_TIMER_CONTROL_REG & 0x80000000;
}

int is_core_timer(){
    unsigned long cntp_ctl_el0;
    asm volatile (
        "mrs %0, cntp_ctl_el0"
        :"=r" (cntp_ctl_el0)
    );
    return cntp_ctl_el0 & 4;
}

void handle_core_timer_irq(){
    core_timer_handler();
    printf("Arm core timer interrupt, jiffies %d\n", core_jf);
    core_jf += 1;
    if(core_jf == 10){
        core_timer_disable();
        core_jf = 1;
    }
    return;
}

void handle_local_timer_irq(){
    local_timer_handler();
    printf("Local timer interrupt, jiffies %d\n", local_jf);
    local_jf += 1;
    if(local_jf == 10){
        // disable local timer
        local_timer_disable();
        local_jf = 1;
    }
    return;
}

void init_uart_irq(){
    *ENABLE_IRQS_2 = (1 << 25);
    return;
}