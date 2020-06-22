#include <stdint.h>
#include "string_util.h"
#ifndef __EXC_H__
#define __EXC_H__

#define X0_TRAPFRAME_OFFSET 288

struct trapframe_struct
{
  uint64_t x1;  uint64_t x2;  uint64_t x3;  uint64_t x4;
  uint64_t x5;  uint64_t x6;  uint64_t x7;  uint64_t x8;
  uint64_t x9;  uint64_t x10; uint64_t x11; uint64_t x12;
  uint64_t x13; uint64_t x14; uint64_t x15; uint64_t x16;
  uint64_t x17; uint64_t x18; uint64_t x19; uint64_t x20;
  uint64_t x21; uint64_t x22; uint64_t x23; uint64_t x24;
  uint64_t x25; uint64_t x26; uint64_t x27; uint64_t x28;
  uint64_t x29; uint64_t x30;
  uint64_t spsr_el1;
  uint64_t elr_el1;
  uint64_t sp_el0;
};

void exc_dispatcher(uint64_t identifier, struct trapframe_struct * trapframe);
void exc_not_implemented(uint64_t code);
void exc_set_x0(uint64_t retval, struct trapframe_struct * trapframe);
void exc_EL1_same_level_EL_SP_EL1_sync(void);
void exc_EL1_same_level_EL_SP_EL1_irq(void);
void exc_EL1_lower_aa64_EL_SP_EL1_sync(struct trapframe_struct * trapframe);
void exc_EL1_lower_aa64_EL_SP_EL1_irq(void);
void exc_EL2_enable_physical_interrupt(void);

#endif

