.section ".text"            // start of the text segment
_start:                     // entry point for the linker
    mrs    x1, mpidr_el1    // load the multiprocessor affinity register
    and    x1, x1, #3       // OxFF mask to get the cpu id
    cbz    x1, mem_setup    // if cpuid is 0, start the booting process

/* 
 * Enter into low power standby mode.
 * It makes the cpu enter the STANDBYWFE state, which suspends execution,
 * unless an event is signaled by the SEV instruction or an interrupt happends.
 * This is a better alternative than using a simple NOP operation
 * because the latter would use all the cpu ressources for an infinite loop
 *
 * source:  http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka15473.html
 */
busy_loop:
    wfe
    b       busy_loop	    // branch back into busy_loop to make the cpu idle forever

mem_setup:
    ldr     x1, = _start
    mov     sp, x1          // set the stack pointer to 0x80000i
    ldr     x1, = __bss_start
    ldr     w2, = __bss_size
    cbz     w2, start_main

mem_zero:    
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, mem_zero

start_main:
    bl      ker_main        // branch and link to the C code
    b       busy_loop       // if something goes wrong, get in the busy loop

