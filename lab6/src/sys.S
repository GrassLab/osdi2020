#include "sys.h"

.globl get_taskid
get_taskid:
    mov    w8, #SYS_GET_TASKID_NUMBER
    svc    #0
    ret


.globl fork
fork:
    mov    w8, #SYS_FORK_NUMBER
    svc    #0
    ret

.globl exit
exit:
    mov w8, #SYS_EXIT_NUMBER    
    svc #0
    ret

.globl exec
exec:
    mov w8, #SYS_EXEC_NUMBER
    svc #0
    ret

.globl uart_read
uart_read:
    mov w8, SYS_UART_READ_NUMBER
    svc #0
    ret

.globl uart_write
uart_write:
    mov w8, SYS_UART_WRITE_NUMBER
    svc #0
    ret

.globl allocator_register
allocator_register:
    mov w8, SYS_REGISTER_ALLOCATOR
    svc #0
    ret

.globl allocator_alloc
allocator_alloc:
    mov w8, SYS_REQ_ALLOCATE
    svc #0
    ret

.globl allocator_free
allocator_free:
    mov w8, SYS_FREE_ALLOCATE
    svc #0
    ret

.globl malloc
malloc:
     mov x8, SYS_MALLOC
     svc #0
     ret

.globl free
free:
     mov x8, SYS_FREE
     svc #0
     ret
