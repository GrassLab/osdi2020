#include "vfs.h"

#include "mm.h"
#include "printf.h"
#include "lib/string.h"
#include "tmpfs.h"


struct mount *rootfs;
struct filesystem_manager fs_manager = {0};


void rootfs_init()
{
    // register tmpfs as rootfs
    struct filesystem *fs = kmalloc(sizeof(struct filesystem));
    setup_fs_tmpfs(fs);
    register_filesystem(fs);

    // mount rootfs
    rootfs = kmalloc(sizeof(struct mount));
    fs->setup_mount(fs, rootfs);
}

void register_filesystem(struct filesystem *fs)
{
    fs-> next = fs_manager.list;
    fs_manager.list = fs;
    fs_manager.nr_fs++;
}

int vfs_lookup(struct vnode *vnode, struct vnode **target, const char *component_name)
{
    if(vnode->type != VNODE_TYPE_DIR){
        printf("[vfs lookup] Can only lookup directory node!\n");
        return -1;
    }

    *target = 0;//NULL
    if(vnode->cache == 0){
        #ifdef __DEBUG
        printf("[vfs lookup] Lookup in true filesystem!\n");
        #endif
        vnode->v_ops->lookup(vnode, target, component_name);
    }else{// lookup in cache
        #ifdef __DEBUG
        printf("[vfs lookup] Lookup in cache!\n");
        #endif
        struct dentry *dentries = vnode->cache->dentries;
        for(int i=0; i<NR_CHILD; i++){
            if(strcmp(dentries[i].name, component_name) == 0){
                *target = dentries[i].vnode;
            }
        }
    }
    return 0;
}

struct file *vfs_open(const char *pathname, int flags) {
    struct vnode *target;
    vfs_lookup(rootfs->root, &target, pathname);
    if(target == 0)
        printf("[vfs open] Path component \"%s\" not exist!\n", pathname);
    // if(flags == O_CREAT){ // create and open file
    //     struct vnode *target;
    //     rootfs->root->v_ops->create(rootfs->dentry,&target,pathname);

    //     struct file *fd = kmalloc(sizeof(struct file));
    //     fd->vnode = target;
    //     fd->f_ops = target->f_ops;
    //     fd->f_pos = 0;
    //     return fd;
    // }else{ // open file
    //     struct vnode *target;
    //     int ret = rootfs->root->v_ops->lookup(rootfs->dentry,&target,pathname);

    //     if(ret == -1){
    //         if (strcmp(pathname,"/")!=0)
    //             printf("\n[vfs open] file not found!\n");
    //         return 0; // NULL
    //     }
    //     printf("\n[vfs open] filename: %s\n", pathname);
    //     struct file *fd = kmalloc(sizeof(struct file));
    //     fd->vnode = target;
    //     fd->f_ops = target->f_ops;
    //     fd->f_pos = 0;
    //     return fd;
    // }
    return 0;
}

// int vfs_close(struct file *file){
//     printf("\n[vfs close] Close file at %d\n", file);
//     kfree((void *)file);
//     return 0;
// }

// int vfs_write(struct file *file, const void *buf, unsigned len){
//     int x = file->f_ops->write(file,buf,len);
//     if(x == -1){
//         printf("\n[vfs write] Exceed max length of file\n");
//     }
// 	return x;
// }

// int vfs_read(struct file *file, void *buf, unsigned len){
// 	return file->f_ops->read(file,buf,len);
// } 