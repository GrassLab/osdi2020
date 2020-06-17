#ifndef __FS_TMPFS_H
#define __FS_TMPFS_H

typedef struct __TmpFsInterface {
} TmpFsInterface;

extern TmpFsInterface gTmpFs;

struct filesystem *newTmpFs(void);

#endif
