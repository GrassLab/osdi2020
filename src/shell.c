#include "kernel/peripherals/uart.h"
#include "lib/string.h"

#include "command.h"
#include "shell.h"

void shell_start () 
{
    int buffer_counter = 0;
    char input_char;
    char buffer[MAX_BUFFER_LEN];
    enum SPECIAL_CHARACTER input_parse;

    strset (buffer, 0, MAX_BUFFER_LEN);   

    // new line head
    uart_puts("# ");

    // read input
    while(1)
    {
        input_char = uart_getc();

        input_parse = parse ( input_char );

        command_controller ( input_parse, input_char, buffer, &buffer_counter);
    }
}

enum SPECIAL_CHARACTER parse ( char c )
{
    if ( !(c < 128 && c >= 0) )
        return UNKNOWN;

    if ( c == BACK_SPACE )
        return BACK_SPACE;
    else if ( c == LINE_FEED || c == CARRIAGE_RETURN )
        return NEW_LINE;
    else
        return REGULAR_INPUT;    
}

void command_controller ( enum SPECIAL_CHARACTER input_parse, char c, char buffer[], int * counter )
{
    if ( input_parse == UNKNOWN )
        return;
    
    // Special key
    if ( input_parse == BACK_SPACE )
    {
        if (  (*counter) > 0 )
            (*counter) --;
        
        uart_send(c);
        uart_send(' ');
        uart_send(c);
    }
    else if ( input_parse == NEW_LINE )
    {
        uart_send(c);

        if ( (*counter) == MAX_BUFFER_LEN ) 
        {
            input_buffer_overflow_message(buffer);
        }
        else 
        {
            buffer[(*counter)] = '\0';

            if      ( !strcmp(buffer, "help"            ) ) command_help();
            else if ( !strcmp(buffer, "hello"           ) ) command_hello();
            else if ( !strcmp(buffer, "timestamp"       ) ) command_timestamp();
            else if ( !strcmp(buffer, "reboot"          ) ) command_reboot();
            else if ( !strcmp(buffer, "vc_base_addr"    ) ) command_vc_base_addr();
            else if ( !strcmp(buffer, "board_revision"  ) ) command_board_revision();
            else if ( !strcmp(buffer, "exc"             ) ) command_svc_exception_trap();
            else if ( !strcmp(buffer, "hvc"             ) ) command_hvc_exception_trap();
            else if ( !strcmp(buffer, "exc_brk"         ) ) command_brk_exception_trap();
            else if ( !strcmp(buffer, "timer"           ) ) command_timer_exception_enable();
            else if ( !strcmp(buffer, "timer-stp"       ) ) command_timer_exception_disable();
            else if ( !strcmp(buffer, "irq"             ) ) command_irq_exception_enable();
            else if ( !strcmp(buffer, "irq-stp"         ) ) command_irq_exception_disable();
            else                                            command_not_found(buffer);
        }
            
        (*counter) = 0;
        strset (buffer, 0, MAX_BUFFER_LEN); 

        // new line head;
        uart_puts("# ");
    }
    else if ( input_parse == REGULAR_INPUT )
    {
        uart_send(c);

        if ( *counter < MAX_BUFFER_LEN)
        {
            buffer[*counter] = c;
            (*counter) ++;
        }
    }
}