#include "kernel/exception/exception.h"

#include "io.h"
#include "task.h"
#include "time.h"

/* defined in task.S */
extern void schedule ( ); 

void idle ( )
{
    while ( 1 )
    {
        printf ("I am idle.\n");

        schedule ( );

        wait_msec(500000);
    }
}