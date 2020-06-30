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
    uart_printf("%s\n", component_name);
    return 0;
}

int fat32_create(struct vnode* dir, struct vnode** target, const char* component_name) {
    return 0;
}

int fat32_ls(struct vnode* dir) {
}

int fat32_mkdir(struct vnode* dir, struct vnode** target, const char* component_name) {
}

// file operations
int fat32_read(struct file* file, void* buf, uint64_t len) {
}

int fat32_write(struct file* file, const void* buf, uint64_t len) {
}