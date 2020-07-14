#ifndef _TMPFS_H
#define _TMPFS_H

#include "type.h"
#include "filesystem/filesystem.h"

int32_t tmpfsSetupMount(struct filesystem* fs, struct mount* mount);

#endif