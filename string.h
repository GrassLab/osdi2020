#include "common.h"

int my_strcmp(const char *str1, const char *str2)
{

    if (*str1 == NULL || *str2 == NULL)
    {
        return *str1 - *str2;
    }

    while (*str1 != NULL || *str2 != NULL)
    {
        if (*str1 == NULL || *str2 == NULL)
            return *str1 - *str2;

        if (*str1 != *str2)
            return *str1 - *str2;
    }

    return 0;
}

void *my_memset(void *str, int c, size_t n)
{

    int i = 0;
    while (i < n)
    {
        *(char *)str = c;
        str += 1;
        ++i;
    }
    return str;
}
