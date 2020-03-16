#include "mm.h"
.section ".text.boot"

_start:
    mrs x0, mpidr_el1
    and x0, x0, 3
    cbz x0, continue

proc_hang:
    wfe
    b proc_hang

continue:
    adr x0, bss_beg
    adr x1, bss_end
    bl  memzero
	mov	sp, #LOW_MEMORY 
    //ldr x0, =_stack_top
    //mov sp, x0
    bl main
