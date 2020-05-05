#include "sys.h"
.globl sync_call_time
sync_call_time:
    mov w8, #1
    svc #0
    ret

.globl sync_call_clone
sync_call_clone:
    /* save register for child */
    mov x10, x0 /*fn */
    mov x11, x1 /*arg */
    mov w8, #SYS_CLONE
    svc #0
    
    cmp	x0, #0
    beq	thread_start
    ret
thread_start:
    mov	x29, 0 //???
    /* Pick the function arg and execute.  */
    mov x0, x11
    blr x10
    mov	w8, #SYS_EXIT_NUMBER
    svc	#0
    ret


.globl exit
exit:
    mov w8, #SYS_EXIT_NUMBER
    svc #0
    ret

.globl sync_call_uart_read
sync_call_uart_read:
    // x0 buffer address
    // x1 read size
    mov w8, #SYS_UART_READ
    svc #0
    // x0 return size
    ret

.globl sync_call_uart_write
sync_call_uart_write:
    // x0 buffer address
    // x1 write size
    mov w8, #SYS_UART_WRITE
    svc #0
    // x0 return size
    ret

.globl exec
exec:
    // x0 pc
    mov w8, #SYS_EXEC
    svc #0
    ret

.globl fork
fork:
    mov w8, #SYS_FORK
    svc #0
    ret  /* if kernel $x0 = new pid, child $x0 = 0 */

.globl get_taskid
get_taskid:
    mov w8, #SYS_ID
    svc #0
    ret /* return value $x0 is pid */

.globl kill
kill:
    mov w8, #SYS_KILL
    svc #0
    ret /* never return here */

.globl sync_call_uart
sync_call_uart:
    mov w8, #SYS_UART_EN
    svc #0
    ret