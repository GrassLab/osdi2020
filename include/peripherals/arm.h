#ifndef __ARM_PERIPHERALS__
#define __ARM_PERIPHERALS__

#include "mm.h"

#define ARM_PERIPHERALS_PHYSICAL        0x40000000
#define ARM_PERIPHERALS_BASE            (KERNEL_VIRT_BASE | ARM_PERIPHERALS_PHYSICAL)

#define CORE0_TIMER_IRQ_CTRL            ((unsigned int*)(ARM_PERIPHERALS_BASE + 0x40))
#define CORE1_TIMER_IRQ_CTRL            ((unsigned int*)(ARM_PERIPHERALS_BASE + 0x44))
#define CORE2_TIMER_IRQ_CTRL            ((unsigned int*)(ARM_PERIPHERALS_BASE + 0x48))
#define CORE3_TIMER_IRQ_CTRL            ((unsigned int*)(ARM_PERIPHERALS_BASE + 0x4C))

#define CORE0_INTR_SRC                  ((unsigned int*)(ARM_PERIPHERALS_BASE + 0x60))
#define CORE1_INTR_SRC                  ((unsigned int*)(ARM_PERIPHERALS_BASE + 0x64))
#define CORE2_INTR_SRC                  ((unsigned int*)(ARM_PERIPHERALS_BASE + 0x68))
#define CORE3_INTR_SRC                  ((unsigned int*)(ARM_PERIPHERALS_BASE + 0x6C))

#define CORE_TIMER_EXPRIED_PERIOD       0xffffff

#define LOCAL_TIMER_CTRL                ((unsigned int*)(ARM_PERIPHERALS_BASE + 0x34))
#define LOCAL_TIMER_IRQ_CLR             ((unsigned int*)(ARM_PERIPHERALS_BASE + 0x38))

#endif