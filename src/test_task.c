#include "lib/io.h"
#include "lib/task.h"
#include "lib/time.h"

void foo ( )
{
    int tmp = 5;
    printf ( "Task %d after exec, tmp address 0x%x, tmp value %d\n", get_pid ( ), &tmp, tmp );
    exit ( 0 );
}

void test_task ( )
{
    int cnt = 1;
    int pid = fork ( );

    if ( pid == 0 )
    {
        fork ( );
        wait_msec ( 1000000 );
        fork ( );

        while ( cnt < 10 )
        {
            printf ( "Task id: %d, cnt address 0x%x, cnt: %d\n", get_pid ( ), &cnt, cnt );
            wait_msec ( 1000000 );
            ++cnt;
        }

        exit ( 0 );
        printf ( "Should not be printed\n" );
    }
    else
    {
        printf ( "Task %d before exec, cnt address 0x%x, cnt value %d\n", get_pid ( ), &cnt, cnt );
        exec ( foo );
    }
}