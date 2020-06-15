.section ".text.entry"

#include "user_lib.h"
.global _user_entry
_user_entry:
	bl main
	bl call_sys_exit


.globl call_sys_exit
call_sys_exit:
	mov w8, #SYS_EXIT_NUMBER	
	svc #0
	ret

.globl call_sys_write
call_sys_write:
	mov w8, #SYS_WRITE_NUMBER	
	svc #0
	ret

.globl call_sys_write_char
call_sys_write_char:
	mov w8, #SYS_WRITE_CHAR_NUMBER	
	svc #0
	ret

.globl call_sys_read
call_sys_read:
	mov w8, #SYS_READ_NUMBER	
	svc #0
	ret