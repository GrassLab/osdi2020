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

void command_help ()
{
    uart_puts("\n");
    uart_puts("Valid Command:\n");
    uart_puts("\thelp:\t\tprint this help.\n");
    uart_puts("\thello:\t\tprint \"Hello World!\".\n");
    uart_puts("\ttimestamp:\tprint current timestamp.\n");
    uart_puts("\tvc_base_addr:\tprint the VC core base address\n");
    uart_puts("\tboard_revision:\tprint the board revision.\n");
    uart_puts("\treboot:\t\treboot the raspi3.\n");
    uart_puts("\n");
}

void command_hello ()
{
    uart_puts("Hello World!\n");
}

void command_timestamp ()
{
    float timestamp;
    char str[20];

    timestamp = get_current_timestamp ( );

    ftoa( timestamp, str, 6);

    uart_send('[');
    uart_puts(str);
    uart_puts("]\n");
}

void command_not_found (char * s) 
{
    uart_puts("Err: command ");
    uart_puts(s);
    uart_puts(" not found, try <help>\n");
}

void command_reboot ()
{
    uart_puts("Start Rebooting...\n");

    *PM_WDOG = PM_PASSWORD | 0x20;
    *PM_RSTC = PM_PASSWORD | 100;
    
	while(1);
}

void command_board_revision()
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

void command_vc_base_addr()
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

void command_load_image ()
{
    int32_t size = 0;
    int32_t is_receive_success = 0;
    char output_buffer[20];
    char *load_address;
    char *address_counter;

    uart_puts("Start Loading Kernel Image...\n");
    uart_puts("Please input kernel load address in decimal.(defualt: 0x80000): ");
    load_address = (char *)((unsigned long)uart_getint());
    uart_puts("Please send kernel image from UART now:\n");

    wait_cycles(5000);

    if ( load_address == 0 )
        load_address = (char *)0x80000;

    do {

        // start signal to receive image
        uart_send(3);
        uart_send(3);
        uart_send(3);

        // read the kernel's size
        size  = uart_getc();
        size |= uart_getc() << 8;
        size |= uart_getc() << 16;
        size |= uart_getc() << 24;

        // send negative or positive acknowledge
        if(size<64 || size>1024*1024)
        {
            // size error
            uart_send('S');
            uart_send('E');            
            
            continue;
        }
        uart_send('O');
        uart_send('K');

        address_counter = load_address;
        
        // 從0x80000開始放
        while ( size-- ) 
        {
            *address_counter++ = uart_getc();
        }

        is_receive_success = 1;

        uart_puts("Kernel Loaded address: ");
        itohex_str( (uint64_t)load_address, sizeof(char *), output_buffer );
        uart_puts(output_buffer);
        uart_send('\n');

        wait_cycles(5000);

    } while ( !is_receive_success );
   
    // restore arguments and jump to the new kernel.
    asm volatile (
        // we must force an absolute address to branch to
        "mov x30, 0x80000;"
        "ret"
    );
}

void command_svc_exception_trap ()
{
    asm volatile ( "svc #1;" );
}

void command_brk_exception_trap ()
{
    asm volatile ( "brk #1;" );
}

void command_irq_exception_enable ()
{
    static int first = 1;
    if ( first )
        irq_setup();

    first = 0;

    irq_enable();
    uart_printf("[IRQ Enable]\n");

    core_timer_enable();
    uart_printf("[Core Timer Enable]\n");

    // local_timer_handler();
    local_timer_enable();
    uart_printf("[Local Timer Enable]\n");
}

void command_irq_exception_disable ()
{
    irq_disable();
    uart_printf("[IRQ Disable]\n");
}