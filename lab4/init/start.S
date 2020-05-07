.section ".text.boot"

.global _start

/*
 * x30 is the link register (used to return from subroutines)
 * x29 is the frame register
 * x19 to x29 are callee-saved
 * x18 is the 'platform register', used for some operating-system-specific special purpose, or an additional caller-saved register
 * x16 and x17 are the Intra-Procedure-call scratch register
 * x9 to x15: used to hold local variables (caller saved)
 * x8: used to hold indirect return value address
 * x0 to x7: used to hold argument values passed to a subroutine, and also hold results returned from a subroutine
 */

_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // cpu id > 0, stop
1:  wfe
    b       1b
2:  // cpu id == 0

    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

    ldr     x0, =_vectors
    msr     VBAR_EL1, x0
    
    ldr     x0, =_start
    msr     sp_el1, x0
    # Set Execution state to Aarch64
    mov     x0, (1 << 31)
    msr     hcr_el2, x0
    
    mov     x0, 0x5
    msr     spsr_el2, x0
    //adr     x0, 4f
    adr     x0, ret2EL0 // why ret from here
    msr     elr_el2, x0
    eret
ret2EL1:
    # SET fpen to 0b11
    mrs     x0, CPACR_EL1
    orr     x0, x0, #(3 << 20)
    msr     CPACR_EL1, x0
    adr     x0, _start
    sub     x0, x0, #0x10000
    msr     sp_el0, x0
    
    mov     x0, 0x0
    msr     spsr_el1, x0
    adr     x0, ret2EL0
    msr     elr_el1, x0
    eret
ret2EL0:
    mov     x0, xzr
4:  bl      main
    // for failsafe, halt this core too
    b       1b
    
