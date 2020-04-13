#include "lib/type.h"
#include "kernel/peripherals/uart.h"

#include "exception.h"
#include "timer.h"

void exec_controller ( SYS_CALL x0 )
{
    uint64_t elr, esr;

    asm volatile ( 
        "mrs     %0, ELR_EL1;"
        "mrs     %1, ESR_EL1;"
        : "=r" ( elr ), "=r" ( esr )
        :
    );
    
    switch ( x0 ) 
    {
        case TEST_SVC:
            print_exec_info ( elr, esr );
            break;
        case CORE_TIMER_ENABLE:
            core_timer_enable ();
            break;
        case CORE_TIMER_DISABLE:
            core_timer_enable ();
            break;
        default:
            print_exec_info ( elr, esr );
            break;
    }
}

void print_exec_info ( uint64_t elr, uint64_t esr )
{
    uint32_t ec, iss;

    ec = (((uint32_t)esr)>>26);
    // int il = (((uint32_t)esr)>>25) & 0x1 ;
    iss = (((uint32_t)esr)) & 0x00FFFFFF;

    uart_printf("Exception return address: %x\n", elr);
    uart_printf("Exception clss (EC): %x\n", ec);
    uart_printf("Instruction specific syndrome (ISS): %x\n", iss);
}

int get_current_el ()
{
    int el;
    asm volatile (
        "mrs    x0, CurrentEL;"
        "mov    %0, x0;" 
        : "=r" ( el )
    );

    el >>= 2;
    el &= 2;

    return el;
}