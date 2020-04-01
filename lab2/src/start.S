.section ".text.boot"

.global _start

_start:
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, main_cpu

idel_cpu:
    wfe
    b       idel_cpu
  
main_cpu:

clear_bss_loop:
    adr x0, __bss_begin
    adr x1, __bss_end
    sub x1, x1, x0
    bl  memzero
    mov sp, #4194304

run:
    bl main

    b idel_cpu

memzero:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.gt memzero
    ret