#include "tmpfs.h"

#include "list.h"
#include "mm.h"
#include "my_string.h"
#include "uart0.h"

struct vnode_operations* tmpfs_v_ops = NULL;
struct file_operations* tmpfs_f_ops = NULL;

struct vnode* tmpfs_create_vnode(struct dentry* dentry) {
    struct vnode* vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
    vnode->dentry = dentry;
    vnode->f_ops = tmpfs_f_ops;
    vnode->v_ops = tmpfs_v_ops;
    return vnode;
}

struct dentry* tmpfs_create_dentry(struct dentry* parent, const char* name, int type) {
    struct dentry* dentry = (struct dentry*)kmalloc(sizeof(struct dentry));
    strcpy(dentry->name, name);
    dentry->parent = parent;
    list_head_init(&dentry->list);
    list_head_init(&dentry->childs);
    if (parent != NULL) {
        list_add(&dentry->list, &parent->childs);
    }
    dentry->vnode = tmpfs_create_vnode(dentry);
    dentry->mountpoint = NULL;
    dentry->type = type;
    return dentry;
}

// error code: -1: already register
int tmpfs_register() {
    if (tmpfs_v_ops != NULL && tmpfs_f_ops != NULL) {
        return -1;
    }
    tmpfs_v_ops = (struct vnode_operations*)kmalloc(sizeof(struct vnode_operations));
    tmpfs_v_ops->create = tmpfs_create;
    tmpfs_v_ops->lookup = tmpfs_lookup;
    tmpfs_v_ops->ls = tmpfs_ls;
    tmpfs_v_ops->mkdir = tmpfs_mkdir;
    tmpfs_f_ops = (struct file_operations*)kmalloc(sizeof(struct file_operations));
    tmpfs_f_ops->read = tmpfs_read;
    tmpfs_f_ops->write = tmpfs_write;
    return 0;
}

int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount) {
    mount->fs = fs;
    mount->root = tmpfs_create_dentry(NULL, "/", DIRECTORY);
    return 0;
}

// vnode operations
int tmpfs_lookup(struct vnode* dir, struct vnode** target, const char* component_name) {
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

int tmpfs_create(struct vnode* dir, struct vnode** target, const char* component_name) {
    // create tmpfs internal structure
    struct tmpfs_internal* file_node = (struct tmpfs_internal*)kmalloc(sizeof(struct tmpfs_internal));

    // create child dentry
    struct dentry* d_child = tmpfs_create_dentry(dir->dentry, component_name, REGULAR_FILE);
    d_child->vnode->internal = (void*)file_node;

    *target = d_child->vnode;
    return 0;
}

int tmpfs_ls(struct vnode* dir) {
    struct list_head* p = &dir->dentry->childs;
    list_for_each(p, &dir->dentry->childs) {
        struct dentry* dentry = list_entry(p, struct dentry, list);
        if (dentry->type == DIRECTORY) {
            uart_printf("d: %s\n", dentry->name);
        }
        else if (dentry->type == REGULAR_FILE) {
            uart_printf("f: %s\n", dentry->name);
        }
    }
    return 0;
}

// file operations
int tmpfs_read(struct file* file, void* buf, uint64_t len) {
    struct tmpfs_internal* file_node = (struct tmpfs_internal*)file->vnode->internal;

    char* dest = (char*)buf;
    char* src = &file_node->buf[file->f_pos];
    uint64_t i = 0;
    for (; i < len && src[i] != (uint8_t)EOF; i++) {
        dest[i] = src[i];
    }
    return i;
}

int tmpfs_write(struct file* file, const void* buf, uint64_t len) {
    struct tmpfs_internal* file_node = (struct tmpfs_internal*)file->vnode->internal;

    char* dest = &file_node->buf[file->f_pos];
    char* src = (char*)buf;
    uint64_t i = 0;
    for (; i < len && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = EOF;
    return i;
}

int tmpfs_mkdir(struct vnode* dir, struct vnode** target, const char* component_name) {
    // create tmpfs internal structure
    struct tmpfs_internal* dir_node = (struct tmpfs_internal*)kmalloc(sizeof(struct tmpfs_internal));

    // create child dentry
    struct dentry* d_child = tmpfs_create_dentry(dir->dentry, component_name, DIRECTORY);
    d_child->vnode->internal = (void*)dir_node;

    *target = d_child->vnode;
    return 0;
}
