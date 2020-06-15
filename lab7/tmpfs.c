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

int setup_mount_tmpfs(struct filesystem *fs, struct mount *mount)
{
    printf("%s\n", fs->name);
    printf("%x\n", mount);
    return 0;
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
    return 0;
}

int read_tmpfs(struct file *file, void *buf, unsigned long len)
{
    return 0;
}