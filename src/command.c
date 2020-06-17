#include "mailbox.h"
#include "string.h"
#include "time.h"
#include "uart.h"

void input_buffer_overflow_message ( char cmd[] )
{
    uart_puts ( "Follow command: \"" );
    uart_puts ( cmd );
    uart_puts ( "\"... is too long to process.\n" );

    uart_puts ( "The maximum length of input is 64." );
}

void command_help ( )
{
    uart_puts ( "\n" );
    uart_puts ( "Valid Command:\n" );
    uart_puts ( "\thelp:\t\tprint this help.\n" );
    uart_puts ( "\thello:\t\tprint \"Hello World!\".\n" );
    uart_puts ( "\ttimestamp:\tprint current timestamp.\n" );
    uart_puts ( "\tvc_base_addr:\tprint the VC core base address\n" );
    uart_puts ( "\tboard_revision:\tprint the board revision.\n" );
    uart_puts ( "\treboot:\t\treboot the raspi3.\n" );
    uart_puts ( "\n" );
}

void command_hello ( )
{
    uart_puts ( "Hello World!\n" );
}

void command_timestamp ( )
{
    float timestamp;
    char str[20];

    timestamp = get_current_timestamp ( );

    ftoa ( timestamp, str, 6 );

    uart_send ( '[' );
    uart_puts ( str );
    uart_puts ( "]\n" );
}

void command_not_found ( char * s )
{
    uart_puts ( "Err: command " );
    uart_puts ( s );
    uart_puts ( " not found, try <help>\n" );
}

void command_reboot ( )
{
    uart_puts ( "Start Rebooting...\n" );

    *PM_WDOG = PM_PASSWORD | 0x20;
    *PM_RSTC = PM_PASSWORD | 100;

    while ( 1 )
        ;
}

void command_board_revision ( )
{
    char str[20];
    uint32_t board_revision = mbox_get_board_revision ( );

    uart_puts ( "Board Revision: " );
    if ( board_revision )
    {
        itohex_str ( board_revision, sizeof ( uint32_t ), str );
        uart_puts ( str );
        uart_puts ( "\n" );
    }
    else
    {
        uart_puts ( "Unable to query serial!\n" );
    }
}

void command_vc_base_addr ( )
{
    char str[20];
    uint64_t vc_base_addr = mbox_get_VC_base_addr ( );

    uart_puts ( "VC Core Memory:\n" );
    if ( vc_base_addr )
    {
        uart_puts ( "    - Base Address: " );
        itohex_str ( ( uint32_t ) ( vc_base_addr >> 32 ), sizeof ( uint32_t ), str );
        uart_puts ( str );
        uart_puts ( " (in bytes)\n" );

        uart_puts ( "    - Size: " );
        itohex_str ( ( uint32_t ) ( vc_base_addr & 0xffffffff ), sizeof ( uint32_t ), str );
        uart_puts ( str );
        uart_puts ( " (in bytes)\n" );
    }
    else
    {
        uart_puts ( "Unable to query serial!\n" );
    }
}