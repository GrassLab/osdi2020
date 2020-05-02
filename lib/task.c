#include "task.h"

#include "kernel/exception/exception.h"

#include "io.h"
#include "time.h"

/* defined in task.S */
extern void schedule ( );

void idle ( )
{
    while ( 1 )
    {
        printf ( "I am idle.\n" );

        schedule ( );

        wait_msec ( 500000 );
    }
}