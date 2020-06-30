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
        *target = 0;//NULL
        for(int i=0; i<NR_CHILD; i++){
            if(strcmp(dentries[i].name, component_name) == 0){
                *target = dentries[i].vnode;
                break;
            }
        }// cache will only set once.
    }
    return 0;
}

void setup_fd(struct file* fd, struct vnode *vnode)
{
    fd->vnode = vnode;
    fd->f_ops = vnode->f_ops;
    fd->f_pos = 0;
}

int __walk_path(struct vnode **stop_at, const char *pathname)
{
    char *_pathname;
    if(pathname[0]!='/'){
        printf("[vfs open] Currently only support absolute path!\n");
        return -1;
    }
    if(strlen(pathname) > MAX_PATH_LEN-1){
        printf("[vfs open] Path length out of limit!\n");
        return -1;
    }else{
        _pathname = kmalloc(MAX_PATH_LEN);
        strncpy(_pathname, pathname, MAX_PATH_LEN);
    }
    char *component_name = strtok(_pathname, '/');
    struct vnode *curr, *next;
    int ur_unreached = 0;
    curr = rootfs->root;
    while(component_name && *component_name != '\0'){
        vfs_lookup(curr, &next, component_name);
        if(next){
            component_name = strtok(0, '/');
            curr = next;
        }else{
            printf("[walk path] Component \"%s\" not exist!\n", component_name);
            ur_unreached++;
            while(strtok(0, '/')){
                ur_unreached++;
            }
            break;
        }
    }
    kfree(_pathname);
    *stop_at = curr;
    return ur_unreached;
}

void vfs_ls(const char *pathname)
{
    printf("[vfs_ls] \"%s\"\n", pathname);
    struct vnode *dir_node;
    if(__walk_path(&dir_node, pathname) == 0 && dir_node->type == VNODE_TYPE_DIR){
        if(dir_node->cache == 0){
            struct vnode *target;
            dir_node->v_ops->lookup(dir_node, &target, "");
        }
        struct dentry *dentries = dir_node->cache->dentries;
        for(int i=0; i<NR_CHILD; i++){
            if(dentries[i].vnode)
                printf("%s\t", dentries[i].name);
        }
        printf("\n");
    }
}

struct file *vfs_open(const char *pathname, int flags)
{
    // if(pathname[0]!='/'){
    //     printf("[vfs open] Currently only support absolute path!\n");
    //     return 0;
    // }
    // char *_pathname;
    // if(strlen(pathname) > MAX_PATH_LEN-1){
    //     printf("[vfs open] Path length out of limit!\n");
    //     return 0;
    // }else{
    //     _pathname = kmalloc(MAX_PATH_LEN);
    //     strncpy(_pathname, pathname, MAX_PATH_LEN);
    // }
    
    // char *component_name = strtok(_pathname, '/');
    // struct vnode *target, *base_dir = rootfs->root;
    // while(component_name){
    //     vfs_lookup(base_dir, &target, component_name);
    //     if(target){
    //         component_name = strtok(0, '/');
    //         base_dir = target;
    //         continue;
    //     }else{
    //         if(strtok(0, '/') != 0){
    //             printf("[vfs open] Can't open file. Directory \"%s\" not exist!\n", component_name);
    //             kfree(_pathname);
    //             return 0;
    //         }else if(flags != O_CREAT){
    //             printf("[vfs open] Can't open file. File not exist!\n", component_name);
    //             kfree(_pathname);
    //             return 0;
    //         }else if(base_dir->v_ops->create(base_dir, &target, component_name)){
    //             printf("[vfs open] Fail to create file %s!\n", component_name);
    //             kfree(_pathname);
    //             return 0;
    //         }
    //     }
    // }
    struct vnode *stop_at, *target = 0;
    int nr_unreached = __walk_path(&stop_at, pathname);
    if(nr_unreached == 1 && flags == O_CREAT){
        char *file_name = strrchr(pathname, '/')+1;
        stop_at->v_ops->create(stop_at, &target, file_name);
    }else if(nr_unreached == 0 && stop_at->type == VNODE_TYPE_REG){
        target = stop_at;
    }
    if(target){
        struct file *fd = kmalloc(sizeof(struct file));
        setup_fd(fd, target);
        return fd;
    }else{
        printf("[vfs open] Fail to open file %s!\n", pathname);
        return 0;
    }
}

int vfs_close(struct file *file)
{
    printf("[vfs close] Close file descriptor @ 0x%X\n", file);
    kfree((void *)file);
    return 0;
}

int vfs_read(struct file *file, void *buf, unsigned len)
{
    if(file->vnode->cache != 0){
        const char *reg_ptr = file->vnode->cache->regbuf + file->f_pos;
        char *_buf = (char *)buf;
        unsigned cnt;
        for(cnt=0; (cnt<len && reg_ptr[cnt] != EOF); cnt++){
            _buf[cnt] = reg_ptr[cnt];
        }
        _buf[cnt] = '\0';
        printf("[read fat] %d byte(s) read. f_pos %d -> %d\n", cnt, file->f_pos, file->f_pos+cnt);
        file->f_pos += cnt;
        return cnt;
    }else{
        return file->f_ops->read(file, buf, len);
    }
}

int vfs_write(struct file *file, const void *buf, unsigned len)
{
    return file->f_ops->write(file, buf, len);
}
