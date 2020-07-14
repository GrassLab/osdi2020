#include <stdint.h>
#include <stddef.h>
#ifndef __SYS_H__
#define __SYS_H__

int sys_mkdir(const char * pathname);
int sys_chdir(const char * pathname);
int sys_mount(const char * device, const char * mountpoint, const char * filesystem);
int sys_umount(const char * mountpoint);

#endif

