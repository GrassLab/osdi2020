#include "lib.h"
#include "string.h"
#inlcude "uart.h"

char uart_getc()
{
    char r = uart_recv();
    /* convert carrige return to newline */
    return r == '\r' ? '\n' : r;
}

void uart_puts(char *s)
{
    while (*s)
    {
        /* convert newline to carrige return + newline */
        if (*s == '\n')
            uart_send('\r');
        uart_send(*s++);
    }
}

void uart_send_int(int n)
{
    char s[32];
    memset(s, 0, 32);
    itoa(n, s, 10);

    uart_puts(s);
}

void uart_send_hex(unsigned int d)
{
    unsigned int n;
    int c;
    // uart_puts("0x");
    for (c = 28; c >= 0; c -= 4)
    {
        // get highest tetrad
        n = (d >> c) & 0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n += n > 9 ? 0x37 : 0x30;
        uart_send(n);
    }
}


int uart_gets(char *buf, int buf_size)
{
    int i = 0;
    char c;

    do
    {
        c = uart_getc();

        c = c == '\r' ? '\n' : c;

        if (c == 8 || c == 127)
        {
            if (i > 0)
            {
                buf[i--] = '\0';
                uart_send(8);
                uart_send(' ');
                uart_send(8);
            }
        }
        else if (c != '\n')
        {
            buf[i++] = c;
            // ensure users can see what they type
            uart_send(c);
        }
    } while (c != '\n' && i < buf_size - 1);

    // replace '\n' with NULL
    if (i > 0)
        buf[i] == '\0';

    if (i == buf_size)
        return -1;

    return i;
}

void putc(void *p, char c)
{
    uart_send(c);
}
