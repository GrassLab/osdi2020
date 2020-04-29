.section ".text.boot"

.global _start

_start:
    # read cpu id, stop slave cores
    mrs     x1,     MPIDR_EL1
    and     x1,     x1,         #3
    
    # cpu id > 0, stop
    cbnz     x1,    loop


clear_bss:
    # clear bss
    ldr     x1,     =__bss_start
    ldr     x2,     =__bss_size
    
cmp_addr: 
    cbz     x2,     stack_init
    str     xzr,    [x1],       #8
    sub     x2,     x2,         #1
    cbnz    x2,     cmp_addr
    
stack_init:  
    # set stack before our code
    ldr     x0,     =_start
    mov     sp,     x0

from_el2_to_el1:

    # EL1 use aarch64
    mov     x1,      #(1 << 31) 
    msr     HCR_EL2, x1

    # EL1h (SPSel = 1) with interrupt disabled
    # SPSR_EL2: Holds the saved process state when an exception is taken to EL2.
    # M[3:0]: AArch64 Exception level and selected Stack Pointer. 0b0101 -> EL1h
    # FIAD[9:6]
    mov     x1,     0b001111000101
    msr     SPSR_EL2, x1

    # set up exception handler for el2
    ldr     x1,     =exception_table_el2
    msr     VBAR_EL2, x1
    
    # set up exception handler for el1
    ldr     x1,     =exception_table_el1
    msr     VBAR_EL1, x1

    # init sp for EL1
    sub     x0,     x0,         #0x10000
    msr     SP_EL1, x0

    # load exception return address
    adr     x1,     return_to_el1
    msr     ELR_EL2, x1

    # return to EL1
    eret 

return_to_el1:

    # disable exception for accessing SIMD reg in EL1
    # CPACR_EL1: Controls access to trace, SVE, Advanced SIMD and floating-point functionality.
    # FPEN, bits[21:20]: Traps EL0 and EL1 accesses to the SVE, Advanced SIMD, and floating-point registers to EL1, or to EL2 when it is implemented and enabled for the current Security state and HCR_EL2.TGE is 1, from both Execution states.
    mrs     x1,     CPACR_EL1
    orr     x1,     x1,         (3 << 20) // # #0b1100110000000000000000
    # orr x1, x1, (1 << 16) 
    msr     CPACR_EL1, x1

    # disable interrupt in el1
    mov     x1,     0x3C0
    msr     SPSR_EL1, x1

    # create IDLE
    bl      create_idle_task


    # x0 would be the stack pointer of main, and should be the stack of el0
    # init sp for EL0
    # sub     x0,     x0,         #0x10000
    # msr     SP_EL0, x0
    
    /* launch main in el1, should not return */
    bl      launch_main
    # bl      create_idle_task
    
    # load exception return address
    # after done this all, go to schedule
    adr     x1,     main
    msr     ELR_EL1, x1

    # return to EL0
    eret

/* user mode start here */
# return_to_el0: 
#     
#     
# jmp_main:  
#     /* starting scheduling, should not return */
#     bl      main
#     b       loop

loop:  
    wfe
    b       loop