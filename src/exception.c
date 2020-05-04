#include "../include/uart.h"
#include "../include/info.h"
#include "../include/interrupt.h"
#include "../include/syscall.h"
#include "../include/task.h"
#include "../include/exception.h"

void exception_not_implement(unsigned int offset)
{
    uart_puts("Exception not implement! offset is "); 
    uart_hex(offset);
    uart_puts("\n");
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
    while(1);
}
/**
 * common exception handler
 */
// follow aarch64 calling convention in system call
// syscall_x0 (x1, x2, x3)
void exception_handler(unsigned int trapframe)
{
    struct task* current = get_current();
    current->trapframe = trapframe;
    current->state = EXC_CONTEXT;
    // uart_puts("\r\n++++++++++  exception_handler begin  ++++++++++\n");
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

    // syscall - svc #0
    if ((esr>>26)==0b010101) {
        if ((esr&0x1ffffff)==0) {
            // decode_exception(esr, elr, spsr);

            unsigned long sys_ret_val = 0;
            //asm volatile ("ldr %0, [%1, #8 * 8]" :"=r" (x8) :"r"(trapframe));
            sys_ret_val = el0_svc_handler(trapframe);
            *(unsigned int*)trapframe = sys_ret_val;
        }
    } 
    else {
        uart_hex(current->user_context.sp_el0);
        uart_hex(current->user_context.spsr_el1);
        uart_hex(current->user_context.elr_el1);
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
    // uart_puts("++++++++++  exception_handler end  ++++++++++\n\r\n");
}


unsigned long el0_svc_handler(unsigned int trapframe)
{
    unsigned int x0; 
    unsigned int x8;  // syscall number
    x0 = *(unsigned int*)(trapframe+8*0);
    x8 = *(unsigned int*)(trapframe+8*8);
    unsigned long sys_ret_val = 0;
    struct task* current = get_current();
    // uart_puts("syscall ID: ");
    // uart_hex(x8);
    // uart_puts(",  el0_svc_handler....\n");
    // enable_irq(); 
    switch (x8) {
        case SYS_CORE_TIMER_IRQ_ENABLE:
            // uart_puts("syscall core timer enable.\n");
            core_timer_enable(); 
            break;
        case SYS_GET_TASKID:
            // if(sys_get_taskid() == 0){
            //     uart_puts("sys_get_taskid success!\n");
            // } else {
            //     uart_puts("sys_get_taskid error!\n");
            // }
            sys_ret_val = current->task_id;
            // uart_puts("sys_get_taskid success!\n");
            break;
        case SYS_EXEC:
            do_exec(x0);
            break;
        case SYS_FORK:
            sys_ret_val = fork();
            // uart_puts("fork_id: "); // child won't do this
            // uart_hex(sys_ret_val);
            // uart_puts("\n");
            break;
        case SYS_EXIT:
            sys_ret_val = exit(0);
            uart_puts("exit error!\n"); // exit should not come here, because schedule
            while(1);
            break;
        case SYS_UART_READ:
            sys_ret_val = uart_getc();
            break;
        case SYS_UART_WRITE:
            uart_puts(x0);
            break;
        default:
            // uart_puts("syscall not found\n");
            break;
    }

    // disable_irq();
    return sys_ret_val;
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