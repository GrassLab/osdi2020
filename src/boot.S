#include "mm.h"
.section ".text.boot"

.global _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbz x0, continue

proc_hang:
    wfe
    b proc_hang

continue:
    adr x0, __bss_beg
    adr x1, __bss_end
    bl  memzero
    ldr x0, =__stack_top
    mov	sp, x0 
    bl main
