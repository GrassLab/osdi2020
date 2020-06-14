#ifndef TMPFS_H
#define TMPFS_H

#include "vfs.h"

typedef struct tmpfs_node
{
    char * name;

} tmpfs_node_t;

void tmpfs_init ( );

#endif