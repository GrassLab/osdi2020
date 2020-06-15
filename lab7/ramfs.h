#ifndef __RAMFS_H__
#define __RAMFS_H__

#define RAMFS_INITRAMFS_SLASH_LENGTH 10u

void ramfs_init(void);

extern char _binary_initramfs_cpio_start;

#endif