.macro intr_cts_s
    sub	sp, sp, #256
    stp	x0, x1, [sp, #16 * 0]
    stp	x2, x3, [sp, #16 * 1]
    stp	x4, x5, [sp, #16 * 2]
    stp	x6, x7, [sp, #16 * 3]
    stp	x8, x9, [sp, #16 * 4]
    stp	x10, x11, [sp, #16 * 5]
    stp	x12, x13, [sp, #16 * 6]
    stp	x14, x15, [sp, #16 * 7]
    stp	x16, x17, [sp, #16 * 8]
    stp	x18, x19, [sp, #16 * 9]
    stp	x20, x21, [sp, #16 * 10]
    stp	x22, x23, [sp, #16 * 11]
    stp	x24, x25, [sp, #16 * 12]
    stp	x26, x27, [sp, #16 * 13]
    stp	x28, x29, [sp, #16 * 14]
    str	x30, [sp, #16 * 15] 
.endm

.macro intr_cts_l
    ldp	x0, x1, [sp, #16 * 0]
    ldp	x2, x3, [sp, #16 * 1]
    ldp	x4, x5, [sp, #16 * 2]
    ldp	x6, x7, [sp, #16 * 3]
    ldp	x8, x9, [sp, #16 * 4]
    ldp	x10, x11, [sp, #16 * 5]
    ldp	x12, x13, [sp, #16 * 6]
    ldp	x14, x15, [sp, #16 * 7]
    ldp	x16, x17, [sp, #16 * 8]
    ldp	x18, x19, [sp, #16 * 9]
    ldp	x20, x21, [sp, #16 * 10]
    ldp	x22, x23, [sp, #16 * 11]
    ldp	x24, x25, [sp, #16 * 12]
    ldp	x26, x27, [sp, #16 * 13]
    ldp	x28, x29, [sp, #16 * 14]
    ldr	x30, [sp, #16 * 15] 
    add	sp, sp, #256		
.endm
.macro cts_s
    sub	sp, sp, #0xB0
    stp	x10, x11, [sp, #0x10 * 0x0]
    stp	x12, x13, [sp, #0x10 * 0x1]
    stp	x14, x15, [sp, #0x10 * 0x2]
    stp	x16, x17, [sp, #0x10 * 0x3]
    stp	x18, x19, [sp, #0x10 * 0x4]
    stp	x20, x21, [sp, #0x10 * 0x5]
    stp	x22, x23, [sp, #0x10 * 0x6]
    stp	x24, x25, [sp, #0x10 * 0x7]
    stp	x26, x27, [sp, #0x10 * 0x8]
    stp	x28, x29, [sp, #0x10 * 0x9]
    str	x30, [sp, #0x10 * 0xA] 
.endm

.macro  cts_l
    ldp	x10, x11, [sp, #16 * 0]
    ldp	x12, x13, [sp, #16 * 1]
    ldp	x14, x15, [sp, #16 * 2]
    ldp	x16, x17, [sp, #16 * 3]
    ldp	x18, x19, [sp, #16 * 4]
    ldp	x20, x21, [sp, #16 * 5]
    ldp	x22, x23, [sp, #16 * 6]
    ldp	x24, x25, [sp, #16 * 7]
    ldp	x26, x27, [sp, #16 * 8]
    ldp	x28, x29, [sp, #16 * 9]
    ldr	x30, [sp, #16 * 10] 
    add	sp, sp, #0xB0
.endm

.MACRO SAVE_REG 
    sub sp, sp, 32 * 8
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    str x30, [sp, 16 * 15]
.ENDM

.MACRO LOAD_REG
    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    ldr x30, [sp, 16 * 15]
    add sp, sp, 32 * 8
.ENDM

.MACRO SAVE_RET
    mrs x19, tpidr_el1
    mrs x20, sp_el0
    mrs x21, elr_el1
    mrs x22, spsr_el1
    mov x23, sp
    stp x20, x21, [x19, 8 * 13]
    stp x22, x23, [x19, 8 * 15]
.ENDM

.MACRO LOAD_RET
    mrs x19, tpidr_el1
    ldp x20, x21, [x19, 8*13]
    ldr x22, [x19, 8 * 15]
    msr sp_el0, x20
    msr elr_el1, x21
    msr spsr_el1, x22
.ENDM

.global exception_handler
exception_handler:
    cts_s
    bl do_sys
    cts_l
    eret

.global interrupt_handler
interrupt_handler:
    intr_cts_s
    bl irq_handler
    intr_cts_l
    eret

    .align 11
.global _vectors
_vectors:
    /*
     * SP_EL0
     */
    b   exception_handler
    .align  7
    b   interrupt_handler
    .align  7
    bl undefine
    eret
    .align  7
    bl undefine
    eret
    .align  7
    /*
     * SP_ELx
     */
    b   exception_handler
    .align  7
    b   interrupt_handler
    .align  7
    bl undefine
    eret
    .align  7
    bl undefine
    eret
    .align  7
    /*
     * SP_EL0
     */
    b   exception_handler
    .align  7
    b   interrupt_handler
    .align  7
    bl undefine
    eret
    .align  7
    bl undefine
    eret
    .align  7
    /*
     * SP_ELx
     */
    b   exception_handler
    .align  7
    b   interrupt_handler
    .align  7
    bl undefine
    eret
    .align  7
    bl undefine
    eret
    .align  7


.globl enable_irq
enable_irq:
    msr   daifclr, #0xf
    ret

.globl disable_irq
disable_irq:
    msr   daifset, #0xf
    ret

#define CORE0_TIMER_IRQ_CTRL 0x40000040
.global sys_core_timer_init
sys_core_timer_init:
    cts_s
    mov     x0, 0xfffffff
    msr     cntp_tval_el0, x0
    mov     x0, 1
    msr     cntp_ctl_el0, x0 // enable timer
    mov     x0, 2
    ldr     x1, =CORE0_TIMER_IRQ_CTRL
    str     x0, [x1] // enable timer interrupt
    cts_l
    ret

#define EXPIRE_PERIOD 0xfffffff
.global core_timer_init
core_timer_init:
    mov     x0, #4
    svc     #0
    ret

.global core_timer_handler
core_timer_handler:
    //sub     sp, sp, #0x08
    //str     x0, [sp, #0x00]
    mov     x0, EXPIRE_PERIOD
    msr     cntp_tval_el0, x0
    //ldr     x0, [sp, #0x00]
    //add     sp, sp, #0x08
    ret

.global init_irq
init_irq:
    mrs     x0, hcr_el2
    orr     x0, x0, #0x10
    msr     hcr_el2, x0
    ret



.global context_switch
context_switch:
  stp x19, x20, [x0, 16 * 0]
  stp x21, x22, [x0, 16 * 1]
  stp x23, x24, [x0, 16 * 2]
  stp x25, x26, [x0, 16 * 3]
  stp x27, x28, [x0, 16 * 4]
  stp fp, lr, [x0, 16 * 5]
  mov x9, sp
  str x9, [x0, 16 * 6]

  ldp x19, x20, [x1, 16 * 0]
  ldp x21, x22, [x1, 16 * 1]
  ldp x23, x24, [x1, 16 * 2]
  ldp x25, x26, [x1, 16 * 3]
  ldp x27, x28, [x1, 16 * 4]
  ldp fp, lr, [x1, 16 * 5]
  ldr x9, [x1, 16 * 6]
  mov sp,  x9
  msr tpidr_el1, x1
  ret

.global jmp_to_el0
jmp_to_el0:
    mrs     x0, CPACR_EL1
    orr     x0, x0, (3 << 20)
    msr     CPACR_EL1, x0
    ldr     x0, =_start
    sub     x0, x0, #0x10000
    msr     sp_el0, x0
    
    mov     x0, 0x0
    msr     spsr_el1, x0
    eret