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

  // setup exception vector for EL1
  ldr x0, =EL1_vector
  msr VBAR_EL1, x0

  // set rw bit in hcr_el2 because we're using 64-bit kernel
  mrs x0, hcr_el2
  mov x1, #0x80000000
  orr x0, x0, x1
  msr hcr_el2, x0

  // set stack pointer to __bss_end__ + 0x4000
  ldr x0, =__bss_end__
  add x0, x0, #0x4000
  mov sp, x0

  // setup sp_el for el0 and el1, el2 need to be set in el3
  add x0, x0, #0x2000
  msr sp_el0, x0
  add x0, x0, #0x2000
  msr sp_el1, x0

  // initialize .bss section
  ldr x0, =__bss_start__
  eor x1, x1, x1 // x1 = 0
  ldr x2, =__bss_end__

  // jump to el2_to_el1 if .bss length is 0
  sub x3, x2, x0
  cbz x3, el2_to_el1

bss_reset_loop:
  str x1, [x0], #0x8

  eor x3, x2, x0 // test if loop done

  cbnz x3, bss_reset_loop

el2_to_el1:
  // setup spsr_el2
  // Ref: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0488c/CHDDGJID.html
  // Use el1h -> sp_el1 -> 0x5
  mov x0, 0x5
  msr spsr_el2, x0

  // Use eret to el1
  adr x0, el1_schedule
  msr elr_el2, x0
  eret

el1_schedule:
// EL1 now
  bl mmu_tcr_el1_init
  bl mmu_mair_el1_init
  bl mmu_ttbr0_el1_init
  bl mmu_enable_mmu

  bl uart_init
  bl scheduler_init

  // In case return, enter low power mode
  b other_core

