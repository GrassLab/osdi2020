#include "kernel/exception/exception.h"

#include "lib/io.h"
#include "lib/time.h"

void idle ( )
{
    while ( 1 )
    {
        printf ("I am idle.\n");
        
        LAUNCH_SYS_CALL ( SYS_CALL_SCHEDULE );

        wait_msec(500000);
    }
}