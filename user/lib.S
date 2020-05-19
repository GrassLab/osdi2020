.section ".text.entry"

.global _user_entry
_user_entry:
	bl main
	bl exit

.set SYS_EXIT,		2
.set SYS_PRINTF, 	8

.global exit
exit:
	mov x8, #SYS_EXIT
	svc #0
	ret

.global printf
printf:
	mov x8, #SYS_PRINTF
	svc #0
	ret
