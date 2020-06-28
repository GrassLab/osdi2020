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
    return 0;
}

int create_tmpfs(struct vnode *vnode, struct vnode **target, const char *component_name)
{
    struct dentry* dentries = vnode->cache->dentries;
    int free_idx = 0;
    while(free_idx<NR_CHILD && dentries[free_idx].vnode!=0){
        free_idx++;
    }
    if(free_idx == NR_CHILD){
        printf("[create tmpfs] Direcotry can only has %d child entries!\n", NR_CHILD);
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
    const char *reg_ptr = file->vnode->cache->regbuf + file->f_pos;
    char *_buf = (char *)buf;
    unsigned cnt;
    for(cnt=0; (cnt<len && reg_ptr[cnt] != EOF); cnt++){
        _buf[cnt] = reg_ptr[cnt];
    }
    _buf[cnt] = '\0';
    printf("[read tmpfs] %d byte(s) read. f_pos %d -> %d\n", cnt, file->f_pos, file->f_pos+cnt);
    file->f_pos += cnt;
    return cnt;
}

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


// void list_tmpfs(struct dentry *dir)
// {
//     printf("\n[list file] dir: %s\n", dir->dname);
//     for(int i=0; i<dir->child_count; i++){
//         printf("File %d: '%s' \r\n",i,dir->child_dentry[i].dname);
//     }
//     return;
// }

//     return -1;
// }