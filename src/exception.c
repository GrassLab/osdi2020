#include "../include/uart.h"
#include "../include/info.h"
#include "../include/interrupt.h"
#include "../include/syscall.h"
#include "../include/task.h"
#include "../include/exception.h"

void exception_not_implement()
{
    uart_puts("Exception not implement!\n"); 
    while (1);
}

void decode_exception(unsigned long esr, unsigned long elr, unsigned long spsr)
{
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
        case 0b110000: uart_puts("Breakpoint, lower EL"); break;
        case 0b110001: uart_puts("Breakpoint, same EL"); break;
        case 0b111100: uart_puts("Breakpoint instruction"); break;
        default: uart_puts("Unknown"); break;
    }
    
    // print EC and ISS
    uart_puts("\n\t Exception Class (EC): ");
    uart_hex(esr>>26);
    uart_puts("\n\t Instr Specific Syndrome (ISS): ");
    uart_hex(esr&0x1ffffff);

    // dump registers
    uart_puts("\n\t  ESR_ELx: 0x");
    uart_hex(esr>>16);
    uart_hex(esr);
    
    uart_puts("\n\t  ELR_ELx: 0x");
    uart_hex(elr>>16);
    uart_hex(elr);

    uart_puts("\n\t SPSR_ELx: 0x");
    uart_hex(spsr>>16);
    uart_hex(spsr);
    uart_puts("\n");
}
/**
 * common exception handler
 */
// follow aarch64 calling convention in system call
// syscall_x0 (x1, x2, x3)
void exception_handler(unsigned int trapframe)
{
    unsigned long esr, elr, spsr; 
    unsigned long sys_ret_val;
    //  check exception level
    int level = show_exception_level();
    switch(level) {
        case 1: 
            asm volatile ("mrs %0, esr_el1" : "=r"(esr));
            asm volatile ("mrs %0, elr_el1" : "=r"(elr));
            asm volatile ("mrs %0, spsr_el1" : "=r"(spsr));
            break;
        case 2: 
            asm volatile ("mrs %0, esr_el2" : "=r"(esr));
            asm volatile ("mrs %0, elr_el2" : "=r"(elr));
            asm volatile ("mrs %0, spsr_el2" : "=r"(spsr));
            break;
        default: 
            uart_puts("Unknown Exception level\n"); 
            return;
    }    

    if ((esr>>26)==0b010101) {
        if ((esr&0x1ffffff)==0) {
            unsigned int x8;
            x8 = *(unsigned int*)(trapframe+8*8);
            //asm volatile ("ldr %0, [%1, #8 * 8]" :"=r" (x8) :"r"(trapframe));
            el0_svc_handler(x8);
        }
    } 
    else {
        decode_exception(esr, elr, spsr);
    }

    // breakpoint jump out
    if (esr>>26==0b110000 || esr>>26==0b110001 || esr>>26==0b111100) {
        switch(level) {
            case 1:
                asm volatile ("msr elr_el1, %0" : : "r" (elr+4)); break;
            case 2:
                asm volatile ("msr elr_el2, %0" : : "r" (elr+4)); break;
            case 3:
                asm volatile ("msr elr_el3, %0" : : "r" (elr+4)); break;  
        }  
    }
}


void el0_svc_handler(unsigned int x8)
{
    unsigned long sys_ret_val = 0;
    struct task* current = get_current();
    uart_puts("syscall ID: ");
    uart_hex(x8);
    uart_puts(",  el0_svc_handler....\n");
    enable_irq(); 
    switch (x8) {
        case SYS_CORE_TIMER_IRQ_ENABLE:
            uart_puts("syscall core timer enable.\n");
            core_timer_enable(); 
            break;
        case SYS_GET_TASKID:
            // if(sys_get_taskid() == 0){
            //     uart_puts("sys_get_taskid success!\n");
            // } else {
            //     uart_puts("sys_get_taskid error!\n");
            // }
            uart_puts("sys_get_taskid success!\n");
            sys_ret_val = current->task_id;
            break;
    }
    disable_irq();
    asm volatile("str x0, [sp, #8 * 0]");
}

// int get_syscall_no()
// {
//     // get syscall number in x8
//     return 0;
// }

// // general interface for all system calls in kernel.
// // may handle trapframe related things here.
// int sys_get_taskid() 
// {
//     set_trap_ret(do_get_taskid());
//     return 0;
// }

// void set_trap_ret(unsigned long x0)
// {
//     // update trapframe
//     // save x0 to trapframe
//     return;
// }

// // real working space for a system call.
// // may also be called by other kernel functions.
// unsigned long do_get_taskid() 
// {
//     struct task* current = get_current();
//     return current->task_id;
// }