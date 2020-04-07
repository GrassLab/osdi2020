#pragma once

enum {
    DATA_ACK = 0x53,            /* 'S' */
    DATA_SYN = 0x45,            /* 'E' */
    ADDR_ACK = 0x40,            /* '@' */
    UPLOAD_ADDRESS = 0x80000,
    BOOT_ADDR      = 0x200000,
};

typedef void (*kentry_t)(void);

/* the parameter for the loadimg is used to indicate the region of bootloader */
void loadimg(unsigned int bootloader_end);
