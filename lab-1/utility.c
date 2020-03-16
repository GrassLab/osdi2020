#include "utility.h"

void memset(void* mem, int value, int size) {
    unsigned char *ptr = (unsigned char*) mem;
    while (size-- > 0)
        *ptr++ = value;
}