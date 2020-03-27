/*****
pcsh.c

pc means Po-Chun, NOT personal computer
*****/

#include "uart.h"
#include "common.h"
#include "string.h"
#include "system.h"
#include "bootloader.h"

#define INPUT_BUFFER_SIZE 2048

extern char __bss_end[];

typedef struct cmd_t
{
    char *name;
    char *detail;
    int (*func)(int);
} cmd_t;

int cmd_exit(int);
int cmd_help(int);
int cmd_hello(int);
int cmd_reboot(int);
int cmd_timestamp(int);
int cmd_load_images(int);
int cmd_not_find(int);
cmd_t default_cmd_arr[] = {
    {"exit", "exit shell", cmd_exit},
    {"help", "show all command", cmd_help},
    {"hello", "print Hello World!", cmd_hello},
    {"reboot", "reboot system", cmd_reboot},
    {"timestamp", "system running time", cmd_timestamp},
    {"load_images", "load images from UART", cmd_load_images},
    {NULL, NULL, cmd_not_find}};

int cmd_exit(int i)
{
    return 0;
}

int cmd_help(int i)
{
    cmd_t *ptr = default_cmd_arr;

    print("Author: Hsu, Po-Chun(0856168)\n");
    print("Available command list:\n");
    while (ptr->name != NULL)
    {
        print(ptr->name);
        print(": ");
        print(ptr->detail);
        print("\n");
        ++ptr;
    }
    return 0;
}

int cmd_hello(int i)
{
    print("Hello World!\n");
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
    my_memset(cmd, 0, INPUT_BUFFER_SIZE);

    gets(cmd, INPUT_BUFFER_SIZE);
    int image_size = atoi(cmd);
    uart_puts("Kernel size is: ");
    uart_send_int(image_size);
    uart_send('\n');

    uart_puts("Input Kernel load address: ");
    gets(cmd, INPUT_BUFFER_SIZE);
    int address = atoi(cmd);
    uart_puts("Kernel load address is: ");
    uart_send_hex(address);
    uart_send('\n');

    // load_images(image_size);
    uart_puts("Start Copy Kernel\n");
    copy_kernel_and_load_images((char *)address, image_size);
}

int cmd_not_find(int i)
{
    print("Command not find, Try 'help'\n");
}

int sh_default_command(char *cmd)
{
    cmd_t *ptr = default_cmd_arr;
    while (ptr->name != NULL)
    {
        if (my_strcmp(ptr->name, cmd) == 0)
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

    // main loop
    while (1)
    {
        // get command
        char cmd[INPUT_BUFFER_SIZE];
        my_memset(cmd, 0, INPUT_BUFFER_SIZE);

        symbol();
        gets(cmd, INPUT_BUFFER_SIZE);

        uart_send('\r');

        if (my_strcmp(cmd, "\n") == 0)
            continue;

        // default command
        sh_default_command(cmd);

        // other program
    }

    return 0;
}
