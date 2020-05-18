#define __ASSEMBLY__
#include "sys.h"

.section ".text.entry"

.global _user_entry
_user_entry:
	bl main
	bl exit

.global exit
exit:
	mov x8, SYS_EXIT
	svc #0
	ret

.global uart_write
uart_write:
    mov x8, SYS_UART_WRITE
    svc #0
    ret
