#include "vfs.h"
#include "tmpfs.h"

extern struct mount* rootfs;

int setup_tmpfs_filesystem()
{
    struct filesystem* fs;
    fs = malloc(sizeof(struct filesystem));
    strcpy(fs->name, "tmpfs");
    fs->setup_mount = setup_tmpfs_mount;
    register_filesystem(fs); 
}

int setup_tmpfs_mount(struct filesystem* fs, struct mount* mount)
{
    log("setup tmpfs mount\n");
    struct mount* tmpfs = malloc(sizeof(struct mount));
    tmpfs->fs = fs;

    log("set vnode.\n");
    tmpfs->root = malloc(sizeof(struct vnode));
    tmpfs->root->mount = tmpfs;
    tmpfs->root->v_ops = malloc(sizeof(struct vnode_operations));
    tmpfs->root->v_ops->lookup = tmpfs_lookup;
    tmpfs->root->v_ops->create = tmpfs_create;
    tmpfs->root->f_ops = malloc(sizeof(struct file_operations));
    tmpfs->root->f_ops->write = tmpfs_write;
    tmpfs->root->f_ops->read = tmpfs_read;


    log("set dentry.\n");
    tmpfs->dentry = malloc(sizeof(struct dentry));
    tmpfs->dentry->vnode = tmpfs->root;
    tmpfs->dentry->child_num = 0;
    strcpy(tmpfs->dentry->name, "/");
    
    rootfs = tmpfs; // directly assign tmpfs to rootfs 
    return 0;
}

int tmpfs_write(struct file* file, const void* buf, int len)
{
    log("tmpfs write.\n");
    struct vnode* vnode = file->vnode;
    struct tmpfs_internal* internal = vnode->internal;
    
    if (internal->type != FILE) {
        printf("ERROR: write non-FILE\n");
        return -1;
    }
    char* dst = buf;
    char* src = internal->content;
    while (len > 0) {
        *dst++ = *src++;
        len--;
        internal->content_size++;
    }
    return internal->content_size;
}

int tmpfs_read(struct file* file, void* buf, int len)
{
    log("tmpfs read.\n");
    struct vnode* vnode = file->vnode;
    struct tmpfs_internal* internal = vnode->internal;
    
    if (internal->type != FILE) {
        printf("ERROR: read non-FILE\n");
        return -1;
    }
    char* dst = internal->content;
    char* src = buf;
    while (len > 0) {
        *dst++ = *src++;
        len--;
        internal->content_size++;
    }
    return internal->content_size;
}

int tmpfs_lookup(struct dentry* dentry, struct vnode** target, const char* component_name)
{
    log("tmpfs lookup.\n");
    struct dentry* child;
    for (int i=0; i<dentry->child_num; i++)
    {
        child = dentry->child[i];
        if (strcmp(child->name, component_name) == 0) {
            *target = child->vnode;
            return 1;
        }
    }
    return 0;
}

int tmpfs_create(struct dentry* parent, struct vnode** target, const char* component_name)
{
    log("tmpfs create.\n");
    struct vnode* vnode = malloc(sizeof(struct vnode));
    vnode->v_ops = malloc(sizeof(struct vnode_operations));
    vnode->f_ops = malloc(sizeof(struct file_operations));
    struct tmpfs_internal* internal = malloc(sizeof(struct tmpfs_internal));
    internal->type = FILE;
    internal->content_size = 0;
    vnode->internal = internal;

    struct dentry* child = malloc(sizeof(struct dentry));
    child->vnode = vnode;
    child->parent = parent;
    parent->child[parent->child_num++] = child;
    child->child_num = 0;
    strcpy(child->name, component_name);
}