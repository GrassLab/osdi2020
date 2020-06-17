#ifndef __FS_TMPFS_H
#define __FS_TMPFS_H

struct filesystem *newTmpFs(void);
extern struct file_operations tmpfs_f_ops;

#endif
