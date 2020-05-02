.section ".text.boot"

.global _start

_start:
  mrs x0, mpidr_el1
  and x0, x0, #3
  cbz x0, 1f
0:
  wfe
  b 0b

1:
  ldr x0, =_start
  mov sp, x0
  ldr x0, =__bss_start
  ldr x1, =__bss_end
  sub x1, x1, x0
  lsr x1, x1, #3
2:
  cbz x1, el2_to_el1
  str xzr, [x0]
  add x0, x0, #8
  sub x1, x1, #1
  b 2b

el2_to_el1:
  mov x0, #(1 << 31)
  msr hcr_el2, x0

  /*
   * 0x3c5 == b1111_0_0_0101
   * M[3:0]: AArch64 Exception level and selected Stack Pointer, 5 means EL1h
   * M[4]:   Execution state, 0 means AArch64 execution state
   * bit 5 is reserved
   * bit 6~9 is DAIF mask
   */
  mov x0, 0x3c5
  msr spsr_el2, x0

  /*
   * Disable traps from EL0 and El1 when accessing SIMD or FP registers.
   * Another method would be prevent gcc from using these registers at all,
   * with '-march=armv8-a+nofp' flag.
   *
   * https://developer.arm.com/docs/ddi0595/b/aarch64-system-registers/cpacr_el1
   * https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
   */
  mrs x0, cpacr_el1
  orr x0, x0, #(3 << 20)
  msr cpacr_el1, x0

  adr x0, 3f
  msr elr_el2, x0
  ldr x0, =kstack_pool
  add x0, x0, #4096
  msr sp_el1, x0
  eret

3:
  bl main
