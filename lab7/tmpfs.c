#include "vfs.h"
#include "tmpfs.h"
#include "obj_alloc.h"
#include "uart.h"
#include "string.h"

void set_tmpfs_vnode(struct vnode *vnode)
{
    // create root directory's vnode
    vnode->v_ops = tmpfs_v_ops;
    vnode->f_ops = tmpfs_f_ops;
}

int lookup_tmpfs(struct dentry *dir, struct vnode **target,
                 char *component_name)
{
    printf("lookup\n");
    for (int i = 0; i < dir->child_count; i++)
    {
        if (strcmp(dir->child_dentry[i].dname, component_name) != 0)
        {
            printf("find\n");
            *target = dir->child_dentry[i].vnode;
            return 0;
        }
    }

    return -1;
}

int create_tmpfs(struct dentry *dir, struct vnode **target,
                 char *component_name)
{
    int get = varied_size_alloc(sizeof(struct vnode));
    struct vnode *vnode = (struct vnode *)fix_object_array[get].obj;
    vnode->objid = get;
    set_tmpfs_vnode(vnode);

    get = varied_size_alloc(sizeof(struct tmpfs_node));
    struct tmpfs_node *in = (struct tmpfs_node *)fix_object_array[get].obj;
    in->objid = get;
    vnode->internal = in;

    get = varied_size_alloc(sizeof(struct dentry));

    struct dentry *child = (struct dentry *)fix_object_array[get].obj;
    child->objid = get;

    set_dentry(child, vnode, component_name);

    if (dir->child_count < MAX_CHILD)
        dir->child_dentry[dir->child_count++] = *child;

    printf("create file %s\n", dir->child_dentry[dir->child_count - 1].dname);

    *target = vnode;
    return 0;
}

int write_tmpfs(struct file *file, void *buf, unsigned long len)
{
    printf("write %s \n", (char *)buf);
    struct vnode *vnode = file->vnode;
    struct tmpfs_node *file_node = (struct tmpfs_node *)vnode->internal;
    char *write_buffer = (char *)buf;
    char *file_buffer = file_node->buffer;

    int write_count = 0;
    for (int i = 0; i < len; i++)
    {
        file_buffer[file->f_pos + i] = write_buffer[i];
        write_count++;
    }

    file_buffer[write_count] = (unsigned char)EOF;
    return write_count;
}

int read_tmpfs(struct file *file, void *buf, unsigned long len)
{
    printf("read\n");
    struct vnode *vnode = file->vnode;
    struct tmpfs_node *file_node = (struct tmpfs_node *)vnode->internal;
    char *read_buffer = (char *)buf;
    char *file_buffer = file_node->buffer;

    int i;
    for (i = 0; i < len; i++)
    {
        if (file_buffer[i] != (unsigned char)EOF)
            read_buffer[i] = file_buffer[i];
        else
        {
            printf("get EOF!!\n");
            break;
        }
    }
    return i;
}