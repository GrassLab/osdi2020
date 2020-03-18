.section ".text.boot"

.global _start

// let core with cpuid != 0 enter busy loop
_start:
    mrs x0, mpidr_el1
    and x0, x0, 3     // Get the frist two bits in Aff0 fileds
    cbz x0, 2f        // Compare and Branch on Zero,
                      // if zero goto forward label2, otherwise continue
1: // cpuid > 0
    wfe
    b 1b

// cpuid = 0
2:
    // set the stack pointer to _start
    ldr x0, =__stack_top
    mov sp, x0

    // initialize the bss segment
    // calcluate the bss size by giving the __
    ldr x0, =__bss_start
    ldr w2, =__bss_size
3:
    cbz     w2, 4f          // if the size of bss is 0 then goto label 4 forward
    str     xzr, [x0], #8   // *x0 = zero, x0 = x0 + 8
    sub     w2, w2, #1      // w2 = w2 - 1
    cbnz    w2, 3b          // go to label 3 backward, until the size of bss is 0

    // then branch and link to main function(shell)
4:
    bl main
    b 1b
//1:
//    b 1b

// read frequency of core timer
//mrs x0, cntfrq_el0

// read counts of core timer
//mrs x0, cntpct_el0
