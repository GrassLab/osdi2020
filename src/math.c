#include "type.h"

int pow ( int base, int exponent )
{
    int result = 1;
    for ( ; exponent > 0; exponent-- )
    {
        result = result * base;
    }
    return result;
}

uint64_t round_to_next_power_2 ( uint64_t n )
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    n++;

    return n;
}

int log2 ( uint32_t n )
{
    return 31 - __builtin_clz ( n );
}

int find_first_0_in_bit ( uint64_t n )
{
    int i;

    for ( i = 0; i < 64; i++ )
    {
        if ( ( n & ( 0b1 << i ) ) == 0 )
        {
            return i;
        }
    }

    return -1;
}
