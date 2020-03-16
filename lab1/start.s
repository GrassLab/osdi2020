
.global _start // otherwise the linkder script cannot find this as entry point

.text
_start:

  // let core with cpuid != 0 enter busy loop
  mrs x0, mpidr_el1
    // mrs: Move to ARM register from system coprocessor register.
    // mpidr_el1: Multiprocessor affinity register
      // Ref: https://developer.arm.com/docs/ddi0595/c/andrch64-system-registers/mpidr_el1

  and x0, x0, 3 // Each core has individual bit set
  cbz x0, core_0 // Compare and branch if zero

// other core will enter busy loop
other_core:
  wfe
  b other_core

// core 0 will jump here
core_0:

  // set stack pointer to __bss_start__ + 0x40000
  ldr x0, =__bss_start__
  add x0, x0, #0x40000
  mov sp, x0

  // initialize .bss section
  ldr x0, =__bss_start__
  eor x1, x1, x1 // x1 = 0
  ldr x2, =__bss_end__

  // jump to main if .bss length is 0
  sub x3, x2, x0
debug_a:
  cbz x3, main

bss_reset_loop:
  str x1, [x0], #0x8

  eor x3, x2, x0 // test if loop done

  cbnz x3, bss_reset_loop

  // Jump to main
  // main should not return or the stack will be smashed
  bl main

  // If main return, enter low power mode
  b other_core


