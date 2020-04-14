#include "timer.h"
#include "uart.h"
#include "irq.h"
#include "printf.h"

unsigned int core_jf = 1;
unsigned int local_jf = 1;

void irq(){
    if(is_core_timer()){
        handle_core_timer_irq();     
    }else if(is_local_timer()){
        handle_local_timer_irq();
    }else{
        uart_puts("Exist a bug ><");
    }
    
    // unsigned int first_level_irq = getRegister(CORE0_INTERRUPT_SOURCE);
    // unsigned int second_level_irq = getRegister(IRQ_PENDING_1);
    // if(first_level_irq == 2) {
    //     core_timer_handler();
    // }else{
    //     local_timer_handler();
    // }
    // core_timer_handler ();
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
        local_jf = 1;
    }
    return;
}