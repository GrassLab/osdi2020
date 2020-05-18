#include "mm.h"

#ifndef MMIO_H
#define MMIO_H

#define MMIO_PHYSICAL   0x3F000000
#define MMIO_BASE       (KERNEL_VIRT_BASE | MMIO_PHYSICAL)

#endif