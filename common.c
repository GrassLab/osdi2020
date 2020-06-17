#include "common.h"
#include "printf.h"

void aFailed(char *file, int line)
{
    printf("Error in %s %d\n", file, line);
    while (1)
    {
    }
}