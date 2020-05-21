.section ".text.entry"

.global _user_entry
_user_entry:
	bl main
	bl exit

.global syscall
syscall:
  svc 0
  ret