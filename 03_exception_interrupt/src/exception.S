#define S_FRAME_SIZE 256

.macro handle_invalid_entry el type
    kernel_entry
    mov x0, #\type
    mrs x1, esr_el\el
    mrs x2, elr_el\el
    bl  report_exception
    // bl err_handle
    kernel_exit
.endm

// x0 - x30, no x31
.macro  kernel_entry
    sub sp, sp, #S_FRAME_SIZE
    stp x0, x1, [sp, #16 * 0]
    stp x2, x3, [sp, #16 * 1]
    stp x4, x5, [sp, #16 * 2]
    stp x6, x7, [sp, #16 * 3]
    stp x8, x9, [sp, #16 * 4]
    stp x10, x11, [sp, #16 * 5]
    stp x12, x13, [sp, #16 * 6]
    stp x14, x15, [sp, #16 * 7]
    stp x16, x17, [sp, #16 * 8]
    stp x18, x19, [sp, #16 * 9]
    stp x20, x21, [sp, #16 * 10]
    stp x22, x23, [sp, #16 * 11]
    stp x24, x25, [sp, #16 * 12]
    stp x26, x27, [sp, #16 * 13]
    stp x28, x29, [sp, #16 * 14]
    str x30, [sp, #16 * 15] 
.endm

.macro  kernel_exit
    ldp x0, x1, [sp, #16 * 0]
    ldp x2, x3, [sp, #16 * 1]
    ldp x4, x5, [sp, #16 * 2]
    ldp x6, x7, [sp, #16 * 3]
    ldp x8, x9, [sp, #16 * 4]
    ldp x10, x11, [sp, #16 * 5]
    ldp x12, x13, [sp, #16 * 6]
    ldp x14, x15, [sp, #16 * 7]
    ldp x16, x17, [sp, #16 * 8]
    ldp x18, x19, [sp, #16 * 9]
    ldp x20, x21, [sp, #16 * 10]
    ldp x22, x23, [sp, #16 * 11]
    ldp x24, x25, [sp, #16 * 12]
    ldp x26, x27, [sp, #16 * 13]
    ldp x28, x29, [sp, #16 * 14]
    ldr x30, [sp, #16 * 15] 
    add sp, sp, #S_FRAME_SIZE
    eret
.endm

.align 11
.global exception_table_el2
exception_table_el2:
  b NotImplementedHandler_el2 // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b NotImplementedHandler_el2
  .align 7
  b NotImplementedHandler_el2
  .align 7
  b NotImplementedHandler_el2
  .align 7

  b sync_exception_el2h
  .align 7
  b core_timer_handler_el2
  .align 7
  b NotImplementedHandler_el2
  .align 7
  b NotImplementedHandler_el2
  .align 7

  b NotImplementedHandler_el2
  .align 7
  b NotImplementedHandler_el2
  .align 7
  b NotImplementedHandler_el2
  .align 7
  b NotImplementedHandler_el2
  .align 7

  b NotImplementedHandler_el2
  .align 7
  b NotImplementedHandler_el2
  .align 7
  b NotImplementedHandler_el2
  .align 7
  b NotImplementedHandler_el2
  .align 7

NotImplementedHandler_el2:
    handle_invalid_entry    2 16
    b _loop

core_timer_handler_el2:
    kernel_entry
    mov x0, 0xffffff
    msr cntp_tval_el0, x0
    bl  count
    kernel_exit

sync_exception_el2h:
    handle_invalid_entry    2 4

// should do something in the future
err_handle:
    bl err_handle
    ret

.align 11
.global exception_table_el1
exception_table_el1:
  b NotImplementedHandler_el1 // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b NotImplementedHandler_el1
  .align 7
  b NotImplementedHandler_el1
  .align 7
  b NotImplementedHandler_el1
  .align 7

  b NotImplementedHandler_el1
  .align 7
  b core_timer_handler_el2
  // b NotImplementedHandler_el1
  .align 7
  b NotImplementedHandler_el1
  .align 7
  b NotImplementedHandler_el1
  .align 7

  b sync_exception_el0_64
  .align 7
  b core_timer_handler_el2
  // b NotImplementedHandler_el1
  .align 7
  b NotImplementedHandler_el1
  .align 7
  b NotImplementedHandler_el1
  .align 7

  b NotImplementedHandler_el1
  .align 7
  b NotImplementedHandler_el1
  .align 7
  b NotImplementedHandler_el1
  .align 7
  b NotImplementedHandler_el1
  .align 7

sync_exception_el0_64:
    kernel_entry
    mrs x0, esr_el1
    mrs x1, elr_el1
    bl handle_sync_excpetion_el0_64
    cbnz x0, _unknown_exception 
    kernel_exit
_unknown_exception:
    handle_invalid_entry    1   8
    kernel_exit

NotImplementedHandler_el1:
    handle_invalid_entry    1   16
    b _loop
