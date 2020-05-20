
#include "lib/io.h"
#include "lib/task.h"
#include "lib/time.h"

#include "shell.h"

int main ( )
{
    printf ( "This is main before test_task!!\n" );

    shell_start ( );

    return 0;
}