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
    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size

clear_bss_loop:
    cbz     w2, run // if all clear, run our program
    str     xzr, [x1], #8  // data xzr store to z1, and then address x1 = x1 + 8
    sub     w2, w2, #1  // w2 -= 1, check how many block we need to clear
    cbnz    w2, clear_bss_loop  // if value not zero, clear bss

    // goto main.o main()
run:
    bl main

    // if any problem, let cpu idel
    b idel_cpu
