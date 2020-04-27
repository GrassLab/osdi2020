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
#include "timer.h"

#include "syscall.h"

#include "printf.h"

#define INPUT_BUFFER_SIZE 64

extern char __bss_end[];

static cmd_t default_cmd_arr[] = {
    {"exit", "exit shell", cmd_exit},
    {"help", "show all command", cmd_help},
    {"hello", "print Hello World!", cmd_hello},
    {"reboot", "reboot system", cmd_reboot},
    {"timestamp", "system running time", cmd_timestamp},
    {"load_images", "load images from UART", cmd_load_images},
    {"exc", "svc #1", cmd_exc},
    {"brk", "brk #1", cmd_brk},
    {"irq", "start irq", cmd_irq},
    {"delay", "delay and print ....", cmd_delay},
    {"delay_x", "delay and print ....(in exception)", cmd_delay_without_bottom_half},
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
    double t = gettime();
    uart_send_float((float)t, 4);
    //uart_send('\n');
}

int cmd_load_images(int i)
{
    loadimg();
    /*
    asm volatile("mov x0, #3");
    asm volatile("svc #0x80");
    */
}

int cmd_exc(int i)
{
    asm volatile("svc #1");
    return 0;
}

int cmd_brk(int i)
{
    asm volatile("brk #1");
    return 0;
}

static int timer_enable = 0;
int cmd_irq(int i)
{
    if (timer_enable)
    {
        // core timer
        uart_puts("timer disable\n");
        //_core_timer_disable();
        asm volatile("mov x0, #0");
        asm volatile("mov x1, #0");
        asm volatile("svc #0x80");

        // local timer
        asm volatile("mov x0, #1");
        asm volatile("mov x1, #0");
        asm volatile("svc #0x80");
        timer_enable = 0;
    }
    else
    {
        uart_puts("timer enable\n");
        //_core_timer_enable();
        asm volatile("mov x0, #0");
        asm volatile("mov x1, #1");
        asm volatile("svc #0x80");

        // local timer
        asm volatile("mov x0, #1");
        asm volatile("mov x1, #1");
        asm volatile("svc #0x80");

        timer_enable = 1;
    }
    return 0;
}

int cmd_delay(int i)
{
    asm volatile("mov x0, #0x100");
    asm volatile("svc #0x80");
    return 0;
}

int cmd_delay_without_bottom_half(int i)
{
    asm volatile("mov x0, #0x101");
    asm volatile("svc #0x80");
    return 0;
}

int cmd_not_find(int i)
{
    printf("Command not find, Try 'help'\n");
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

    printf("\"%s\", ", cmd);
    cmd_not_find(0);

    return 0;
}

int symbol()
{
    printf("\r>");
}

void pcsh()
{
    char cmd[INPUT_BUFFER_SIZE];
    int x = 0;

    printf("Shell start\n");
    // main loop
    while (x != -1)
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
    }
}
