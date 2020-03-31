.section ".text.boot"

.global _start

_start:
    // save arguments in registers (we will need them later for the new kernel)
    // I choosed x10-x13 because instructions generated from C by gcc does not
    // touch them. You can check that with "aarch64-elf-objdump -d kernel8.elf"
    mov     x10, x0
    mov     x11, x1
    mov     x12, x2
    mov     x13, x3

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


    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
clear_bss_loop:
    cbz     w2, run
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, clear_bss_loop

    // goto main.o main()
run:
    bl main 

    // if any problem, let cpu idel
    b idel_cpu

