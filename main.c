#include "uart.h"
#include "string.h"

void deal_command ( char [64]);
void input_buffer_overflow_message ( char [64] );

void command_help ();
void command_hello ();
void command_timestamp ();

int main()
{
    // set up serial console
    uart_init();

    int buffer_counter = 0;
    char buffer[64];
    strset (buffer, 0, sizeof(char) * 64);   
    
    // say hello
    uart_puts("Hello World!\n");

    // echo everything back
    while(1)
    {
        int a = (int)uart_getc();
        
        // send the current input to display echo
        uart_send(a);

        // buffer input
        buffer[buffer_counter] = a;
        buffer_counter ++;

        if ( a == '\n' )
        {
            buffer[buffer_counter-1] = '\0';
            deal_command(buffer);
            
            buffer_counter = 0;
            strset (buffer, 0, sizeof(char) * 64);  

        } else if ( buffer_counter == 63 )
        {
            input_buffer_overflow_message(buffer);

            buffer_counter = 0;
            strset (buffer, 0, sizeof(char) * 64);  
        }
    }

    return 0;
}

void deal_command ( char cmd[64])
{
    if ( !strcmp(cmd, "help") ) 
    {
        command_help();
    }
    else if ( !strcmp(cmd, "hello") )
    {
        command_hello();
    }
    else if ( !strcmp(cmd, "timestamp") )
    {
        command_timestamp();
    }
}

void input_buffer_overflow_message ( char cmd[64] )
{
    uart_puts("Folled command: \"");
    uart_puts(cmd);
    uart_puts("\" is too long to process.\n");

    uart_puts("Input buffer will be reset.");
    uart_puts("The maximum length of input is 64.");
}

void command_help ()
{
    uart_puts("\n");
    uart_puts("Valid Command:\n");
    uart_puts("\thelp:\t\tprint this help.\n");
    uart_puts("\thello:\t\tprint \"Hello World!\".\n");
    uart_puts("\ttimestamp:\tprint current timestamp.\n");
    uart_puts("\n");
}

void command_hello ()
{
    uart_puts("Hello World!\n");
}

void command_timestamp ()
{
    uart_puts("[Current Timestamp]\n");

}