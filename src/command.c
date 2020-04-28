#include "kernel/exception/exception.h"
#include "kernel/exception/irq.h"
#include "kernel/exception/timer.h"
#include "kernel/peripherals/mailbox.h"
#include "kernel/peripherals/time.h"
#include "kernel/peripherals/uart.h"
#include "lib/string.h"

void input_buffer_overflow_message ( char cmd[] )
{
    uart_puts("Follow command: \"");
    uart_puts(cmd);
    uart_puts("\"... is too long to process.\n");

    uart_puts("The maximum length of input is 64.");
}

void command_help ( )
{
    uart_puts("\n");
    uart_puts("Valid Command:\n");
    uart_puts("\thelp:\t\tprint this help.\n");
    uart_puts("\thello:\t\tprint \"Hello World!\".\n");
    uart_puts("\ttimestamp:\tprint current timestamp.\n");
    uart_puts("\tvc_base_addr:\tprint the VC core base address\n");
    uart_puts("\tboard_revision:\tprint the board revision.\n");
    uart_puts("\texc:\t\tTrap into EL1.\n");
    uart_puts("\thvc:\t\tTrap into EL2.\n");
    uart_puts("\ttimer:\t\tEnable timer interrupt of core timer and local timer.\n");
    uart_puts("\ttimer-stp:\tDisable timer interrupt of core timer and local timer.\n");
    uart_puts("\tirq:\tEnable interrupt.\n");
    uart_puts("\tirq:\tDisable interrupt.\n");

    uart_puts("\treboot:\t\treboot the raspi3.\n");
    uart_puts("\n");
}

void command_hello ( )
{
    uart_puts("Hello World!\n");
}

void command_timestamp ( )
{
    LAUNCH_SYS_CALL ( SYS_CALL_PRINT_TIMESTAMP_EL0 );
}

void command_not_found ( char * s ) 
{
    uart_puts("Err: command ");
    uart_puts(s);
    uart_puts(" not found, try <help>\n");
}

void command_reboot ( )
{
    uart_puts("Start Rebooting...\n");

    *PM_WDOG = PM_PASSWORD | 0x20;
    *PM_RSTC = PM_PASSWORD | 100;
    
	while(1);
}

void command_board_revision ( )
{
    char str[20];  
    uint32_t board_revision = mbox_get_board_revision ();

    uart_puts("Board Revision: ");
    if ( board_revision )
    {
        itohex_str(board_revision, sizeof(uint32_t), str);
        uart_puts(str);
        uart_puts("\n");
    }
    else
    {
        uart_puts("Unable to query serial!\n");
    }
}

void command_vc_base_addr ( )
{
    char str[20];  
    uint64_t vc_base_addr = mbox_get_VC_base_addr ();

    uart_puts("VC Core Memory:\n");
    if ( vc_base_addr )
    {
        uart_puts("    - Base Address: ");
        itohex_str((uint32_t)(vc_base_addr >> 32), sizeof(uint32_t), str);
        uart_puts(str);
        uart_puts(" (in bytes)\n");


        uart_puts("    - Size: ");
        itohex_str((uint32_t)(vc_base_addr & 0xffffffff), sizeof(uint32_t), str);
        uart_puts(str);
        uart_puts(" (in bytes)\n");
    }
    else
    {
        uart_puts("Unable to query serial!\n");
    }
}

void command_svc_exception_trap ( )
{
    LAUNCH_SYS_CALL ( SYS_CALL_TEST_SVC );
}

void command_hvc_exception_trap ( )
{
    LAUNCH_SYS_CALL ( SYS_CALL_TEST_HVC );
}

void command_brk_exception_trap ( )
{
    asm volatile ( "brk #1;" );
}

void command_timer_exception_enable ( )
{
    // enable irq in el1
    LAUNCH_SYS_CALL ( SYS_CALL_IRQ_EL1_ENABLE );
    uart_printf("[IRQ Enable]\n");
    
    // enable core timer in el1
    LAUNCH_SYS_CALL ( SYS_CALL_CORE_TIMER_ENABLE );
    uart_printf("[Core Timer Enable]\n");

    // enable local timer
    local_timer_enable();
    uart_printf("[Local Timer Enable]\n");
}

void command_timer_exception_disable ( )
{   
    // core timer disable need to be done in el1
    LAUNCH_SYS_CALL ( SYS_CALL_CORE_TIMER_DISABLE );
    uart_printf("[Core Timer Disable]\n");

    local_timer_disable ();
    uart_printf("[Local Timer Disable]\n");
}

void command_irq_exception_enable ( )
{
    LAUNCH_SYS_CALL ( SYS_CALL_IRQ_EL1_ENABLE );
    uart_printf("[IRQ Enable]\n");
}

void command_irq_exception_disable ( )
{
    LAUNCH_SYS_CALL ( SYS_CALL_IRQ_EL1_DISABLE );
    uart_printf("[IRQ Disable]\n");
}