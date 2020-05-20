.section ".text.entry"

.global _user_entry
_user_entry:
	bl main
	bl sys_exit

.global sys_uart_write
sys_uart_write:
    mov x8, #0
    svc 1
    ret

.global sys_uart_read
sys_uart_read:
    mov x8, #1
    svc 1
    ret

.global sys_exit
sys_exit:
    mov x8, #4
    svc 1
