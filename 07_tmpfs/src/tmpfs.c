#include "fs/tmpfs.h"
#include "mm.h"
#include "peripherals/uart.h"
#include "utils.h"

struct vnode_operations* tmpfs_v_ops;
struct file_operations* tmpfs_f_ops;

void set_tmpfs_vnode(struct vnode* vnode) {
    vnode->v_ops = tmpfs_v_ops;
    vnode->f_ops = tmpfs_f_ops;
}

int setup_mount_tmpfs(struct filesystem* fs, struct mount* mount) {
    uart_puts("setting ");
    uart_puts(fs->name);
    uart_puts(" ...\n");
    return 0;
}

int lookup_tmpfs(struct dentry* dir, struct vnode** target, const char* component_name) {
    for (int i = 0; i < dir->child_count; ++ i) {
        if (strcmp(dir->child_dentry[i]->dname, component_name) == -1) {
            *target = dir->child_dentry[i]->vnode;
            return 0;
        }
    }
    return -1;
}

int create_tmpfs(struct dentry* dir, struct vnode** target, const char* component_name) {
    struct vnode *vnode = (struct vnode*) dynamic_alloc(sizeof(struct vnode));
    set_tmpfs_vnode(vnode);

    struct tmpfs_node *tmpfs_node = (struct tmpfs_node*) dynamic_alloc(sizeof(struct tmpfs_node));
    tmpfs_node->flag = REGULAR_FILE;
    vnode->internal = (void*) tmpfs_node;
    
    struct dentry* child = (struct dentry*) dynamic_alloc(sizeof(struct dentry));
    set_dentry(child, vnode, component_name);

    if (dir->child_count < MAX_CHILD) {
        dir->child_dentry[dir->child_count++] = child;
    } else {
        return -1;
    }
    *target = vnode;
    return 0;
}

int write_tmpfs(struct file* file, const void* buf, size_t len) {
    struct vnode *vnode = file->vnode;
    
    char *buffer = (char *)buf;
    struct tmpfs_node *file_node = (struct tmpfs_node*)vnode->internal;
    
    if (file_node->flag != REGULAR_FILE) {
        return -1;
    }
    
    char *file_content = file_node->buffer;
    unsigned int i = 0;
    for(; i < len; ++i) {
        file_content[file->f_pos + i] = buffer[i];
    }
    file_content[i] = EOF;
    return i;
}

int read_tmpfs(struct file* file, void* buf, size_t len) {
    struct vnode* vnode = file->vnode;

    struct tmpfs_node *file_node = (struct tmpfs_node *) vnode->internal;
    char *file_content = file_node->buffer;
    
    if (file_node ->flag != REGULAR_FILE) {
        return -1;
    }

    char *buffer = (char *)buf;
    unsigned int i = 0;
    for (; i < len; ++ i) {
        if (file_content[i] != (unsigned char)(EOF)) {
            buffer[i] = file_content[i];
        } else {
            break;
        }
    }
    return i;
}
