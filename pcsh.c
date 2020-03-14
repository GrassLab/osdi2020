/*****
pcsh.c

pc means Po-Chun, NOT personal computer
*****/

#include "uart.h"
#include "common.h"
#include "string.h"
#include "system.h"

#define INPUT_BUFFER_SIZE 2048

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
int cmd_not_find(int);
cmd_t default_cmd_arr[] = {
    {"exit", "exit shell", cmd_exit},
    {"help", "show all command", cmd_help},
    {"hello", "print Hello World!", cmd_hello},
    {"reboot", "reboot system", cmd_reboot},
    {"timestamp", "system running time", cmd_timestamp},
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
    while(ptr->name != NULL){
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

        if(c == 8 || c == 127){
            if(i > 0){
                buf[i--] = '\0';
                uart_send(8);
                uart_send(' ');
                uart_send(8);
            }
        }
        else{
            buf[i++] = c;
	    // ensure users can see what they type
            uart_send(c);
        }
    } while (c != '\n' && i < buf_size - 1);

    // replace '\n' with NULL
    buf[--i] == '\0';

    if (i == buf_size)
        return -1;

    return i;
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


        if(my_strcmp(cmd, "\n") == 0)
            continue;

        // default command
        sh_default_command(cmd);

        // other program
    }

    return 0;
}
