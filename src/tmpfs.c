#include "pool.h"
#include "tmpfs.h"
#include "utils.h"
#include "vfs.h"

struct file_operations* tmpfs_f_ops;
struct vnode_operations* tmpfs_v_ops;

void init_fentry(struct fentry* fentry, struct vnode* vnode, const char* name) {
    fentry->vnode = vnode;
    strcpy(fentry->name, name);
}

int tmpfs_mount(struct filesystem* fs, struct mount* mount) {
    mount->fs = fs;
    struct fentry* fentry = (struct fentry*)kmalloc(sizeof(struct fentry));
    struct vnode* vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
    vnode->v_ops = tmpfs_v_ops;
    vnode->f_ops = tmpfs_f_ops;

    /* vnode->f_ops = tmpfs_f_ops; */
    fentry->type = FILE_TYPE_D;
    fentry->vnode = vnode;

    init_fentry(fentry, vnode, "/");
    for (int i = 0; i < DIR_MAX; i++) {
        fentry->list[i] = (struct fentry*)kmalloc(sizeof(struct fentry));
        fentry->list[i]->type = FILE_TYPE_N;
        fentry->list[i]->name[0] = 0;
    }

    mount->root->v_ops = tmpfs_v_ops;
    mount->root->f_ops = tmpfs_f_ops;
    mount->root->internal = (void*)fentry;
    return 1;
}

int tmpfs_lookup(struct vnode* dir_node, struct vnode** target,
                 const char* component_name) {
    for (int i = 0; i < DIR_MAX; i++) {
        if (!strcmp(((struct fentry*)dir_node->internal)->list[i]->name,
                    component_name)) {
            struct fentry* fentry =
                ((struct fentry*)dir_node->internal)->list[i];
            *target = fentry->vnode;
            return 1;
        }
    }
    return -1;
}

int tmpfs_create(struct vnode* dir_node, struct vnode** target,
                 const char* component_name) {
    for (int i = 0; i < DIR_MAX; i++) {
        if (((struct fentry*)dir_node->internal)->list[i]->type ==
            FILE_TYPE_N) {
            struct fentry* fentry =
                ((struct fentry*)dir_node->internal)->list[i];
            strcpy(fentry->name, component_name);
            fentry->type = FILE_TYPE_F;
            struct vnode* vnode = kmalloc(sizeof(struct vnode));
            vnode->v_ops = dir_node->v_ops;
            vnode->f_ops = dir_node->f_ops;
            vnode->internal = fentry;
            init_fentry(fentry, vnode, component_name);
            fentry->buf = kmalloc(sizeof(struct tmpfs_buf));
            fentry->buf->size = 0;

            *target = fentry->vnode;
            return 1;
        }
    }
    return -1;
}

int tmpfs_write(struct file* file, const void* buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        ((struct fentry*)file->vnode->internal)->buf->buffer[file->f_pos++] =
            ((char*)buf)[i];
        if (((struct fentry*)file->vnode->internal)->buf->size < file->f_pos) {
            ((struct fentry*)file->vnode->internal)->buf->size = file->f_pos;
        }
    }
    return len;
}
int tmpfs_read(struct file* file, void* buf, size_t len) {
    size_t ans = 0;
    for (size_t i = 0; i < len; i++) {
        ((char*)buf)[i] =
            ((struct fentry*)file->vnode->internal)->buf->buffer[file->f_pos++];
        ans++;
        if (ans == ((struct fentry*)file->vnode->internal)->buf->size) {
            break;
        }
    }
    asm volatile("fd:");
    return ans;
}
