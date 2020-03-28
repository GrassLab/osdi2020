.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, main_cpu // compare if zero, jump to main_cpu

// cpu id > 0, idle
idel_cpu:
    wfe
    b       idel_cpu
  
// cpu id == 0
main_cpu:

clear_bss_loop:
    adr x0, __bss_begin
    adr x1, __bss_end
    sub x1, x1, x0
    bl  memzero // Branch with a link (when the function call is done, it should come back here)
    mov sp, #4194304

    // goto main.o main()
run:
    bl main

    // if any problem, let cpu idel
    b idel_cpu

memzero:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.gt memzero
    ret
