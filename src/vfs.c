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

void setup_fd(struct file* fd, struct vnode *vnode)
{
    fd->vnode = vnode;
    fd->f_ops = vnode->f_ops;
    fd->f_pos = 0;
}

struct file *vfs_open(const char *pathname, int flags)
{
    if(pathname[0]!='/'){
        printf("[vfs open] Currently only support absolute path!\n");
        return 0;
    }
    char *_pathname;
    if(strlen(pathname) > MAX_PATH_LEN-1){
        printf("[vfs open] Pathn lenth out of limit!\n");
        return 0;
    }else{
        _pathname = kmalloc(MAX_PATH_LEN);
        strncpy(_pathname, pathname, MAX_PATH_LEN);
    }
    
    char *component_name = strtok(_pathname, '/');
    struct vnode *target, *base_dir = rootfs->root;
    while(component_name){
        vfs_lookup(base_dir, &target, component_name);
        if(target){
            component_name = strtok(0, '/');
            base_dir = target;
            continue;
        }else{
            if(strtok(0, '/') != 0){
                printf("[vfs open] Can't open file. Directory \"%s\" not exist!\n", component_name);
                kfree(_pathname);
                return 0;
            }else if(flags != O_CREAT){
                printf("[vfs open] Can't open file. File not exist!\n", component_name);
                kfree(_pathname);
                return 0;
            }else if(base_dir->v_ops->create(base_dir, &target, component_name)){
                printf("[vfs open] Fail to create file %s!\n", component_name);
                kfree(_pathname);
                return 0;
            }
        }
    }
    kfree(_pathname);
    struct file *fd = kmalloc(sizeof(struct file));
    setup_fd(fd, target);
    return fd;
}

int vfs_close(struct file *file){
    printf("[vfs close] Close file descriptor @ 0x%X\n", file);
    kfree((void *)file);
    return 0;
}

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