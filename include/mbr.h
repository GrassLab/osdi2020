#ifndef __MBR_H__
#define __MBR_H__

struct mbr_partition {
    int type;
    int first_sector_idx;
    int sector_num;
};

struct mbr_meta {
    struct mbr_partition partition[4];
};

void mbr_parse(struct mbr_meta* meta, char* buf);
struct mount* mbr_mount_router(int partition_type, char* metadata);

#endif
