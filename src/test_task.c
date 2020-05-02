
#include "lib/io.h"
#include "lib/task.h"
#include "lib/time.h"

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
    }
    else
    {
        printf ( "Task %d before exec, cnt address 0x%x, cnt value %d\n", get_pid ( ), &cnt, cnt );
    }
}