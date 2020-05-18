#include "lib.h"
#include "common.h"

void uart_send(unsigned int x1)
{
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x04\n"
                 "svc #0x80\n" ::"r"(x1));
}

char uart_recv()
{
    char x1;
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x05\n"
                 "svc #0x80\n" ::"r"(&x1));
    return x1;
}

void core_timer(int enable)
{
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x0\n"
                 "svc #0x80\n" ::"r"(enable));
}

double gettime()
{
    double t;
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x02\n"
                 "svc #0x80\n" ::"r"(&t));

    return t;
}

int get_taskid()
{
    int taskid;
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x20\n"
                 "svc #0x80\n" ::"r"(&taskid));
    return taskid;
}

int fork()
{
    int return_value = 0;
    /*
    asm volatile("mov x10, x0\n"
                 "mov x11, x1\n"
                 "mov x12, x2\n");
                 */
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x30\n"
                 "svc #0x80\n" ::"r"(&return_value));
    if (return_value == 0)
    {
        // thread_start();
        //asm volatile("mov x29, x10\n");
    }
    return return_value;
}

int exec(unsigned int func)
{
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x31\n"
                 "svc #0x80\n" ::"r"(func));
}

/*
void exit(int value)
{
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x32\n"
                 "svc #0x80\n" ::"r"(value));
}
*/

void reboot()
{
    asm volatile("mov x0, #0x33\n"
                 "svc #0x80\n");
}

void kill(int task_id, int signal)
{
    asm volatile("mov x1, %0\n"
                 "mov x2, %1\n"
                 "mov x0, #0x40\n"
                 "svc #0x80\n" ::"r"(task_id),
                 "r"(signal));
}

int remain_page_num(){
    int num;
    asm volatile("mov x1, %0\n"
                 "mov x0, #0x50\n"
                 "svc #0x80\n" ::"r"(&num));
    return num;
}

/*****
STRING.h
*****/
int strcmp(const char *s1, const char *s2)
{
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void * memcpy (void *dest, const void *src, size_t len)
{
  char *d = dest;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dest;
}


void *memset(void *str, int c, size_t n)
{

    size_t i = 0;
    while (i < n)
    {
        *(char *)str = c;
        str += 1;
        ++i;
    }
    return str;
}


/*****
UART.h
*****/
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