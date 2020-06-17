#include "mmu.h"

.global mmu_tcr_el1_init
.type mmu_tcr_el1_init,%function
mmu_tcr_el1_init:

  // tcr_el1: The control register for stage 1 of the EL1&0 translation regime

  ldr x0, = TCR_CONFIG_DEFAULT
  msr tcr_el1, x0
  ret

.global mmu_mair_el1_init
.type mmu_mair_el1_init,%function
mmu_mair_el1_init:

  /* mair_el1: Provides the memory attribute encodings corresponding to the possible AttrIndx values in a Long-descriptor format translation table entry for stage 1 translations at EL1. */

  ldr x0, =( \
    (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
    (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
  )
  msr mair_el1, x0
  ret

.global mmu_enable_mmu
.type mmu_enable_mmu,%function
mmu_enable_mmu:
  /* DO NOT CHANGE VALUE OF x0 */

  /* Provides top level control of the system, including its memory system, at EL1 and EL0 */
  /* M[0]: MMU enable for EL1 and EL0 stage 1 address translation */

  mrs x2, sctlr_el1
  orr x2 , x2, 1
  msr sctlr_el1, x2 // enable MMU, cache remains disabled
  ret

