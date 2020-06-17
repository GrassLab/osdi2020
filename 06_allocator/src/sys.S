#include "sys.h"

.global uart_read
uart_read:
    mov x8, #SYS_UART_READ
    svc #0
    ret

.global uart_write
uart_write:
    mov x8, #SYS_UART_WRITE
    svc #0
    ret

.global exec
exec:
    mov w8, #SYS_EXEC
    svc #0
    ret

.global fork
fork:
    mov w8, #SYS_FORK
    svc #0
    ret

.global exit
exit:
    mov w8, #SYS_EXIT
    svc #0
    ret

.global sys_allocator_register
sys_allocator_register:
    mov w8, #SYS_ALLOCATOR_REGISTER
    svc #0
    ret

.global sys_allocator_alloc
sys_allocator_alloc:
    mov w8, #SYS_ALLOCATOR_ALLOC
    svc #0

.global sys_allocator_free
sys_allocator_free:
    mov w8, #SYS_ALLOCATOR_FREE
    svc #0

.global sys_allocator_unregister
sys_allocator_unregister:
    mov w8, #SYS_ALLOCATOR_UNREGISTER
    svc #0
