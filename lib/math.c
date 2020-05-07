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

int abs ( int n )
{
    return n < 0 ? n * -1 : n;
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

uint64_t mask_0_in_bit ( uint64_t n, uint64_t d )
{
    uint64_t temp = 0;
    int i;
    int zero;

    for ( i = 15; i >= 0; i-- )
    {
        if ( i == d / 4 )
        {
            zero = d % 4;

            if ( zero == 0 )
                temp = ( temp << 4 ) | 0b1110;
            else if ( zero == 1 )
                temp = ( temp << 4 ) | 0b1101;
            else if ( zero == 2 )
                temp = ( temp << 4 ) | 0b1011;
            else
                temp = ( temp << 4 ) | 0b0111;
        }
        else
        {
            temp = ( temp << 4 ) | 0xF;
        }
    }

    n &= temp;

    return n;
}