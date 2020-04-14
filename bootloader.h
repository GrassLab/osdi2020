#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#define TMP_KERNEL_ADDR 0x100000
#include "uart.h"
#include "common.h"

void loadimg()
    __attribute__((section(".bootloader")));

void loadimg_jmp(void *address, unsigned long img_size)
    __attribute__((section(".bootloader")));
#endif
