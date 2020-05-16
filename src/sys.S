#define __ASSEMBLY__
#include "sys.h"

.global get_taskid
get_taskid:
    mov x8, SYS_GET_TASK_ID
    svc #0
    ret
