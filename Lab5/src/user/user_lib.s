# 1 "src/user/user_lib.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "src/user/user_lib.S"
.section ".text.entry"

.global _user_entry
_user_entry:
 bl main
 bl exit

.set CORE_TIMER, 0
.set DAIF, 1
.set SYS_KILL, 2
.set SYS_FORK, 3
.set SYS_EXEC, 4
.set SYS_EXIT, 5
.set SYS_GET_TASKID, 6
.set SYS_UART_WRITE, 7
.set SYS_UART_READ, 8
.set SYS_USER_PRINT, 9

.global call_core_timer
call_core_timer:
 mov x8,CORE_TIMER
        svc #0
        ret

.global call_daif
call_daif:
 mov x8,DAIF
 svc #0
 ret

.global kill
kill:
 mov x8,SYS_KILL
 svc #0
 ret

.global fork
fork:
 mov x8,SYS_FORK
 svc #0
 ret

.global exec
exec:
 mov x8,SYS_EXEC
 svc #0
 ret

.global exit
exit:
 mov x8,SYS_EXIT
 svc #0
 ret

.global get_taskid
get_taskid:
 mov x8,SYS_GET_TASKID
 svc #0
 ret

.global uart_write
uart_write:
 mov x8,SYS_UART_WRITE
 svc #0
 ret

.global uart_read
uart_read:
 mov x8,SYS_UART_READ
 svc #0
 ret


.global user_printf
user_printf:
 mov x8,SYS_USER_PRINT
 svc #0
 ret
