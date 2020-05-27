.section ".text.entry"

.global _user_entry
_user_entry:
	bl main
	bl exit

.set CORE_TIMER, 0   // syscal numbers
.set DAIF,     1
.set SYS_KILL, 2
.set SYS_FORK, 3
.set SYS_EXEC, 4
.set SYS_EXIT, 5
.set SYS_GET_TASKID, 6
.set SYS_UART_WRITE, 7
.set SYS_UART_READ, 8
.set SYS_USER_PRINT, 9
.set SYS_REBOOT,     10
.set SYS_DELAY,      11
.set SYS_REMAIN_PAGE, 12
.set SYS_MMAP,       13
.set SYS_WAIT,       14
.set SYS_MALLOC,      15
.set SYS_FREE,      16

.global call_core_timer
call_core_timer:
	mov x8,CORE_TIMER //x8 is for syscall number
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

	
.global printf
printf:
	mov x8,SYS_USER_PRINT
	svc #0
	ret

.global reboot
reboot:
	mov x8, SYS_REBOOT
	svc #0
	ret

.global delay
delay:	
	mov x8, SYS_DELAY
	svc #0
	ret

.global get_remain_page_num
get_remain_page_num:
	mov x8, SYS_REMAIN_PAGE
	svc #0
	ret

.global mmap
mmap:
	mov x8, SYS_MMAP
	svc #0
	ret

.global wait
wait:
	mov x8, SYS_WAIT
	svc #0
	ret

.global malloc
malloc:
	mov x8, SYS_MALLOC
	svc #0
	ret

.global free
free:
	mov x8, SYS_FREE
	svc #0
	ret
