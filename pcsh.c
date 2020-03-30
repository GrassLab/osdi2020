/*****
pcsh.c

pc means Po-Chun, NOT personal computer
*****/

#include "uart.h"
#include "common.h"
#include "string.h"
#include "system.h"
#include "bootloader.h"
#include "pcsh.h"

#define INPUT_BUFFER_SIZE 256

extern char __bss_end[];

static cmd_t default_cmd_arr[8] = {
        {"exit", "exit shell", cmd_exit},
        {"help", "show all command", cmd_help},
        {"hello", "uart_print Hello World!", cmd_hello},
        {"reboot", "reboot system", cmd_reboot},
        {"timestamp", "system running time", cmd_timestamp},
        {"load_images", "load images from UART", cmd_load_images},
        {NULL, NULL, cmd_not_find}};

int cmd_init()
{

}

int cmd_exit(int i)
{
    return 0;
}

int cmd_help(int i)
{
    cmd_t *ptr = default_cmd_arr;

    uart_print("Author: Hsu, Po-Chun(0856168)\n");
    uart_print("Available command list:\n");
    while (ptr->name != NULL)
    {
        uart_print(ptr->name);
        uart_print(": ");
        uart_print(ptr->detail);
        uart_print("\n");
        ++ptr;
    }
    uart_print("===\n");
    return 0;
}

int cmd_hello(int i)
{
    uart_print("Hello World!\n");
}

int cmd_reboot(int i)
{
    reset(10);
}

int cmd_timestamp(int i)
{
    float t = gettime();
    uart_send_float(t, 4);
    uart_send('\n');
}

int cmd_load_images(int i)
{
    uart_puts("Input kernel size: ");
    char cmd[INPUT_BUFFER_SIZE];
    // memset(cmd, 0, INPUT_BUFFER_SIZE);

    uart_gets(cmd, INPUT_BUFFER_SIZE);
    int image_size = atoi(cmd);
    uart_puts("Kernel size is: ");
    uart_send_int(image_size);
    uart_send('\n');

    uart_puts("Input Kernel load address: ");
    uart_gets(cmd, INPUT_BUFFER_SIZE);
    int address = atoi(cmd);
    uart_puts("Kernel load address is: ");
    uart_send_hex(address);
    uart_send('\n');

    // copy_kernel_and_load_images((void *)(long)address, image_size);
    load_images((char *)(long)address, image_size);
}

int cmd_not_find(int i)
{
    uart_print("Command not find, Try 'help'\n");
}

int sh_default_command(char *cmd)
{
    cmd_t *ptr = default_cmd_arr;
    while (ptr->name != NULL)
    {
        if (strcmp(ptr->name, cmd) == 0)
        {
            ptr->func(0);
            return 0;
        }
        ptr++;
    }
    ptr->func(0);

    return -1;
}

int symbol()
{
    uart_send('\r');
    uart_send('>');
}

int pcsh()
{
    char cmd[INPUT_BUFFER_SIZE];
    
    // main loop
    while (1)
    {
        symbol();

        // get command
        memset(cmd, 0, INPUT_BUFFER_SIZE);
        uart_gets(cmd, INPUT_BUFFER_SIZE);

        uart_send('\r');
        uart_send('\n');

        if (strcmp(cmd, "") == 0)
            continue;

        // default command
        sh_default_command(cmd);


        // other program
    }
    uart_puts("Shell End\n");

    return 0;
}
