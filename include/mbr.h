#ifndef __MBR_H__
#define __MBR_H__

struct mbr_partition {
    unsigned char status_flag;              //0x0
    unsigned char partition_begin_head;     //0x1
    unsigned short partition_begin_sector;  //0x2-0x3
    unsigned char partition_type;           //0x4
    unsigned char partition_end_head;       //0x5
    unsigned short partition_end_sector;    //0x6-0x7
    unsigned int starting_sector;           //0x8-0xB
    unsigned int number_of_sector;          //0xC-0xF
};

struct mount* mbr_mount_router(int partition_type, char* metadata);

#endif
