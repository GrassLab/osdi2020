#include "exception.h"

#include "kernel/peripherals/mailbox.h"
#include "kernel/peripherals/time.h"
#include "kernel/peripherals/uart.h"
#include "kernel/task/schedule.h"
#include "kernel/task/task.h"

#include "lib/type.h"

#include "irq.h"
#include "timer.h"

#define SYS_CALL_CASE( CASE, FUNC ) \
    case CASE:                      \
        FUNC;                       \
        break;

void exec_controller_el1 ( int sys_call_type )
{
    uint64_t elr, esr;
    thread_info_t * current_thread = get_current_task_el0 ( );
    uint32_t * argus               = (uint32_t *) ( current_thread->cpu_context ).x;

    asm volatile(
        "mrs     %0, ELR_EL1;"
        "mrs     %1, ESR_EL1;"
        : "=r"( elr ), "=r"( esr )
        : );

    switch ( sys_call_type )
    {
        // clang-format off
        SYS_CALL_CASE ( SYS_IRQ_EL1_ENABLE,         irq_el1_enable          ( ) )
        SYS_CALL_CASE ( SYS_IRQ_EL1_DISABLE,        irq_el1_disable         ( ) )
        SYS_CALL_CASE ( SYS_CORE_TIMER_ENABLE,      sys_core_timer_enable   ( ) )
        SYS_CALL_CASE ( SYS_CORE_TIMER_DISABLE,     sys_core_timer_disable  ( ) )
        SYS_CALL_CASE ( SYS_SCHEDULE,               sys_do_schedule         ( ) )
        SYS_CALL_CASE ( SYS_LOCAL_TIMER_ENABLE,     sys_local_timer_enable  ( ) )
        SYS_CALL_CASE ( SYS_LOCAL_TIMER_DISABLE,    sys_local_timer_disable ( ) )
        SYS_CALL_CASE ( SYS_CLEAR_ZOMBIE,           clear_zombie            ( ) )

        SYS_CALL_CASE ( SYS_TEST_SVC,               print_exec_info     ( EL1, elr, esr ) )
        SYS_CALL_CASE ( SYS_WAIT_MSEC,              sys_wait_msec       ( (unsigned int) ( argus[0] ) ) )
        SYS_CALL_CASE ( SYS_DO_EXEC,                sys_do_exec         ( ( void ( * ) ( ) ) ( intptr_t ) ( argus[0] ) ) )
        SYS_CALL_CASE ( SYS_DO_EXIT,                sys_do_exit         ( current_thread ) )
        SYS_CALL_CASE ( SYS_UART_WRITE,             uart_puts           ( (char *) ( intptr_t ) ( argus[0] ) ) )

        SYS_CALL_CASE ( SYS_GET_PID,                argus[0] = current_thread->task_id )
        SYS_CALL_CASE ( SYS_FORK,                   argus[0] = sys_duplicate_task  ( current_thread )->task_id )
        SYS_CALL_CASE ( SYS_GET_TIMESTAMP,          argus[0] = (double) sys_get_current_timestamp ( ) )
        SYS_CALL_CASE ( SYS_UART_READ,              argus[0] = (char) ( uart_getc ( ) ) )
        SYS_CALL_CASE ( SYS_GET_VC_BASE_ADDR,       argus[0] = ( uint64_t ) ( mbox_get_VC_base_addr ( ) ) )
        SYS_CALL_CASE ( SYS_GET_BOARD_REVISION,     argus[0] = ( uint32_t ) ( mbox_get_board_revision ( ) ) )
        
        default:
            print_exec_info ( EL1, elr, esr );
            break;
        // clang-format on      
    } 
}

void print_exec_info ( EL el, uint64_t elr, uint64_t esr )
{
    uint32_t ec, iss;

    ec  = ( ( (uint32_t) esr ) >> 26 );
    iss = ( ( (uint32_t) esr ) ) & 0x00FFFFFF;

    sys_printk ( "Current Exception Level: %d\n", el );
    sys_printk ( "Exception return address: %x\n", elr );
    sys_printk ( "Exception clss (EC): %x\n", ec );
    sys_printk ( "Instruction specific syndrome (ISS): %x\n", iss );
}

int get_current_el ( )
{
    int el;
    asm volatile(
        "mrs    x0, CurrentEL;"
        "mov    %0, x0;"
        : "=r"( el ) );

    el >>= 2;
    el &= 2;

    return el;
}