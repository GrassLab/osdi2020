#include "sys.h"
.globl sync_call_time
sync_call_time:
    mov w8, #1
    svc #1
    ret

.globl sync_call_exc
sync_call_exc:
    mov w8, #2
    svc #2
    ret

.global sync_call_proc
sync_call_proc:
    mov w8, #3
    svc #3
    ret
