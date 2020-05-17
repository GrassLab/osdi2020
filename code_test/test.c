#include "lib.h"

int main()
{
    uart_send('=');
    uart_send('=');
    uart_send('=');
    uart_send('u');
    uart_send('s');
    uart_send('e');
    uart_send('r');
    uart_send(' ');
    uart_send('p');
    uart_send('r');
    uart_send('o');
    uart_send('g');
    uart_send('r');
    uart_send('a');
    uart_send('m');
    uart_send('=');
    uart_send('=');
    uart_send('=');
    uart_send('\n');

    return 0;
}

void print()
{
    char str[] = "\n=====\nuser program test\n=====\n";
    char *ptr = str;
    while (*ptr != '\0')
    {
        uart_send(*ptr);
        ptr++;
    }
}