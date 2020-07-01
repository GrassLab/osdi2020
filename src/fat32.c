#include "fat32.h"

#include "mm.h"
#include "my_string.h"
#include "sdhost.h"
#include "uart0.h"
#include "vfs.h"

struct fat32_metadata fat32_metadata;

struct vnode_operations* fat32_v_ops = NULL;
struct file_operations* fat32_f_ops = NULL;

struct vnode* fat32_create_vnode(struct dentry* dentry) {
    struct vnode* vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
    vnode->dentry = dentry;
    vnode->f_ops = fat32_f_ops;
    vnode->v_ops = fat32_v_ops;
    return vnode;
}

struct dentry* fat32_create_dentry(struct dentry* parent, const char* name, int type) {
    struct dentry* dentry = (struct dentry*)kmalloc(sizeof(struct dentry));
    strcpy(dentry->name, name);
    dentry->parent = parent;
    list_head_init(&dentry->list);
    list_head_init(&dentry->childs);
    if (parent != NULL) {
        list_add(&dentry->list, &parent->childs);
    }
    dentry->vnode = fat32_create_vnode(dentry);
    dentry->mountpoint = NULL;
    dentry->type = type;
    return dentry;
}

// error code: -1: already register
int fat32_register() {
    if (fat32_v_ops != NULL && fat32_f_ops != NULL) {
        return -1;
    }
    fat32_v_ops = (struct vnode_operations*)kmalloc(sizeof(struct vnode_operations));
    fat32_v_ops->create = fat32_create;
    fat32_v_ops->lookup = fat32_lookup;
    fat32_v_ops->ls = fat32_ls;
    fat32_v_ops->mkdir = fat32_mkdir;
    fat32_v_ops->load_dentry = fat32_load_dentry;
    fat32_f_ops = (struct file_operations*)kmalloc(sizeof(struct file_operations));
    fat32_f_ops->read = fat32_read;
    fat32_f_ops->write = fat32_write;
    return 0;
}

int fat32_setup_mount(struct filesystem* fs, struct mount* mount) {
    mount->fs = fs;
    mount->root = fat32_create_dentry(NULL, "/", DIRECTORY);
    return 0;
}

int fat32_lookup(struct vnode* dir, struct vnode** target, const char* component_name) {
    // component_name is empty, return dir vnode
    if (!strcmp(component_name, "")) {
        *target = dir;
        return 0;
    }
    // search component_name in dir
    struct list_head* p = &dir->dentry->childs;
    list_for_each(p, &dir->dentry->childs) {
        struct dentry* dentry = list_entry(p, struct dentry, list);
        if (!strcmp(dentry->name, component_name)) {
            *target = dentry->vnode;
            return 0;
        }
    }
    *target = NULL;
    return -1;
}

int fat32_create(struct vnode* dir, struct vnode** target, const char* component_name) {
    return 0;
}

int fat32_ls(struct vnode* dir) {
}

int fat32_mkdir(struct vnode* dir, struct vnode** target, const char* component_name) {
}

int fat32_load_dentry(struct dentry* dir, char* component_name) {
    // read first block of cluster
    struct fat32_internal* dir_internal = (struct fat32_internal*)dir->vnode->internal;
    uint32_t target_blk = fat32_metadata.root_sector_idx +
                          (dir_internal->cluster_num - fat32_metadata.first_cluster) * fat32_metadata.sector_per_cluster;
    uint8_t sector[512];
    readblock(target_blk, sector);

    // parse
    struct fat32_dirent* sector_dirent = (struct fat32_dirent*)sector;

    // load all children under dentry
    for (int i = 0; sector_dirent[i].name[0] != '\0'; i++) {
        // get filename
        char filename[13];
        int len = 0;
        for (int j = 0; j < 8; j++) {
            char c = sector_dirent[i].name[j];
            if (c == ' ') {
                break;
            }
            filename[len++] = c;
        }
        filename[len++] = '.';
        for (int j = 0; j < 3; j++) {
            char c = sector_dirent[i].ext[j];
            if (c == ' ') {
                break;
            }
            filename[len++] = c;
        }
        filename[len++] = 0;
        // create dirent
        uint8_t file_attr = sector_dirent[i].attr[0];
        struct dentry* dentry;
        if (file_attr == 0x10) { // directory
            dentry = fat32_create_dentry(dir, filename, DIRECTORY);
        }
        else { // file
            dentry = fat32_create_dentry(dir, filename, REGULAR_FILE);
        }
        // create fat32 internal
        struct fat32_internal* child_internal = (struct fat32_internal*)kmalloc(sizeof(struct fat32_internal));
        child_internal->cluster_num = ((sector_dirent[i].cluster_high) << 16) | (sector_dirent[i].cluster_low);
        dentry->vnode->internal = child_internal;
    }
    return 0;
}

// file operations
int fat32_read(struct file* file, void* buf, uint64_t len) {
}

int fat32_write(struct file* file, const void* buf, uint64_t len) {
}