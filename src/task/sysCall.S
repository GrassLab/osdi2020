#include "task/sysCallTable.h"

.globl fork
fork:
    mov x8, FORK
    svc #0
    ret

.globl exec
exec:
    mov x8, EXEC
    svc #0
    ret

.globl exit
exit:
    mov x8, EXIT
    svc #0
    ret