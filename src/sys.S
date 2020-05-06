#define IN_ASM
#include "sys.h"

.global exit
exit:
    mov x8, SYS_EXiT
    svc #0