.section ".text.entry"

.global _user_entry
_user_entry:
	bl main
	 bl exit

.global exit
exit:
	mov x0, #0x32
	svc #0x80