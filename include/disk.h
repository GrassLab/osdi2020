#ifndef __DISK_H__
#define __DISK_H__

struct mbr_partition {
    int partition_type;
    int first_sector_idx;
    int sector_num;
};

struct disk {
    struct mbr_partition partition[4];
};

void mbr_parse(struct disk* disk, char* buf);

#endif