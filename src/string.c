#include "string.h"

#include "math.h"
#include "type.h"

int strcmp ( const char * s1, const char * s2 )
{
    int i;

    for ( i = 0; i < strlen ( s1 ); i++ )
    {
        if ( s1[i] != s2[i] )
        {
            return s1[i] - s2[i];
        }
    }

    return s1[i] - s2[i];
}

void strset ( char * s1, int c, int size )
{
    int i;

    for ( i = 0; i < size; i++ )
        s1[i] = c;
}

int strlen ( const char * s )
{
    int i = 0;
    while ( 1 )
    {
        if ( *( s + i ) == '\0' )
            break;
        i++;
    }

    return i;
}
#include "uart.h"
// https://www.geeksforgeeks.org/convert-floating-point-number-string/
void itoa ( int x, char str[], int d )
{
    int i = 0;
    while ( x )
    {
        str[i++] = ( x % 10 ) + '0';
        x        = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while ( i < d )
        str[i++] = '0';

    if ( i == 0 )
        str[i++] = '0';

    str[i] = '\0';
    reverse ( str );
}

// https://www.geeksforgeeks.org/convert-floating-point-number-string/
void ftoa ( float n, char * res, int afterpoint )
{
    // Extract integer part
    int ipart = (int) n;

    // Extract floating part
    float fpart = n - (float) ipart;

    // convert integer part to string
    itoa ( ipart, res, 0 );
    int i = strlen ( res );

    // check for display option after point
    if ( afterpoint != 0 )
    {
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow ( 10, afterpoint );

        itoa ( (int) fpart, res + i + 1, afterpoint );
    }
}

void itohex_str ( uint64_t d, int size, char * s )
{
    int i = 0;
    unsigned int n;
    int c;

    c    = size * 8;
    s[0] = '0';
    s[1] = 'x';

    for ( c = c - 4, i = 2; c >= 0; c -= 4, i++ )
    {
        // get highest tetrad
        n = ( d >> c ) & 0xF;

        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        if ( n > 9 && n < 16 )
            n += ( 'A' - 10 );
        else
            n += '0';

        s[i] = n;
    }

    s[i] = '\0';
}

void reverse ( char * s )
{
    int i;
    char temp;

    for ( i = 0; i < strlen ( s ) / 2; i++ )
    {
        temp                    = s[strlen ( s ) - i - 1];
        s[strlen ( s ) - i - 1] = s[i];
        s[i]                    = temp;
    }
}