#ifndef __RAMFS_H__
#define __RAMFS_H__

#define RAMFS_INITRAMFS_LENGTH 9u /* leave the slash to indicate it as a absolute path */

void ramfs_init(void);

extern char _binary_initramfs_cpio_start;

#endif

