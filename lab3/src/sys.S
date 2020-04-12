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

.globl sync_call_dintp
sync_call_dintp:
    svc #4
    ret

.globl sync_call_eintp
sync_call_eintp:
    svc #5
    ret