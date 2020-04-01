#include "type.h"
#include "uart.h"

const static unsigned int MAX_BUFFER_SIZE = 512;

bool strcmp(const char *s1, const char *s2)
{
    for (; *s1 != '\0' && *s2 != '\0'; s1++, s2++)
    {
        if ((*s1 - *s2) != 0)
            return false;
    }

    if (*s1 == '\0' && *s2 == '\0')
    {
        return true;
    }
    else
    {
        return false;
    }
}

void printInt(int i)
{
    char buf[MAX_BUFFER_SIZE];
    int buf_ptr = 0;
    while (i > 0)
    {
        buf[buf_ptr++] = (i % 10) + '0';
        i = i / 10;
    }
    buf[buf_ptr] = '\0';
    for (int e = buf_ptr - 1, s = 0, half = (buf_ptr - 1) / 2; e > half; --e, ++s)
    {
        char tmp = buf[s];
        buf[s] = buf[e];
        buf[e] = tmp;
    }

    uartPuts(buf);
}

int atoi(char *s)
{
    int sum = 0;
    for (int i = 0; s[i] != '\0'; i++)
    {
        sum = sum * 10 + s[i] - '0';
    }
    return sum;
}