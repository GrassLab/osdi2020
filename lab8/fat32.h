#define FAT32_FILE 0x20
#define FAT32_DIR 0x10

typedef struct FAT32_info
{
    int sector_size;
    int cluster_sectors;
    int num_of_reserved_sec;
    int num_of_FATs;
    int FATs_secs;
    int root_dir_sector;
    int size;
    int first_block_idx;
    int root_dir_start_idx;
    int FAT_cluster_start_idx;
}FAT32_info;

typedef struct FAT32fs{
    FAT32_info *meta;
    int file_cluster_idx;
    char name[9];
    char ext[4];
    int file_type;
    int file_size;
}FAT32fs;
void fat32_setup_mount(struct filesystem* fs, struct mount* mount);
struct mount * fat32_init();
int fat32_vnode_creat(struct vnode* dir_node, struct vnode** target, const char* component_name);
int fat32_write(struct file* file, const void* buf, int len);
int fat32_read(struct file* file, void* buf, int len);
int fat32_vnode_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);