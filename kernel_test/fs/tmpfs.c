#include "vfs.h"
#include "tmpfs.h"
// #include "../kernel/uart.h"
// #include "../allocator/obj_allocator.h"

extern struct mount* rootfs;

void setup_vnode(struct vnode** vnode)
{
    
    struct vnode* ptr = vnode;
    printf("%x\n", (*vnode));
    *vnode = malloc(sizeof(struct vnode));
    printf("%x\n", (*vnode));
    ptr->v_ops = malloc(sizeof(struct vnode_operations));
    ptr->v_ops->lookup = tmpfs_lookup;
    ptr->v_ops->create = tmpfs_create;
    ptr->f_ops = malloc(sizeof(struct file_operations));
    ptr->f_ops->write = tmpfs_write;
    ptr->f_ops->read = tmpfs_read;
}

int setup_tmpfs_filesystem()
{
    struct filesystem* fs;
    fs = malloc(sizeof(struct filesystem));
    strcpy(fs->name, "tmpfs");
    fs->setup_mount = setup_tmpfs_mount;
    register_filesystem(fs); 
}

// int init_tmpfs_vnode()
// {

// }

int setup_tmpfs_mount(struct filesystem* fs, struct mount* mount)
{
    log("setup tmpfs mount\n");
    struct mount* tmpfs = malloc(sizeof(struct mount));
    tmpfs->fs = fs;

    log("set vnode.\n");
    // setup_vnode(&tmpfs->root);
    tmpfs->root = malloc(sizeof(struct vnode));
    tmpfs->root->v_ops = malloc(sizeof(struct vnode_operations));
    tmpfs->root->v_ops->lookup = tmpfs_lookup;
    tmpfs->root->v_ops->create = tmpfs_create;
    tmpfs->root->f_ops = malloc(sizeof(struct file_operations));
    tmpfs->root->f_ops->write = tmpfs_write;
    tmpfs->root->f_ops->read = tmpfs_read;
    printf("%x\n", tmpfs->root);
    tmpfs->root->mount = tmpfs;


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
    char* dst = internal->content;
    char* src = buf;
    while (len > 0) {
        *dst++ = *src++;
        len--;
        internal->content_size++;
    }
    // printf("internal->content: %s\n", internal->content);
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
    char* dst = buf;
    char* src = internal->content;
    int i = -1;
    for (i=0; i<len; i++) {
        if (*src == 0) {
            break;
        }
        *dst++ = *src++;
    }
    // printf("buf: %s, ret: %d\n", buf, i);
    return i;
}

int tmpfs_lookup(struct dentry* dentry, struct vnode** target, const char* component_name)
{
    log("tmpfs lookup.\n");
    struct dentry* child;
    if (strcmp(dentry->name, component_name) == 0) {
        *target = dentry->vnode;
        printf("%s\n", component_name);
        return 1;
    }
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
    vnode->v_ops->lookup = tmpfs_lookup;
    vnode->v_ops->create = tmpfs_create;
    vnode->f_ops = malloc(sizeof(struct file_operations));
    vnode->f_ops->write = tmpfs_write;
    vnode->f_ops->read = tmpfs_read;

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

    *target = vnode;
}