// very thing defined in boot.S should go in the .text.boot section
.section ".text.boot"


.global _start

_start:
    // read cpu id, stop slave cores
    mrs     x0, mpidr_el1  // put mpidr_el1(process id) in x0
    and     x0, x0, #0xFF // Check processor id(x0= x0 and 0xFF)
    cbz     x0, master // Hang for all non-primary CPU
    
    //(if x0 == 0 jump to master)
    b       proc_hang    //Else it will become a for loop


proc_hang:
    b proc_hang

master:
    // set stack before our code
    ldr     x1, =_start
    
    // set up exception handlers
    ldr     x2, = exception_table
    msr     vbar_el2, x2

    mov     sp,x1

    // clear bss
    ldr     x1, =_bss_begin
    ldr     w2, =__bss_size
clean:
    cbz     w2, main
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, clean

    // jump to C code, should not return
main:  
    bl      kernel_main
    // for failsafe, halt this core too
    b       proc_hang



