/*****
pcsh.c

pc means Po-Chun, NOT personal computer
*****/

#include "uart.h"
#include "common.h"
#include "string.h"

#define INPUT_BUFFER_SIZE 2048

typedef struct cmd_t
{
    char *name;
    int (*func)(int);
} cmd_t;

int cmd_exit(int);
cmd_t default_cmd_arr[] = {
    {"exit", cmd_exit},
    {NULL, NULL}};

int cmd_exit(int i)
{
    return 0;
}

int gets(char *buf, int buf_size)
{
    int i = 0;
    char c;

    do
    {
        c = uart_getc();
        c = c == '\r' ? '\n' : c;
        buf[i++] = c;

        // ensure users can see what they type
        uart_send(c);
    } while (c != '\n' && i < buf_size - 1);

    buf[i] == NULL;

    if (i == buf_size)
        return -1;

    return i;
}

int print(char *s)
{
    uart_puts(s);
}

int sh_default_command(char *cmd)
{
    cmd_t *ptr = default_cmd_arr;
    while (ptr->name != NULL)
    {
        if (my_strcmp(ptr->name, cmd) == 0)
            ptr->func(0);
    }

    return -1;
}

int symbol()
{
    uart_send('>');
}

int pcsh()
{

    // main loop
    // input, output

    while (1)
    {
        // get command
        char cmd[INPUT_BUFFER_SIZE];
        my_memset(cmd, 0, INPUT_BUFFER_SIZE);

        symbol();
        gets(cmd, INPUT_BUFFER_SIZE);

        print(cmd);

        // default command
        sh_default_command(cmd);

        // other program
    }

    return 0;
}
