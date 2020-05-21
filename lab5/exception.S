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

.section ".text"
.align 11 
.global exception_table
    exception_table:
        b _other_exc_handler 
        .align 7
        b _other_exc_handler
        .align 7
        b _other_exc_handler
        .align 7
        b _other_exc_handler
        .align 7

        b _EL1_sync_handler //0x200-0x280
        .align 7
        b _ELx_IRQ_handler
        .align 7
        b _other_exc_handler
        .align 7
        b _other_exc_handler
        .align 7

        b _EL1_sync_handler  //0x400-0x600
        .align 7
        b _ELx_IRQ_handler
        .align 7
        b _other_exc_handler
        .align 7
        b _other_exc_handler
        .align 7
        
        b _other_exc_handler 
        .align 7
        b _other_exc_handler
        .align 7
        b _other_exc_handler
        .align 7
        b _other_exc_handler
        .align 7


.global _ELx_IRQ_handler
_ELx_IRQ_handler:
    SAVE_REG
    SAVE_RET 
    bl irq_hanlder
    LOAD_RET
    LOAD_REG
    eret


.global _other_exc_handler
_other_exc_handler:
    SAVE_REG
    bl other_handler
    LOAD_REG
    eret

.global _EL1_sync_handler
_EL1_sync_handler: 
    SAVE_REG
    SAVE_RET
    bl sync_el1_exc_handler
    LOAD_RET
    LOAD_REG
    eret

.global syscall
syscall:
  svc 0
  ret

