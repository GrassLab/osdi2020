#include "type.h"
#include "device/uart.h"

void copystr(const char *src, char *dst)
{
    char *p;
    for (p = src; *p != '\0'; ++p, ++dst)
        *dst = *p;
    *dst = *p;
}

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

uint32_t atoi(char *s)
{
    uint32_t sum = 0;
    for (uint32_t i = 0; s[i] != '\0'; i++)
    {
        sum = sum * 10 + s[i] - '0';
    }
    return sum;
}