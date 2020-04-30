#include "math.h"
#include "string.h"
#include "type.h"
#include "ctype.h"

int strcmp ( const char * s1, const char * s2 )
{
    int i;

    for ( i = 0; i < strlen(s1); i ++ )
    {
        if ( s1[i] != s2[i] )
        {
            return s1[i] - s2[i];
        }
    }

    return  s1[i] - s2[i];
}

void strset ( char * s1, int c, int size )
{
    int i;

    for ( i = 0; i < size; i ++)
        s1[i] = c;
}

int strlen ( const char * s )
{
    int i = 0;
    while ( 1 )
    {
        if ( *(s+i) == '\0' )
            break;
        i++;
    }

    return i;
}

char * strcpy ( char * destination, const char * source )
{
    int i;
    for ( i = 0; i < strlen ( source ) + 1; i++ )
    {
        destination[i] = source[i];
    }

    return destination;
}

char * strncpy ( char * destination, const char * source, int num )
{
    int i;  
    int len = num > strlen ( source ) ?  strlen ( source ) : num;
    
    for ( i = 0; i < len; i++ )
    {
        destination[i] = source[i];
    }

    return destination;
}

const char * strchr ( const char * str, int character )
{
    int i;

    for ( i = 0; i < strlen(str); i++ )
    {
        if ( str[i] == character )
            return str + i;
    }

    return NULL;
}

char *  itoa ( int value, char * str, int base )
{
    int str_count = 0;
    int new_value;
    const int origin_value = value;   
    unsigned int v = value;

    if ( origin_value < 0 && base == 10 )
    {
        str[str_count++] = '-';
        v = abs( value );
    }
        
    while ( v != 0 )
    {
        new_value = v % base;

        if ( new_value >= 0 && new_value <= 9 )
            str[str_count] = new_value + '0';
        else if ( new_value >= 10 && new_value <= 35 )
            str[str_count] = ( new_value - 10 ) + 'A';
        else
            return str;
        
        str_count ++;
        v /= base;
    }

    if ( str_count == 0 )
        str[str_count++] = '0';
    
    str[str_count] = '\0';

    reverse ( ( origin_value < 0 && base == 10 ) ? str + 1 : str );

    return str;
}

int atoi ( const char * str )
{
    int i = 0;

    while ( isspace ( *str ) )
        str ++;

    while ( isdigit(*str) )
    {
        i *= 10;
        i += ( (*str) - '0' );

        str ++;
    }
    
    return i;    
}

// https://www.geeksforgeeks.org/convert-floating-point-number-string/
void ftoa ( double n, char * res, int afterpoint ) 
{ 
    // Extract integer part 
    int ipart = (int)n; 
  
    // Extract floating part 
    double fpart = n - (double)ipart; 
  
    // convert integer part to string 
    itoa(ipart, res, 10); 
    int i = strlen(res);

    int p = 0;
  
    // check for display option after point 
    if (afterpoint != 0) { 
        
        res[i++] = '.'; 
        
        /* get the number that are wanted after point */
        fpart = fpart * pow(10, afterpoint); 

        /* padding necessary 0 here */
        while ( fpart < (double) pow ( 10, ( afterpoint - 1 - p ) ) )
        {
            res[i++] = '0';
            p++;
        }

        itoa(fpart, res + i, 10); 
    } 
}

void reverse ( char * s )
{
    int i;
    char temp;

    for ( i = 0; i < strlen(s) / 2; i++ ) 
    {
        temp = s[strlen(s) - i - 1];
        s[strlen(s) - i - 1] = s[i];
        s[i] = temp;
    }
}