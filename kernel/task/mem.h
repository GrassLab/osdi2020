#ifndef MEM_H

#include "kernel/peripherals/gpio.h"

#define THREAD_SIZE     4096
#define NUM_THREADS     64

#define KERNEL_SPACE    ( THREAD_SIZE * NUM_THREADS )

#define HIGH_MEM        ((volatile unsigned int*) MMIO_BASE )
#define LOW_MEM         ((volatile unsigned int*)( HIGH_MEM - KERNEL_SPACE ))

#endif