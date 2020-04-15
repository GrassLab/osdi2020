#include "sys.h"
.globl sync_call_time
sync_call_time:
    svc #1
    ret

.globl sync_call_exc
sync_call_exc:
    svc #2
    ret

.globl sync_call_uart
sync_call_uart:
    svc #3
    ret