#include "fat32.h"
mount fat32_mount;
vnode fat32_root;
file_operations f_ops;
vnode_operations v_ops;

struct partition_entry *part1;
struct boot_sector *fat32;
struct fat32_node init_node;

filesystem fat32_filesystem(){
    filesystem fs;
    fs.name = "fat32";
    fs.setup_mount = fat32_setup_mount;
    return fs;
}

int fat32_setup_mount(struct filesystem_t *fs, struct mount_t **mount){
    uart_puts("fat32 setup\n");
    fat32_mount.root = &fat32_root;
    fat32_mount.fs = fs;

    *mount = &fat32_mount;

    f_ops.write = fat32_write;
    f_ops.read = fat32_read;

    v_ops.lookup = fat32_lookup;
    v_ops.create = fat32_create;

    fat32_root.mount = &fat32_mount;
    fat32_root.v_ops = &v_ops;
    fat32_root.f_ops = &f_ops;

    char buf[512];
    

    // parse MBR to find first partition entry
    readblock (0, buf);
    part1 = (void *) buf;
    init_node.info.lba = part1->lba;
    init_node.info.size = part1->size;
    
    // parse fat32 BPB
    readblock (init_node.info.lba, buf);
    fat32 = (void *) buf;
    init_node.info.cluster_num_of_root = fat32->cluster_num_of_root;
    init_node.info.count_of_reserved = fat32->count_of_reserved;
    init_node.info.num_of_fat = fat32->num_of_fat;
    init_node.info.sectors_per_fat = fat32->sectors_per_fat;

    fat32_root.internal = &init_node;

    return 0;
}
int fat32_write (struct file_t *file, const void *buf, size_t len)
{
}

int fat32_read (struct file_t *file, void *buf, size_t len)
{
}

int fat32_lookup (struct vnode_t *dir_node, struct vnode_t **target,
	const char *component_name)
{
    uart_puts("fat32 lookup!\n");
    return 0;
}

int fat32_create (struct vnode_t *dir_node, struct vnode_t **target,
	const char *component_name)
{
}