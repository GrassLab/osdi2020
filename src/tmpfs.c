#include "tmpfs.h"


#include "mm.h"
#include "printf.h"
#include "vfs.h"
#include "lib/string.h"


struct vnode_operations tmpfs_v_ops = {lookup_tmpfs, create_tmpfs};
struct file_operations tmpfs_f_ops ={read_tmpfs, write_tmpfs};


void setup_fs_tmpfs(struct filesystem *fs)
{
    fs->name = "tmpfs";
    fs->setup_mount = setup_mount_tmpfs;
}

struct vnode *create_tmpfs_vnode(int type)
{
    struct vnode *vnode = kmalloc(sizeof(struct vnode));
    vnode->mount = 0;//NULL
    vnode->v_ops = &tmpfs_v_ops;
    vnode->f_ops = &tmpfs_f_ops;
    vnode->cache = 0;//NULL
    vnode->type = type;
    vnode->internal = 0;//NULL
    return vnode;
}

int setup_mount_tmpfs(struct filesystem *fs, struct mount *mnt)
{
    struct vnode *vnode = create_tmpfs_vnode(VNODE_TYPE_DIR);
    mnt->fs= fs;
    mnt->root = vnode;

    return 0;
}

int lookup_tmpfs(struct vnode *vnode, struct vnode **target, const char *component_name)
{
    vnode->cache = kmalloc(sizeof(struct vnode_cache));
    memset(vnode->cache, 0U, sizeof(struct vnode_cache));
    *target = 0;
    return 0;
}

int create_tmpfs(struct vnode *vnode, struct vnode **target, const char *component_name)
{
    if(vnode->type != VNODE_TYPE_DIR){
        printf("[create tmpfs] Parent-node is not a directory!\n");
        *target = 0;
        return -1;
    }
    struct dentry* dentries = vnode->cache->dentries;
    int free_idx = 0;
    while(free_idx<NR_CHILD && dentries[free_idx].vnode!=0){
        free_idx++;
    }
    if(free_idx == NR_CHILD){
        printf("[create tmpfs] Direcotry can only has %d child entries!\n", NR_CHILD);
        *target = 0;
        return -1;
    }
    // create new node
    *target = create_tmpfs_vnode(VNODE_TYPE_REG);
    (*target)->cache = kmalloc(sizeof(struct vnode_cache));
    (*target)->cache->regbuf[0] = EOF;
    // add link to parent direcory
    strncpy(dentries[free_idx].name, component_name, DNAME_LEN);
    dentries[free_idx].vnode = *target;
    printf("[create tmpfs] file name: %s. Create success!\n", component_name);
    return 0;
}

int read_tmpfs(struct file *file, void *buf, unsigned len)
{
    printf("[read tmpfs] all context in the cache, should not reach here!\n");
    return -1;
}
//TODO
int write_tmpfs(struct file *file, const void *buf, unsigned len)
{
    char *reg_ptr = file->vnode->cache->regbuf + file->f_pos;
    const char *_buf = (const char *)buf;
    unsigned free_size = TMPFS_FILE_SIZE - file->f_pos - 1;
    unsigned cnt = (free_size<len) ? free_size : len;

    strncpy(reg_ptr, _buf, len);
    printf("[write tmpfs] %d byte(s) witre. f_pos %d -> %d\n", cnt, file->f_pos, file->f_pos+cnt);
    file->f_pos += cnt;
    reg_ptr[file->f_pos] = EOF;
    return cnt;
}