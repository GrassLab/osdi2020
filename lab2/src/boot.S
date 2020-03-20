#include "mm.h"

.section ".text.boot"

.global _start
_start:
    mrs x0, mpidr_el1
    and x0, x0,#0xFF // We do this and to strip the last byte of the value coming from mpidr_el1
    cbz x0, master
    b   proc_hang

proc_hang:
    b proc_hang

master:
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl  memzero // Branch with a link (when the function call is done, it should come back here)
    mov sp, #LOW_MEMORY
    bl kernel_main