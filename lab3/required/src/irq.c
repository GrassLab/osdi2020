#include "timer.h"
#include "uart.h"
#include "irq.h"
#include "printf.h"

unsigned int core_jf = 1;
unsigned int local_jf = 1;

void irq(){
    unsigned int arm = *IRQ_BASIC_PENDING;
    unsigned int arm_local = *CORE0_INTERRUPT_SOURCE;
    
    if(arm_local & 0x800){
        // local timer interrupt
        handle_local_timer_irq();
    }else if(arm_local & 0x2){
        // core timer interrupt
        handle_core_timer_irq();
    }else{
        uart_puts("Exist a bug ><");
    }
    return;
    
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
    printf("Arm core timer interrupt, jiffies %d\n", core_jf);
    core_timer_handler();
    core_jf += 1;
    if(core_jf == 4){
        core_timer_disable();
        core_jf = 1;
    }
    return;
}

void handle_local_timer_irq(){
    printf("Local timer interrupt, jiffies %d\n", local_jf);
    local_timer_handler();
    local_jf += 1;
    if(local_jf == 4){
        local_timer_disable();
        local_jf = 1;
    }
    return;
}