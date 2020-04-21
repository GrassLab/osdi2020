.section ".text"
.global _start
_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    // cpu id == 0
    cbnz     x1, loop

    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
clear_bss:
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, clear_bss


_setup_el1:
    // check if run in EL2
    mrs     x0, CurrentEL
    and     x0, x0, #12 // clear reserved bits //lsr x0, x0, #2

    // set up exception handlers
    ldr     x0, =_exception_table
    msr     vbar_el1, x0

    // set stack before our code
    ldr     x1, =_start
    msr     sp_el1, x1

    // enable AArch64 in EL1
    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2
    
    // change execution level to EL1
    mov     x0, 0x5//#0x3c4
    msr     spsr_el2, x0
    adr     x0, _run_main
    msr     elr_el2, x0
    eret


#define USER_STACK 0x1000

_el2_to_el1:
    // disable exception of access SIMD register in EL1
    mrs     x0, cpacr_el1
    orr     x0, x0, #(3 << 20)
    msr     cpacr_el1, x0

    // setup EL0 stack
    mov     x1, USER_STACK
    msr     sp_el0, x1
    // switch to EL0
    // spsr_el1 clear daif and switch to EL0
    mov     x1, 0x0
    msr     spsr_el1, x1
    adr     x1, _el1_to_el0
    msr     elr_el1, x1
    eret

_el1_to_el0:
_run_main:
    // // set up stack
    // ldr     x0, =_start
    // mov     sp, x0

    // // set argv[0] == current exception level 
    // mrs     x0, CurrentEL
    // lsr     x0, x0, #2
    // jump to C code, should not return
    bl      main
loop:  
    // for failsafe, halt this core too
    // cpu id > 0, stop
    wfe             // wait for event 
    b       loop
