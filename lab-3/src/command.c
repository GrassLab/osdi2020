#include "command.h"
#include "string.h"
#include "uart.h"
#include "utility.h"
#include "mailbox.h"
#include "bootloader.h"
#include "framebuffer.h"

#define command_size 9
Command command_list[command_size] = {
    {.name = "help", .description = "help: help", .function = command_help},
    {.name = "reboot", .description = "reboot: reboot pi", .function = command_reboot},
    {.name = "timestamp", .description = "timestamp: get current timestamp", .function = command_timestamp},
    {.name = "loadimg", .description = "loadimg: load kernel image", .function = command_load_image},
    {.name = "info", .description = "info: get hardware information", .function = command_hardware_info},
    {.name = "uartclock", .description = "uartclock: get uart clock", .function = command_get_uart_clock},
    {.name = "picture", .description = "picture: print a picture", .function = command_picture},
    {.name = "exc", .description = "exc: execute exception handler", .function = command_exc},
    {.name = "irq", .description = "irq: enable timer interrupt", .function = command_irq}
};

static void commandSwitcher(char *comm)
{
    for(int i = 0; i < command_size; i++) {
        if (strEqual(comm, command_list[i].name)) {
            command_list[i].function();
            return;
        }
    }
    if (strlen(comm) != 0){
        uart_puts("Error: command ");
        uart_puts(comm);
        uart_puts(" not found\n");
    }
}

void interative()
{
    uart_puts("# ");
    char command[1024] = {0};
    int commandIndex = 0;
    int isbooting = 1;
    while (1) {
        // if (uart_buffer_empty()) {
        //     continue;
        // }
        char c = uart_getc();
        if (isbooting) {
            isbooting = 0;
            continue;
        }
        if (c == '\n') {
            uart_puts("\n");
            commandSwitcher(command);
            uart_puts("# ");
            memset(command, 0, sizeof(command));
            commandIndex = 0;
        } else {
            uart_send(c);
            command[commandIndex] = c;
            commandIndex ++;
        }
    }
}

void command_help()
{
    for(int i = 0; i < command_size; i++) {
        uart_puts(command_list[i].description);
        uart_puts("\n");
    }
}

void command_reboot()
{
    reboot(0);
}

void command_timestamp()
{
    getTimestamp();
}

void command_hardware_info()
{
    uart_puts("Serial number: : ");
    print_serial_number();
    uart_puts("\nBoard revision: ");
    print_board_revision();
    uart_puts("\nVC memory base address: ");
    print_vc_base_address();
    uart_puts("\nVC memory size: ");
    print_vc_size();
    uart_puts("\n");
}

void command_load_image()
{
    uart_puts("Please input kernel load address: (default: 0x80000): ");
    char address[1024] = {0};
    uart_get_string(address);

    char size[1024] = {0};
    uart_puts("\nPlease input kernel size: ");
    uart_get_string(size);
    uart_puts("\n");
    load_image(strToNum(address, 16), strToInt(size));
}

void command_get_uart_clock() {
    print_uart_clock();
}

void command_picture() 
{
    uart_puts("Start showing image.\n");
    lfb_init();
    lfb_showpicture();
}

void command_exc() 
{
    asm volatile("svc #1");
}

void command_irq()
{
    enable_irq();
    local_timer_init();
    core_timer_init();
    // sys_timer_init();
}