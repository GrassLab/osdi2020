#ifndef TMPFS_H
#define TMPFS_H

#include "vfs.h"

#define TMPFS_FIEL_BUFFER_MAX_LEN 64

typedef struct tmpfs_node
{
    char * name;
    size_t file_length;
    char buffer[TMPFS_FIEL_BUFFER_MAX_LEN];

} tmpfs_node_t;

void tmpfs_init ( );

#endif