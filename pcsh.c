/*****
pcsh.c

pc means Po-Chun, NOT personal computer
*****/

#include "uart.h"
#include "common.h"
#include "string.h"
#include "system.h"

#define INPUT_BUFFER_SIZE 2048


int print(char *);

typedef struct cmd_t
{
    char *name;
    int (*func)(int);
} cmd_t;

int cmd_exit(int);
int cmd_help(int);
int cmd_hello(int);
int cmd_reboot(int);
int cmd_timestamp(int);
int cmd_not_find(int);
cmd_t default_cmd_arr[] = {
    {"exit", cmd_exit},
    {"help", cmd_help},
    {"hello", cmd_hello},
    {"reboot", cmd_reboot},
    {"timestamp", cmd_timestamp},
    {NULL, cmd_not_find}};

int cmd_exit(int i)
{
    return 0;
}

int cmd_help(int i)
{
    cmd_t *ptr = default_cmd_arr;
        
    print("Author: Hsu, Po-Chun(0856168)\n");
    print("Available command list:\n");
    while(ptr->name != NULL){
        print(ptr->name);
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

int cmd_not_find(int i)
{
    print("Command not find, Try 'help'\n");
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

    // replace '\n' with NULL
    buf[--i] == NULL;

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
        if (my_strcmp(ptr->name, cmd) == 0){
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

        // default command
        sh_default_command(cmd);

        // other program
    }

    return 0;
}
