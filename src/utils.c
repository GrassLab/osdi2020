#include "type.h"
#include "device/uart.h"

size_t copynstr(const char *src, char *dst, size_t len)
{
    size_t i;
    for (i = 0; i < len; ++i)
    {
        if (*(src+i) == '\0')
            break;
        *(dst+i) = *(src+i);
    }
        
    *(dst+i) = '\0';

    return i;
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