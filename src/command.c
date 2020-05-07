#include "lib/io.h"
#include "lib/string.h"
#include "lib/sys.h"
#include "lib/time.h"

void input_buffer_overflow_message ( char cmd[] )
{
    printf ( "Follow Command: \"%s\"... is too long to process.\n" );
    printf ( "The maximum length of input is 64." );
}

void command_help ( )
{
    printf ( "\n" );
    printf ( "Valid Command:\n" );
    printf ( "\thelp:\t\tprint this help.\n" );
    printf ( "\thello:\t\tprint \"Hello World!\".\n" );
    printf ( "\ttimestamp:\tprint current timestamp.\n" );
    printf ( "\tvc_base_addr:\tprint the VC core base address\n" );
    printf ( "\tboard_revision:\tprint the board revision.\n" );
    printf ( "\texc:\t\tTrap into EL1.\n" );
    printf ( "\thvc:\t\tTrap into EL2.\n" );
    printf ( "\ttimer:\t\tEnable timer interrupt of core timer and local timer.\n" );
    printf ( "\ttimer-stp:\tDisable timer interrupt of core timer and local timer.\n" );
    printf ( "\tirq:\tEnable interrupt.\n" );
    printf ( "\tirq:\tDisable interrupt.\n" );

    printf ( "\treboot:\t\treboot the raspi3.\n" );
    printf ( "\n" );
}

void command_hello ( )
{
    printf ( "Hello World!\n" );
}

void command_timestamp ( )
{
    double timestamp = get_time_stamp ( );
    printf ( "[%f]\n", timestamp );
}

void command_not_found ( char * s )
{
    printf ( "Err: command %s not found, try <help>\n" );
}
/*
void command_reboot ( )
{
    printf("Start Rebooting...\n");

    *PM_WDOG = PM_PASSWORD | 0x20;
    *PM_RSTC = PM_PASSWORD | 100;

    while(1);
}
*/

void command_board_revision ( )
{
    uint32_t board_revision = get_board_revision ( );

    printf ( "Board Revision: " );
    if ( board_revision )
    {
        printf ( "%x\n", board_revision );
    }
    else
    {
        printf ( "Unable to query serial!\n" );
    }
}

void command_vc_base_addr ( )
{
    uint64_t vc_base_addr = get_vc_base_addr ( );

    printf ( "VC Core Memory:\n" );
    if ( vc_base_addr )
    {
        printf ( "    - Base Address: %x (in bytes)\n", ( uint32_t ) ( vc_base_addr >> 32 ) );
        printf ( "    - Size: %x (in bytes)\n", ( uint32_t ) ( vc_base_addr & 0xffffffff ) );
    }
    else
    {
        printf ( "Unable to query serial!\n" );
    }
}

void command_svc_exception_trap ( )
{
    svc_test ( );
}

void command_timer_exception_enable ( )
{
    // enable irq in el1
    irq_enable ( );
    printf ( "[IRQ Enable]\n" );

    // enable core timer in el1
    core_timer_enable ( );
    printf ( "[Core Timer Enable]\n" );

    // enable local timer
    local_timer_enable ( );
    printf ( "[Local Timer Enable]\n" );
}

void command_timer_exception_disable ( )
{
    // core timer disable need to be done in el1
    core_timer_disable ( );
    printf ( "[Core Timer Disable]\n" );

    local_timer_disable ( );
    printf ( "[Local Timer Disable]\n" );
}

void command_irq_exception_enable ( )
{
    irq_enable ( );
    printf ( "[IRQ Enable]\n" );
}

void command_irq_exception_disable ( )
{
    irq_disable ( );
    printf ( "[IRQ Disable]\n" );
}