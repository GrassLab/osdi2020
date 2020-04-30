#include "lib/time.h"
#include "lib/io.h"

int main ( )
{
    char s[200];
    int i;
    while ( 1 )
    {
        gets(s);
        printf("%s Here is main.\n", s);
        wait_msec ( 1000000 );

        i = 5;
        while ( i -- )
        {
            printf("Here is main.\n");
            wait_msec ( 50000 );
        }
        
    }
    
    return 0;
}