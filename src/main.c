
#include "lib/task.h"
#include "lib/time.h"
#include "lib/io.h"

#include "test_task.h"

int main ( )
{
    printf("This is main before test_task!!\n");

    do_exec ( test_task );
    
    while ( 1 )
    {
        printf( "Here is main.\n" );
        wait_msec ( 1000000 );
    }
    
    return 0;
}