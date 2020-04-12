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
#include "syscall.h"
#include "timer.h"

#define INPUT_BUFFER_SIZE 256

extern char __bss_end[];

static cmd_t default_cmd_arr[] = {
    {"exit", "exit shell", cmd_exit},
    {"help", "show all command", cmd_help},
    {"hello", "uart_print Hello World!", cmd_hello},
    {"reboot", "reboot system", cmd_reboot},
    {"timestamp", "system running time", cmd_timestamp},
    {"load_images", "load images from UART", cmd_load_images},
    {"exc", "svc #1", cmd_exc},
    {"brk", "brk #1", cmd_brk},
    {"irq", "start irq", cmd_irq},
    {NULL, NULL, cmd_not_find}};

int cmd_exit(int i)
{
    return -1;
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
    return 0;
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
    return 0;
}

int cmd_load_images(int i)
{
    uart_puts("Input kernel size: ");
    char cmd[INPUT_BUFFER_SIZE];
    memset(cmd, 0, INPUT_BUFFER_SIZE);

    uart_gets(cmd, INPUT_BUFFER_SIZE);
    int image_size = atoi(cmd);
    uart_puts("Kernel size is: ");
    uart_send_int(image_size);
    uart_send('\n');

    uart_puts("Input Kernel load address: ");
    memset(cmd, 0, INPUT_BUFFER_SIZE);
    uart_gets(cmd, INPUT_BUFFER_SIZE);

    int address = atoi(cmd);
    uart_puts("Kernel load address is: ");
    uart_send_hex(address);
    uart_send('\n');

    loadimg(address, image_size);
    // copy_kernel_and_load_images((void *)(long)address, image_size);
    // load_images((char *)(long)address, image_size);
}

int cmd_exc(int i)
{
    svc(1);
    return 0;
}

int cmd_brk(int i)
{
    brk(1);
    return 0;
}

static int timer_enable = 0;
int cmd_irq(int i)
{
    if (timer_enable)
    {
        _core_timer_disable();
        timer_enable = 0;
    }
    else
    {
        _core_timer_enable();
        timer_enable = 1;
    }
    return 0;
}

int cmd_not_find(int i)
{
    uart_print("Command not find, Try 'help'\n");
    return 0;
}

int sh_default_command(char *cmd)
{
    cmd_t *ptr = default_cmd_arr;
    while (ptr->name != NULL)
    {
        if (strcmp(ptr->name, cmd) == 0)
        {
            return ptr->func(0);
        }
        ptr++;
    }
    cmd_not_find(0);

    return 0;
}

int symbol()
{
    uart_send('\r');
    uart_send('>');
}

int pcsh()
{
    char cmd[INPUT_BUFFER_SIZE];
    int x = 0;

    // main loop
    while (x == 0)
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
        x = sh_default_command(cmd);

        // other program
    }
    uart_puts("Shell End\n");

    return 0;
}
